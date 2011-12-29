-- main - the db that has outdated data
-- extern -- the db that has up-to-date data

-- first step - config_section table

-- remove all records that exists in main db no longer exist in extern data;
-- this also removes all config records belonging to this section
DELETE FROM config_section WHERE
 id IN (SELECT m.id FROM config_section m LEFT JOIN externdb.config_section e 
   ON m.id == e.id WHERE e.id IS NULL);

-- update all records that are different; IS NOT is like '!=' but takes care of NULLs
UPDATE config_section SET
  name = (SELECT e.name FROM externdb.config_section e
    WHERE e.id == config_section.id)
  WHERE id IN (SELECT m.id FROM config_section m JOIN externdb.config_section e
    ON m.id == e.id WHERE m.name IS NOT e.name);

-- insert all records that are missing
INSERT INTO config_section (id, name)
  SELECT e.id, e.name FROM externdb.config_section e LEFT JOIN config_section m
    ON m.id == e.id WHERE m.id IS NULL;

-- second step - config table

-- remove all records that exists in main db no longer exist in extern data
DELETE FROM config WHERE
 id IN (SELECT m.id FROM config m LEFT JOIN externdb.config e 
   ON m.id == e.id WHERE e.id IS NULL);

-- update all records that are different; IS NOT is like '!=' but takes care of NULLs
UPDATE config SET 
  section_id = (SELECT e.section_id FROM externdb.config e
    WHERE e.id == config.id),
  key = (SELECT e.key FROM externdb.config e
    WHERE e.id == config.id),
  value = (SELECT e.value FROM externdb.config e
    WHERE e.id == config.id)
  WHERE id IN (SELECT m.id FROM config m JOIN externdb.config e
    ON m.id == e.id
    WHERE m.section_id IS NOT e.section_id OR m.key IS NOT e.key OR m.value IS NOT  e.value);

-- insert all records that are missing
INSERT INTO config (id, section_id, key, value)
  SELECT e.id, e.section_id, e.key, e.value FROM externdb.config e LEFT JOIN config m
    ON m.id == e.id WHERE m.id IS NULL;

