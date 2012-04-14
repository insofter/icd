DROP TABLE IF EXISTS tests;

CREATE TABLE tests (
  id INTEGER PRIMARY KEY,
  itd TEXT NOT NULL,        -- input transition detector (e.g. photocell) (e.g. itd0, itd1 etc)
  dtm INTEGER NOT NULL,     -- Unix time, expressed in seconds
  result INTEGER NOT NULL,  -- result of the test
  UNIQUE(itd, dtm)
);

CREATE INDEX tests_dtm_index ON tests (dtm);

