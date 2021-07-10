FROM ubuntu:20.04

RUN apt update && apt upgrade -y
RUN apt install -y \
    software-properties-common
RUN add-apt-repository ppa:tkchia/build-ia16
RUN apt update && apt install -y upx make libi86-ia16-elf  gcc-ia16-elf g++-ia16-elf

