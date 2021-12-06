FROM ubuntu:18.04

# apt should not expect interactive shell when setting up the docker image
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -qy upgrade
RUN apt-get -qy install apt-utils

RUN apt-get -qy install tzdata
RUN dpkg-reconfigure --frontend noninteractive tzdata

# unminimize is required to run full Ubuntu as a container
RUN yes | unminimize

RUN apt-get -qy install openssh-server sudo
RUN apt-get -qy install build-essential gcc-multilib g++-multilib g++
RUN apt-get -qy install zip unzip dos2unix curl wget emacs git screen
RUN apt-get -qy install bison flex
RUN apt-get -qy install openjdk-8-jdk
RUN apt-get -qy install xauth x11-apps
RUN apt-get -qy install libc6-dbg gdb
RUN apt-get -qy install net-tools
RUN apt-get -qy install python2.7 python2.7-dev
RUN apt-get -qy install awscli
RUN apt-get -qy install mosquitto
RUN apt-get -qy install cmake automake autoconf ninja-build python-jinja2
RUN apt-get -qy install libusb-1.0-0-dev
RUN apt-get -qy install libsecret-1-dev
RUN apt-get -qy install socat
RUN apt-get -qy install libssl-dev

# Recent versions of Linux tend to use systemd (which uses systemctl command), while the older versions of Linux tend to use System V init (which uses the service command) If your system uses systemd, the command is:- 'sudo systemctl start mongod' and if it uses System V Init then the command is:- 'sudo service mongod start'

RUN service ssh start

EXPOSE 22
CMD ["/usr/sbin/sshd","-D"]