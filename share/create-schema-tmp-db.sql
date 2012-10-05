CREATE TABLE config (
  id INTEGER PRIMARY KEY,
  section TEXT NOT NULL,
  key TEXT NOT NULL,
  value TEXT NOT NULL,
  UNIQUE(section, key)
);

CREATE TABLE flow (
  id INTEGER PRIMARY KEY,
  counter_id INTEGER NOT NULL, -- input transition detector (e.g. photocell) (e.g. itd0, itd1 etc)
  dtm INTEGER NOT NULL,        -- the beginning of the entry; Unixtime in secs
                               -- unixtime of hour 12:00 means entry of 12:00:00 to 12:59:59
  cnt INTEGER DEFAULT 0,       -- number of registered crossings
  dark_time INTEGER DEFAULT 0, -- time the detector was blocked in milisecs
  work_time INTEGER DEFAULT 0, -- time the detector was operational within current flow entry
  flags INTEGER DEFAULT 1,     -- 1 - NOT SENT, 0 - SENT
  UNIQUE(counter_id, dtm)
);

