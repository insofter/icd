DROP TABLE IF EXISTS events;

CREATE TABLE events (
  id INTEGER PRIMARY KEY,
  itd_id INTEGER NOT NULL, -- id of input transition detector (e.g. photocell)
  dtmms INTEGER NOT NULL, -- Unix time, but expressed in miliseconds
  state INTEGER NOT NULL, -- new state of the detector
  flags INTEGER DEFAULT 1, -- flags 1 - NOT SENT, 0 - SENT
  UNIQUE(itd_id, dtmms)
);

CREATE INDEX events_dtmms_index ON events (dtmms);

