-- DROP TABLE IF EXISTS flow;

CREATE TABLE flow (
  id INTEGER PRIMARY KEY,
  counter_id INTEGER NOT NULL,
  dtm INTEGER NOT NULL,        -- the beginning of the entry; Unixtime in secs
  cnt INTEGER DEFAULT 0,       -- number of registered crossings
  dark_time INTEGER DEFAULT 0, -- time the detector was blocked in milisecs
  work_time INTEGER DEFAULT 0, -- time the detector was operational within current flow entry
  test INTEGER DEFAULT NULL,   -- itd device test result
  flags INTEGER DEFAULT 1,     -- 1 - NOT SENT, 0 - SENT
  UNIQUE(counter_id, dtm)
);


CREATE INDEX dtm_index ON flow ( dtm ASC, flags );--index to make searching faster
CREATE INDEX id_index ON flow ( id );--index to make searching faster

--  test TEXT,                   -- TODO
