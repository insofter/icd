DELETE FROM config;
DELETE FROM config_section;

INSERT INTO config_section (id, name) VALUES ( 1, 'current' );

INSERT INTO config_section (id, name) VALUES ( 2, 'tcpip' );
-- section tcpip --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dhcp', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'ip', '192.168.2.21' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'mask', '255.255.255.0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'gate', '192.168.2.1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dns1', '192.168.2.1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dns2', '' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'mac', '00:1C:D3:00:00:00' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'tcpip'), 'dnsmasq', 'no' );


INSERT INTO config_section (id, name) VALUES ( 3, 'device' );
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


INSERT INTO config_section (id, name) VALUES ( 4, 'itd0' );
INSERT INTO config_section (id, name) VALUES ( 5, 'itd1' );
INSERT INTO config_section (id, name) VALUES ( 6, 'itd2' );
INSERT INTO config_section (id, name) VALUES ( 7, 'itd3' );
-- section itd0 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'name', 'Wej 0' );-- == counter0.name
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'enabled', 'yes' );-- == counter0.enabled
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'engage-delay-ms', '200' );-- == ...
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd0'), 'active-low', '1' );
-- section itd1 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'name', 'Wej 1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd1'), 'active-low', '1' );
-- section itd2 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'name', 'Wej 2' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd2'), 'active-low', '1' );
-- section itd3 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'name', 'Wej 3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'engage-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'release-delay-ms', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'itd3'), 'active-low', '1' );


INSERT INTO config_section (id, name) VALUES ( 8, 'counter0' );
INSERT INTO config_section (id, name) VALUES ( 9, 'counter1' );
INSERT INTO config_section (id, name) VALUES ( 10, 'counter2' );
INSERT INTO config_section (id, name) VALUES ( 11, 'counter3' );
-- section counter0 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'name', 'Wej 0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'counter_id', '1' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'enabled', 'yes' );
--main configuration
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'dev', 'itd0' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'dev-engage', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'dev-release', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'dev-active-low', 'yes' );
-- toggle off on
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'enab', '-' );--wylacznik czasowy
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'enab-time', '5000' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'enab-active-low', 'yes' );
-- thickness and direction detection
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'thick', '-' ); -- ``-'' == disabled
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'thick-detect-direction', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter0'), 'thick-active-low', 'yes' );

-- section counter1 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'name', 'Wej 1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'counter_id', '2' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'enabled', 'yes' );
--main configuration
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'dev', 'itd1' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'dev-engage', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'dev-release', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'dev-active-low', 'yes' );
-- toggle off on
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'enab', '-' );--wylacznik czasowy
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'enab-time', '5000' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'enab-active-low', 'yes' );
-- thickness and direction detection
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'thick', '-' ); -- ``-'' == disabled
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'thick-detect-direction', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter1'), 'thick-active-low', 'yes' );

-- section counter2 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'name', 'Wej 2' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'counter_id', '3' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'enabled', 'yes' );
--main configuration
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'dev', 'itd2' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'dev-engage', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'dev-release', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'dev-active-low', 'yes' );
-- toggle off on
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'enab', '-' );--wylacznik czasowy
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'enab-time', '5000' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'enab-active-low', 'yes' );
-- thickness and direction detection
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'thick', '-' ); -- ``-'' == disabled
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'thick-detect-direction', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter2'), 'thick-active-low', 'yes' );

-- section counter3 --
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'name', 'Wej 3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'counter_id', '4' ); -- flow.counter_id
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'enabled', 'yes' );
--main configuration
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'dev', 'itd3' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'dev-engage', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'dev-release', '200' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'dev-active-low', 'yes' );
-- toggle off on
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'enab', '-' );--wylacznik czasowy
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'enab-time', '5000' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'enab-active-low', 'yes' );
-- thickness and direction detection
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'thick', '-' ); -- ``-'' == disabled
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'thick-detect-direction', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'counter3'), 'thick-active-low', 'yes' );


INSERT INTO config_section (id, name) VALUES ( 12, 'login' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'login'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'login'), 'pass', 'admin' );


INSERT INTO config_section (id, name) VALUES ( 13, 'wifi' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifi'), 'enabled', 'no' );

INSERT INTO config_section (id, name) VALUES ( 14, 'wifinet-"insofter"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"insofter"'), 'psk', '"insofter"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"insofter"'), 'key_mgmt', 'WPA-PSK' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"insofter"'), 'auth_alg', 'OPEN' );

INSERT INTO config_section (id, name) VALUES ( 15, 'wifinet-"IFirma"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"IFirma"'), 'psk', '"Insofter8971002738"' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"IFirma"'), 'key_mgmt', 'WPA-PSK' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'wifinet-"IFirma"'), 'auth_alg', 'OPEN' );


INSERT INTO config_section (id, name) VALUES ( 16, 'gsm' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'enabled', 'no' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'connect-with-transfer', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'pin-enabled', 'yes' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'pin', '2508' );
INSERT INTO config (section_id, key, value) VALUES ( (SELECT id FROM config_section WHERE name == 'gsm'), 'conf-dir', 'orange+huawei-e1752' );

