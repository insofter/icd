-- For convenience we create temporary table that holds
-- some settings so the queries below are slightly less
-- complicated
CREATE TEMP TABLE t (
  key TEXT PRIMARY KEY,
  value INTEGER
);

INSERT INTO t (key, value) VALUES (
  "aggr-period-secs",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "device" AND c.key == "aggr-period-mins") * 60
);

INSERT INTO t (key, value) VALUES (
  "current-time",
  CAST(strftime("%s", "now") AS INTEGER)
);

INSERT INTO t (key, value) VALUES (
  "current-entry-time",
  ((SELECT value FROM t WHERE key == "current-time")
      / (SELECT value FROM t WHERE key == "aggr-period-secs"))
    * (SELECT value FROM t WHERE key == "aggr-period-secs")
);

INSERT INTO t (key, value) VALUES (
  "last-aggr-time",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "current" AND c.key == "last-aggr-time")
);

INSERT INTO t (key, value) VALUES (
  "last-entry-time",
  ((SELECT value FROM t WHERE key == "last-aggr-time")
      / (SELECT value FROM t WHERE key == "aggr-period-secs"))
    * (SELECT value FROM t WHERE key == "aggr-period-secs")
);

INSERT INTO t (key, value) VALUES (
  "event-retention-period-sec",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "device" AND c.key == "event-retention-period-mins") * 60
);

INSERT INTO t (key, value) VALUES (
  "flow-entry-retention-period-sec",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "device" AND c.key == "flow-entry-retention-period-mins") * 60
);

-- Create missing entries in flow table; there is a trick used here
-- that takes advantage of "recursive" trigger - we insert only 
-- the "current" entry for each device and the trigger inserts
-- all missing entried up to the time of last aggregation
CREATE TEMP TRIGGER flow_before_insert_trigger BEFORE INSERT ON flow
WHEN NEW.dtm > (SELECT value FROM t WHERE key == "last-aggr-time")
BEGIN
  INSERT INTO flow (itd, dtm) VALUES (
    NEW.itd,
    NEW.dtm - (SELECT value FROM t WHERE key == "aggr-period-secs"));
END;

INSERT OR IGNORE INTO flow (itd, dtm) 
  SELECT s.name, (SELECT value FROM t WHERE key == "current-entry-time")
    FROM config c JOIN config_section s ON c.section_id == s.id 
    WHERE s.name LIKE "itd%" AND c.key == "enabled" AND c.value == "yes";

DROP TRIGGER flow_before_insert_trigger;

-- Execute aggregation
UPDATE flow SET cnt = flow.cnt + (SELECT count(*) FROM events e1 JOIN events e2
    ON e2.dtmms == (SELECT MIN(x.dtmms) FROM events x WHERE x.dtmms > e1.dtmms)
      AND e2.itd == e1.itd
    WHERE e1.dtmms >= flow.dtm * 1000
      AND e1.dtmms < (flow.dtm + (SELECT value FROM t WHERE key == "aggr-period-secs")) * 1000
      AND e1.itd == flow.itd AND e1.state == 1 AND e1.flags != 0),
  dark_time = flow.dark_time + IFNULL((SELECT SUM(max(flow.dtm * 1000, min((flow.dtm +
      (SELECT value FROM t WHERE key == "aggr-period-secs")) * 1000, e2.dtmms))
    - max(flow.dtm * 1000, min((flow.dtm +
      (SELECT value FROM t WHERE key == "aggr-period-secs")) * 1000, e1.dtmms)))
    FROM events e1 JOIN events e2
      ON e2.dtmms == (SELECT MIN(x.dtmms) FROM events x WHERE x.dtmms > e1.dtmms)
      AND e2.itd == e1.itd
    WHERE e1.itd == flow.itd AND e1.state == 1 AND e1.flags != 0),0),
  work_time = work_time + (max(dtm, min(dtm + (SELECT value FROM t WHERE key == "aggr-period-secs"),
      (SELECT value FROM t WHERE key == "current-time")))
    - max(dtm, min(dtm + (SELECT value FROM t WHERE key == "aggr-period-secs"),
      (SELECT value FROM t WHERE key == "last-aggr-time")))) * 1000
  WHERE flow.dtm >= (SELECT value FROM t WHERE key == "last-entry-time");

-- Mark all events as aggregated; The only exceptions are events that starts
-- an unfinished crossing (we got 0->1 transition but  not yet 1->0);
-- such "unfinished" events are also not counted for cnt, dark_time
UPDATE events SET flags = 0
 WHERE id IN (SELECT e1.id FROM events e1 JOIN events e2
     ON e2.dtmms == (SELECT MIN(x.dtmms) FROM events x WHERE x.dtmms > e1.dtmms)
     AND e2.itd == e1.itd)
  AND flags != 0;

-- Update last-aggr-time, so next time this script runs it starts from
-- when we just has finised
UPDATE config SET value = (SELECT value FROM t WHERE key == "current-time")
  WHERE section_id = (SELECT id FROM config_section WHERE name == "current")
    AND key == "last-aggr-time";

DROP TABLE t;
