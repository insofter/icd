INSERT OR IGNORE INTO config (section_id, key, value) VALUES (
  (SELECT id FROM config_section WHERE name == 'current'),
  'aggr-period-mins',
  '15' 
);
