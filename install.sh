#!/bin/sh

install -d /usr/share/icd
install -m 644 ./usr/share/icd/*sql /usr/share/icd/
install -m 755 ./usr/share/icd/*script /usr/share/icd/
install ./usr/bin/* /usr/bin

