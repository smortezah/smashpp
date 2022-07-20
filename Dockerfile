FROM ubuntu:22.04
LABEL maintainer "Morteza Hosseini"

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y cmake g++

COPY . /smashpp
WORKDIR /smashpp
RUN bash install.sh
# ENTRYPOINT ["./smashpp"]
