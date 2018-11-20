#!/bin/sh

exec ./autogen.sh && ./configure && make
exec make install

exec ./codigo-exemplo/autogen.sh && ./codigo-exemplo/configure && make
exec sudo sysctl -w net.ipv4.ip_forward=1