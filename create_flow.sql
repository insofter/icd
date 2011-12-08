DROP TABLE IF EXISTS flow;

CREATE TABLE flow (
  id INTEGER PRIMARY KEY,
  itd_id INTEGER NOT NULL, -- id of the input transition detector (e.g photocell)
  dtm INTEGER NOT NULL, -- the beginning of the entry; Unix time in secs
  cnt INTEGER DEFAULT 0, -- number of registered crossings
  dark_time INTEGER DEFAULT 0, -- time the detector was blocked in milisecs
  work_time INTEGER DEFAULT 0, -- time the detector was operational within current flow entry
  test TEXT, -- TODO
  flags INTEGER DEFAULT 1, -- 1 - NOT SENT, 0 - SENT
  UNIQUE(itd_id, dtm)
);
