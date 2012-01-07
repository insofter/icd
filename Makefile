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

INC= db-config.h curlcc.h sqlite3cc.h transfer-agent.h

all: icd-config icd-sql icd-itd-daemon icd-aggr-data icd-transfer-data

icd-config: icd-config.cc db-config.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-config.cc db-config.cc -lsqlite3 

icd-sql: icd-sql.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-sql.cc -lsqlite3 

icd-itd-daemon: icd-itd-daemon.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-itd-daemon.cc -lsqlite3 

icd-aggr-data: icd-aggr-data.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-aggr-data.cc -lsqlite3 

icd-transfer-data: icd-transfer-data.cc transfer-agent.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icd-transfer-data.cc transfer-agent.cc -lsqlite3 -lcurl -lroxml

clean:
	$(RM) icd-config
	$(RM) icd-sql
	$(RM) icd-itd-daemon
	$(RM) icd-aggr-data
	$(RM) icd-transfer-data

