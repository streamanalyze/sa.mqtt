FROM ubuntu:18.04

# apt should not expect interactive shell when setting up the docker image
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -qy install build-essential git mosquitto cmake automake autoconf ninja-build python-jinja2 libssl-dev

ADD sa_engine_core_linux_x64.tar.gz /
ENV SA_ENGINE_HOME /sa.engine
ENV PATH $PATH:/sa.engine/bin
