# sa.mqtt
MQTT extender for sa.engine
[![v4.3.0 Test](https://github.com/streamanalyze/sa.mqtt/actions/workflows/v4-3-0_test.yml/badge.svg)](https://github.com/streamanalyze/sa.mqtt/actions/workflows/v4-3-0_test.yml)

This project implements publish and subscribe using
[eclipse/paho.mqtt.c](https://github.com/eclipse/paho.mqtt.c).  Install the
libpaho-mqttcs.so library according to the install instructions from their
repository.  If you wish to do this another way, modify the Makefile and make
sure that one of:

* libpaho-mqttas.so 
* libpaho-mqtta.so 
* libpaho-mqttas.a 
* libpaho-mqtta.a 

is linked/included in the build.


To build this extender you need to:

1. Sign up at [studio.streamanalyze.com](https://studio.streamanalyze.com/)
and download sa.engine core for the target platform.

2. Unpack the sa.engine package to a desired location,
e.g. `~/development/sa.engine`.

3. set `SA_ENGINE_HOME` to point to the sa.engine folder, e.g: `export
SA_ENGINE_HOME=~/development/sa.engine`.

4. run `make test`

Cross compiling should work as long as you have libpaho-mqttaX.x and sa.engine
installed on the target platform. Modify the Makefile to add crosscompiling
options if needed.

To build the extender using make run:

```
make all
```

To test:

```
make test
```

Example output from test:

```bash
~/development/sa.mqtt$ make test
cc -O2 -Werror -DUNIX=1 -DLINUX=1 -DNO_HEAP_TRACKING=1 -DHIGH_PERFORMANCE=1  -fPIC -I/home/johan/sa.engine/C -Iinclude -shared -o mqtt.so src/mqtt.c src/cbfns.c src/sa_circular_buff.c -L/home/johan/sa.engine/bin -lsa.kernel -l:libpaho-mqtt3as.so
cp mqtt.so /home/johan/sa.engine/bin/
pkill -9 mosquitto ; pkill -9 sa.engine
cp mqtt.so /home/johan/sa.engine/bin/
mosquitto -d > /dev/null
sa.engine -O test/test.osql
Database image: /home/johan/sa.engine/bin/sa.engine.dmp
Release 4, v2, 64 bits
Reading OSQL statements from "/home/johan/development/sa.mqtt/test/test.osql"
Reading OSQL statements from "/home/johan/development/sa.mqtt/mqtt.osql"
Loading "mqtt.lsp"
INITIALIZING MQTT
"mqtt.lsp"
"mqtt.osql"
"NIL
"
"mqtt"
"mqtt1"
"mqtt2"
Loading "test/setup_test.lsp"
"test/setup_test.lsp"
[Testing MQTT subscribe types ... OK]
[Testing MQTT Subscribes (and publish) ... OK]
[Testing MQTT Modify and relay ... OK]
[Testing MQTT Fast ...

        QoS0 2183.71602956752 (max ~1000)
        QoS1 4880.04841008023 (max ~1000)
        QoS2 1389.32884302251 (max ~1000)
 OK]
pkill -9 mosquitto
```


## Building using CMAKE
To build this project using CMAKE simply run:
```
mkdir build
cd build
cmake .. -DSA_ENGINE_HOME=</PATH/TO/YOUR/SA.ENGINE/FOLDER>
```


## Build using docker
Download sa_engine_core_linux_x64.tar.gz and put in the sa.mqtt directory.
Build the container:
```
docker build . -t sa.mqtt.builder:1
```
Run it and build:
```
Windows:
docker run -ti --mount type=bind,source="%cd%",target=/sa.mqtt  sa.mqtt.builder:1 /bin/bash
Linux:
docker run -ti --mount type=bind,source="$(pwd)",target=/sa.mqtt  sa.mqtt.builder:1 /bin/bash

root@f0f49b504f21:/# cd sa.mqtt/
root@f0f49b504f21:/sa.mqtt# make test
```


## Testing
As of now this repo is tested in our internal CI/CD pipeline every night against main. 
We are working on a good way to supply a public CI/CD pipeline to view results from.
Feel free to use the dockerimage for your own testing.
