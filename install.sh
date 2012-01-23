#!/bin/sh

install -d /usr/share/icd
install -m 644 ./usr/share/icd/*sql /usr/share/icd/
install -m 755 ./usr/share/icd/*script /usr/share/icd/
install ./usr/bin/* /usr/bin
install -m 644 ./etc/lighttpd.conf /etc/
install ./etc/init.d/S70icd /etc/init.d/
install ./etc/init.d/S80lighttpd /etc/init.d/
install -d /var/www
install -m 644 ./var/www/* /var/www

