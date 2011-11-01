#***********************************************************************
#
# Copyright (c) Tomasz Rozensztrauch
# 
#***********************************************************************
.PHONY: cleanall

#CC=arm-linux-uclibcgnueabi-gcc
#CC=g++
CPPFLAGS=
CXXFLAGS=-Wall -g -D_GNU_SOURCE

all: icdtcp3-transfer-data

icdtcp3-transfer-data: icdtcp3-transfer-data.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lsqlite3 -o $@ icdtcp3-transfer-data.cc

#INC= tsdfs.h ../tsdfs-ioctl.h

#all: tsdfs-erase tsdfs-split tsdfs-join

#tsdfs-erase: tsdfs-erase.c $(INC)
#	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ tsdfs-erase.c

#tsdfs-split: tsdfs-split.c $(INC)
#	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ tsdfs-split.c

#tsdfs-join: tsdfs-join.c $(INC)
#	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ tsdfs-join.c

clean:
	$(RM) icdtcp3-itd-daemon

