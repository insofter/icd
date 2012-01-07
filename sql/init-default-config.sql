DELETE FROM config;
DELETE FROM config_section;

INSERT INTO config_section (id, name) VALUES ( 1, 'current' );
INSERT INTO config_section (id, name) VALUES ( 2, 'tcpip' );
INSERT INTO config_section (id, name) VALUES ( 3, 'device' );
INSERT INTO config_section (id, name) VALUES ( 4, 'itd0' );
INSERT INTO config_section (id, name) VALUES ( 5, 'itd1' );
INSERT INTO config_section (id, name) VALUES ( 6, 'itd2' );
INSERT INTO config_section (id, name) VALUES ( 7, 'itd3' );

-- section tcpip --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'dhcp', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'ip', '192.168.2.45' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'mask', '255.255.255.0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'gate', '192.168.2.1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'dns1', '192.168.2.1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'dns2', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'current'), 'net-name', 'ICDTCP3' );

-- sectin device --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'ids', 'ICDTCP3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'idd', '0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'address', 'www.insofter.pl' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'user', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'pass', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'transfer-enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'aggr-period-mins', '15' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'event-retention-period-mins', '60' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'flow-entry-retention-period-mins', '120' );

-- section itd0 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'name', 'Entrance A' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'blocked-state', '1' );

-- section itd1 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'name', 'Entrance B' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'blocked-state', '1' );

-- section itd2 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'name', 'Entrance C' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'blocked-state', '1' );

-- section itd3 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'name', 'Entrance D' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'blocked-state', '1' );
