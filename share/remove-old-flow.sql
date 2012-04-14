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
  "flow-entry-retention-period-sec",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "device" AND c.key == "flow-entry-retention-period-mins") * 60
);

-- Remove old flow entries (but only those that has been sent to a remote server)
DELETE FROM flow WHERE dtm < ((SELECT value FROM t WHERE key == "current-time")
  - (SELECT value FROM t WHERE key == "flow-entry-retention-period-sec"))
  AND flags == 0;

DROP TABLE t;

