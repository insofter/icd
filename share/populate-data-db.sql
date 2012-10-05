
-- section tcpip --
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'dhcp', 'yes' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'ip', '192.168.2.21' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'mask', '255.255.255.0' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'gate', '192.168.2.1' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'dns1', '192.168.2.1' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'dns2', '' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'net-name', 'ICDTCP3' );
INSERT INTO config (section, key, value) VALUES ( 'tcpip', 'mac', '00:1C:D3:00:00:00' );

-- section device --
INSERT INTO config (section, key, value) VALUES ( 'device', 'ids', 'ICDTCP3' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'idd', '0' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'address', 'www.insofter.pl' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'user', '' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'pass', '' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'transfer-enabled', 'yes' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'aggr-period-mins', '15' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'test-retention-period-mins', '60' );

INSERT INTO config (section, key, value) VALUES ( 'device', 'server-sync-period-mins', '30' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'event-retention-period-count', '100000' );
INSERT INTO config (section, key, value) VALUES ( 'device', 'flow-entry-retention-period-mins', '120' );

INSERT INTO config (section, key, value) VALUES ( 'wifi', 'enabled', 'no' );
INSERT INTO config (section, key, value) VALUES ( 'gsm', 'enabled', 'no' );


INSERT INTO counters (id, name, enabled, device, engage_delay_ms, release_delay_ms,
  active_low, direction_enabled, thickness_enabled, disabling_enabled) 
  VALUES(1,'wej0','TRUE','/dev/itd0',200,200,'TRUE','FALSE','FALSE','FALSE');

INSERT INTO counters (id, name, enabled, device, engage_delay_ms, release_delay_ms,
  active_low, direction_enabled, thickness_enabled, disabling_enabled) 
  VALUES(2,'wej1','TRUE','/dev/itd1',200,200,'TRUE','FALSE','FALSE','FALSE');

INSERT INTO counters (id, name, enabled, device, engage_delay_ms, release_delay_ms,
  active_low, direction_enabled, thickness_enabled, disabling_enabled)
  VALUES(3,'wej2','TRUE','/dev/itd2',200,200,'TRUE','FALSE','FALSE','FALSE');

INSERT INTO counters (id, name, enabled, device, engage_delay_ms, release_delay_ms,
  active_low, direction_enabled, thickness_enabled, disabling_enabled)
  VALUES(4,'wej3','TRUE','/dev/itd3',200,200,'TRUE','FALSE','FALSE','FALSE');


