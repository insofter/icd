
DELETE FROM config WHERE section_id = (SELECT id FROM config_section WHERE name == 'current');



