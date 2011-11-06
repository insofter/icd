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

INC= curlcc.h sqlite3cc.h transfer-agent.h

all: icdtcp3-transfer-data

icdtcp3-transfer-data: icdtcp3-transfer-data.cc transfer-agent.cc $(INC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lsqlite3 -lcurl -lroxml -o $@ icdtcp3-transfer-data.cc transfer-agent.cc


#all: tsdfs-erase tsdfs-split tsdfs-join

#tsdfs-erase: tsdfs-erase.c $(INC)
#	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ tsdfs-erase.c

#tsdfs-split: tsdfs-split.c $(INC)
#	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ tsdfs-split.c

#tsdfs-join: tsdfs-join.c $(INC)
#	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ tsdfs-join.c

clean:
	$(RM) icdtcp3-transfer-data

