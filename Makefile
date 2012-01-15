#***********************************************************************
#
# Copyright (c) Tomasz Rozensztrauch
# 
#***********************************************************************
.PHONY: cleanall

CC=arm-linux-gcc
CXX=arm-linux-g++
CPPFLAGS=
CXXFLAGS=-Wall -g -D_GNU_SOURCE

INC=db-config.h curlcc.h sqlite3cc.h transfer-agent.h

all: icd-config icd-sql icd-itd-daemon icd-fc-daemon icd-transfer-data

icd-config: icd-config.cc db-config.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-config.cc db-config.cc -lsqlite3 

icd-sql: icd-sql.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-sql.cc -lsqlite3 

icd-itd-daemon: icd-itd-daemon.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-itd-daemon.cc db-config.cc -lsqlite3 

icd-fc-daemon: icd-fc-daemon.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-fc-daemon.cc db-config.cc -lsqlite3 

icd-transfer-data: icd-transfer-data.cc transfer-agent.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-transfer-data.cc transfer-agent.cc -lsqlite3 -lcurl -lroxml

mktarget:
	mkdir -p output/target/usr/bin
	cp -au icd-config output/target/usr/bin/
	cp -au icd-sql output/target/usr/bin/
	cp -au icd-itd-daemon output/target/usr/bin/
	cp -au icd-fc-daemon output/target/usr/bin/
	cp -au icd-transfer-data output/target/usr/bin/
	cp -au scripts/* output/target/usr/bin/
	mkdir -p output/target/usr/share/icd
	cp -au sql/* output/target/usr/share/icd/

scp: mktarget
	ssh admin@192.168.2.21 \
	  'rm -Rf ./target;' \
          'sudo rm -f /usr/bin/icd-*;' \
	  'sudo rm -Rf /usr/share/icd'
	scp -r output/target admin@192.168.2.21:~/
	ssh admin@192.168.2.21 \
	  'sudo install -d /usr/share/icd;' \
	  'sudo install -m 644 ./target/usr/share/icd/* /usr/share/icd/;' \
	  'sudo install ./target/usr/bin/* /usr/bin;' \
	  'rm -Rf ./target'

clean:
	$(RM) icd-config
	$(RM) icd-sql
	$(RM) icd-itd-daemon
	$(RM) icd-fc-daemon
	$(RM) icd-transfer-data

