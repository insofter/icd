PRAGMA foreign_keys = ON;

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

INSERT INTO config_section (id, name) VALUES ( 1, "current" );
INSERT INTO config_section (id, name) VALUES ( 2, "tcpip" );
INSERT INTO config_section (id, name) VALUES ( 3, "device" );
INSERT INTO config_section (id, name) VALUES ( 4, "itd0" );
INSERT INTO config_section (id, name) VALUES ( 5, "itd1" );
INSERT INTO config_section (id, name) VALUES ( 6, "itd2" );
INSERT INTO config_section (id, name) VALUES ( 7, "itd3" );

-- section tcpip --
INSERT INTO config (section_id, key, value) VALUES ( 2, "dhcp", "no" );
INSERT INTO config (section_id, key, value) VALUES ( 2, "ip", "192.168.2.45" );
INSERT INTO config (section_id, key, value) VALUES ( 2, "mask", "255.255.255.0" );
INSERT INTO config (section_id, key, value) VALUES ( 2, "gate", "192.168.2.1" );
INSERT INTO config (section_id, key, value) VALUES ( 2, "dns1", "192.168.2.1" );
INSERT INTO config (section_id, key, value) VALUES ( 2, "dns2", "" );
INSERT INTO config (section_id, key, value) VALUES ( 2, "net-name", "ICDTCP3" );

-- sectin device --
INSERT INTO config (section_id, key, value) VALUES ( 3, "ids", "ICDTCP3" );
INSERT INTO config (section_id, key, value) VALUES ( 3, "idd", "0" );
INSERT INTO config (section_id, key, value) VALUES ( 3, "address", "www.insofter.pl" );
INSERT INTO config (section_id, key, value) VALUES ( 3, "user", "" );
INSERT INTO config (section_id, key, value) VALUES ( 3, "pass", "" );
INSERT INTO config (section_id, key, value) VALUES ( 3, "aggr-period-mins", "15" );
INSERT INTO config (section_id, key, value) VALUES ( 3, "transfer-enabled", "yes" );

-- section itd0 --
INSERT INTO config (section_id, key, value) VALUES ( 4, "name", "Entrance A" );
INSERT INTO config (section_id, key, value) VALUES ( 4, "enabled", "yes" );
INSERT INTO config (section_id, key, value) VALUES ( 4, "engage-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 4, "release-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 4, "blocked-state", "1" );

-- section itd1 --
INSERT INTO config (section_id, key, value) VALUES ( 5, "name", "Entrance B" );
INSERT INTO config (section_id, key, value) VALUES ( 5, "enabled", "no" );
INSERT INTO config (section_id, key, value) VALUES ( 5, "engage-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 5, "release-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 5, "blocked-state", "1" );

-- section itd2 --
INSERT INTO config (section_id, key, value) VALUES ( 6, "name", "Entrance C" );
INSERT INTO config (section_id, key, value) VALUES ( 6, "enabled", "no" );
INSERT INTO config (section_id, key, value) VALUES ( 6, "engage-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 6, "release-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 6, "blocked-state", "1" );

-- section itd3 --
INSERT INTO config (section_id, key, value) VALUES ( 7, "name", "Entrance D" );
INSERT INTO config (section_id, key, value) VALUES ( 7, "enabled", "no" );
INSERT INTO config (section_id, key, value) VALUES ( 7, "engage-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 7, "release-delay-ms", "200" );
INSERT INTO config (section_id, key, value) VALUES ( 7, "blocked-state", "1" );

