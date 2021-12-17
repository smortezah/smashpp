FROM ubuntu:20.04
LABEL maintainer "Morteza Hosseini"

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y cmake g++ wget unzip

RUN wget https://github.com/smortezah/smashpp/archive/refs/heads/master.zip
RUN unzip master.zip && rm -f master.zip
RUN mv smashpp-master smashpp

WORKDIR /smashpp
RUN bash install.sh