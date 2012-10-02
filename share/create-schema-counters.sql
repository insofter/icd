DROP TABLE IF EXISTS counters;

CREATE TABLE counters (
  id INTEGER PRIMARY KEY, -- id == itd in flow table
  name TEXT NOT NULL,                -- name of counter
  enabled BOOLEAN NOT NULL DEFAULT TRUE,
  engage_delay_ms INTEGER NOT NULL DEFAULT 200,
  release_delay_ms INTEGER NOT NULL DEFAULT 200,
  active_low BOOLEAN NOT NULL DEFAULT TRUE,
  direction_detector INTEGER,
  thickness_detector INTEGER,
  disabling_input INTEGER,
  UNIQUE(id)
);

