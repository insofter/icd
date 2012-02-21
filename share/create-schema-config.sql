DROP TABLE IF EXISTS config;
DROP TABLE IF EXISTS config_section;

CREATE TABLE config_section (
  id INTEGER PRIMARY KEY,
  name TEXT NOT NULL,
  UNIQUE(name)
);

CREATE TABLE config (
  id INTEGER PRIMARY KEY,
  section_id INTEGER NOT NULL,
  key TEXT NOT NULL,
  value TEXT NOT NULL,
  UNIQUE(section_id, key),
  FOREIGN KEY(section_id) REFERENCES config_section(id) ON DELETE CASCADE
);

