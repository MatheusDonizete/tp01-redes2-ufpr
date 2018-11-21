FROM ubuntu:latest

RUN apt-get update
RUN apt-get install  build-essential  git  debhelper autotools-dev dh-autoreconf  iptables  protobuf-compiler libprotobuf-dev pkg-config  libssl-dev  dnsmasq-base ssl-cert libxcb-present-dev  libcairo2-dev  libpango1.0-dev  iproute2 apache2-dev  apache2-bin  iptables  dnsmasq-base  gnuplot  iproute2 apache2-api-20120211  libwww-perl -y

RUN apt-get install openvpn -y

RUN curl -sL https://deb.nodesource.com/setup_11.x | sudo -E bash -
RUN apt-get install -y nodejs

RUN useradd redes

COPY . /home/contest
WORKDIR /home/contest