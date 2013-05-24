DELETE FROM config;
DELETE FROM config_section;

INSERT INTO config_section (name) VALUES ( 'current' );

INSERT INTO config_section (name) VALUES ( 'tcpip' );
-- section tcpip --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dhcp', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'ip', '192.168.2.21' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'mask', '255.255.255.0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'gate', '192.168.2.1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dns1', '192.168.2.1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dns2', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'mac', '00:1C:D3:00:00:00' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dnsmasq', 'no' );


INSERT INTO config_section (name) VALUES ( 'device' );
-- section device --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'ids', 'ICDTCP3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'idd', '0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'address', 'www.insofter.pl' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'user', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'pass', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'transfer-enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'aggr-period-mins', '15' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'test-retention-period-mins', '60' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'server-sync-period-mins', '30' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'event-retention-period-count', '100000' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'device'), 'flow-entry-retention-period-days', '3660' );


INSERT INTO config_section (name) VALUES ( 'itd0' );
INSERT INTO config_section (name) VALUES ( 'itd1' );
INSERT INTO config_section (name) VALUES ( 'itd2' );
INSERT INTO config_section (name) VALUES ( 'itd3' );
-- section itd0 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'active-low', '1' );
-- section itd1 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'active-low', '1' );
-- section itd2 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'active-low', '1' );
-- section itd3 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'active-low', '1' );


INSERT INTO config_section (name) VALUES ( 'counter0' );
INSERT INTO config_section (name) VALUES ( 'counter1' );
INSERT INTO config_section (name) VALUES ( 'counter2' );
INSERT INTO config_section (name) VALUES ( 'counter3' );
-- section counter0 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'name', 'Wej 0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'counter_id', '1' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'master', 'itd0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'slave', 'null' ); -- itdX OR null
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'slave-mode', 'single' ); -- single OR direction 
                                                                                                                                -- OR thickness OR nothick
-- section counter1 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'name', 'Wej 1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'counter_id', '2' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'master', 'itd1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'slave', 'null' ); -- itdX OR null
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'slave-mode', 'single' ); -- single OR direction 
                                                                                                                                -- OR thickness OR nothick
-- section counter2 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'name', 'Wej 2' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'counter_id', '3' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'master', 'itd2' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'slave', 'null' ); -- itdX OR null
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'slave-mode', 'single' ); -- single OR direction 
                                                                                                                                -- OR thickness OR nothick
-- section counter3 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'name', 'Wej 3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'counter_id', '4' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'master', 'itd3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'slave', 'null' ); -- itdX OR null
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'slave-mode', 'single' ); -- single OR direction 
                                                                                                                                -- OR thickness OR nothick
INSERT INTO config_section (name) VALUES ( 'led0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'led0'), 'master-counter_id', '1' ); -- counterX.counter_id
INSERT INTO config_section (name) VALUES ( 'led1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'led1'), 'master-counter_id', '2' ); -- counterX.counter_id
INSERT INTO config_section (name) VALUES ( 'led2' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'led2'), 'master-counter_id', '3' ); -- counterX.counter_id
INSERT INTO config_section (name) VALUES ( 'led3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'led3'), 'master-counter_id', '4' ); -- counterX.counter_id


INSERT INTO config_section (name) VALUES ( 'login' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'login'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'login'), 'pass', 'admin' );


INSERT INTO config_section (name) VALUES ( 'wifi' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifi'), 'enabled', 'no' );

INSERT INTO config_section (name) VALUES ( 'wifinet-"insofter"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"insofter"'), 'psk', '"insofter"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"insofter"'), 'key_mgmt', 'WPA-PSK' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"insofter"'), 'auth_alg', 'OPEN' );

INSERT INTO config_section (name) VALUES ( 'wifinet-"IFirma"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"IFirma"'), 'psk', '"Insofter8971002738"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"IFirma"'), 'key_mgmt', 'WPA-PSK' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"IFirma"'), 'auth_alg', 'OPEN' );


INSERT INTO config_section (name) VALUES ( 'gsm' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'connect-with-transfer', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'pin-enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'pin', '2508' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'conf-dir', 'orange+huawei-e1752' );

