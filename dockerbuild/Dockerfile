FROM base/archlinux:latest
MAINTAINER Ruslan Sakevych

RUN pacman -Sy
RUN pacman --noconfirm -S gzip
RUN pacman --noconfirm -S openmp openmpi
RUN pacman --noconfirm -S protobuf gflags google-glog

ADD https://github.com/lionell/labs/releases/download/v0.1/pr-0.1.bin.tgz /var/pr/pr.tgz
RUN tar xzf /var/pr/pr.tgz -C /usr/bin
