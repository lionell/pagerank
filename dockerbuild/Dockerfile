FROM base/archlinux:latest
MAINTAINER Ruslan Sakevych

RUN pacman -Sy
RUN pacman --noconfirm -S gzip
RUN pacman --noconfirm -S openmp openmpi
RUN pacman --noconfirm -S protobuf gflags google-glog

ADD https://github.com/lionell/pagerank/releases/download/v1.0/pagerank-1.0-linux-x86_64.tar.gz /var/pagerank/pagerank.tar.gz
RUN tar xzf /var/pagerank/pagerank.tar.gz -C /usr/bin
