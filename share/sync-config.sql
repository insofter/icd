-- main - the db that has outdated data
-- extern -- the db that has up-to-date data

DELETE FROM config;
DELETE FROM config_section;

INSERT INTO config_section SELECT * FROM externdb.config_section e
  WHERE e.name != "current";

INSERT INTO config SELECT * FROM externdb.config e
  WHERE e.section_id != (SELECT x.id FROM externdb.config_section x
    WHERE x.name == 'current');
 
INSERT INTO config_section (name) VALUES ('current');
