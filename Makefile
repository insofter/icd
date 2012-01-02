#***********************************************************************
#
# Copyright (c) Tomasz Rozensztrauch
# 
#***********************************************************************
.PHONY: cleanall

#CC=arm-linux-uclibcgnueabi-gcc
#CXX=arm-linux-uclibcgnueabi-g++
CPPFLAGS=
CXXFLAGS=-Wall -g -D_GNU_SOURCE

INC= db-config.h curlcc.h sqlite3cc.h transfer-agent.h

all: icdtcp3-config icdtcp3-sql icdtcp3-itd-daemon icdtcp3-aggr-data icdtcp3-transfer-data

icdtcp3-config: icdtcp3-config.cc db-config.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icdtcp3-config.cc db-config.cc -lsqlite3 

icdtcp3-sql: icdtcp3-sql.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icdtcp3-sql.cc -lsqlite3 

icdtcp3-itd-daemon: icdtcp3-itd-daemon.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icdtcp3-itd-daemon.cc -lsqlite3 

icdtcp3-aggr-data: icdtcp3-aggr-data.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icdtcp3-aggr-data.cc -lsqlite3 

icdtcp3-transfer-data: icdtcp3-transfer-data.cc transfer-agent.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ icdtcp3-transfer-data.cc transfer-agent.cc -lsqlite3 -lcurl -lroxml

clean:
	$(RM) icdtcp3-config
	$(RM) icdtcp3-sql
	$(RM) icdtcp3-itd-daemon
	$(RM) icdtcp3-aggr-data
	$(RM) icdtcp3-transfer-data

