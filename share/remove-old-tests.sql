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
  "test-retention-period-sec",
  (SELECT c.value FROM config c JOIN config_section s ON c.section_id == s.id
     WHERE s.name == "device" AND c.key == "test-retention-period-mins") * 60
);

-- Remove old tests
DELETE FROM tests WHERE dtm < ((SELECT value FROM t WHERE key == "current-time")
  - (SELECT value FROM t WHERE key == "test-retention-period-sec"));

DROP TABLE t;

