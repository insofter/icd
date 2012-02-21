-- For convenience we create temporary table that holds
-- some settings so the queries below are slightly less
-- complicated
CREATE TEMP TABLE t (
  key TEXT PRIMARY KEY,
  value INTEGER
);

INSERT INTO t (key, value) VALUES (
  "current-time",
  CAST(strftime("%s", "now") AS INTEGER)
);

INSERT INTO t (key, value) VALUES (
  "event-retention-period-sec",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "device" AND c.key == "event-retention-period-mins") * 60
);

-- Remove old events (but only those that have been aggregated)
DELETE FROM events WHERE dtmms < ((SELECT value FROM t WHERE key == "current-time")
  - (SELECT value FROM t WHERE key == "event-retention-period-sec")) * 1000
  AND flags == 0;

DROP TABLE t;

