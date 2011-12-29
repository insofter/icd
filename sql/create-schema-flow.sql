DROP TABLE IF EXISTS flow;

CREATE TABLE flow (
  id INTEGER PRIMARY KEY,
  itd TEXT NOT NULL,           -- input transition detector (e.g. photocell) (e.g. itd0, itd1 etc)
  dtm INTEGER NOT NULL,        -- the beginning of the entry; Unixtime in secs
  cnt INTEGER DEFAULT 0,       -- number of registered crossings
  dark_time INTEGER DEFAULT 0, -- time the detector was blocked in milisecs
  work_time INTEGER DEFAULT 0, -- time the detector was operational within current flow entry
  test TEXT,                   -- TODO
  flags INTEGER DEFAULT 1,     -- 1 - NOT SENT, 0 - SENT
  UNIQUE(itd, dtm)
);
