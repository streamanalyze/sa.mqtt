#####################################################################
# This is a generic Makefile for making sa.engine plugins.
# Assign SOURCES to your C source files (here `foreign.c`).
# Assign EXTENDER to the name of your extender (here `plugin`).
# The extender will be generated in the library $SA_ENGINE_HOME/bin
# The environment variable OS must be set to either `Linux` or `OSX`. 
# The environmant variable WORDLEN must be set to 64 or 32
#####################################################################

EXTENSION = libsa_mqtt
srcdir = src
SOURCE_FILES = $(wildcard $(srcdir)/*.c)

#####################################################################
XFLAGS = -fPIC
LFLAGS = -shared

CC ?= cc

CFLAGS = -O2 -Wall -Werror -DUNIX=1 -DLINUX=1 -DNO_HEAP_TRACKING=1 -DHIGH_PERFORMANCE=1 $(CARCH) $(XFLAGS) $(INCLUDES)

LIBDIR = $(SA_ENGINE_HOME)/bin

INCLUDES = -I$(SA_ENGINE_HOME)/C -Iinclude -Ipaho.mqtt.c/src
PAHO_LIB=:libpaho-mqtt3as.so


all: $(SOURCE_FILES) $(SA_ENGINE_HOME)/bin/libpaho-mqtt3as.so
	$(CC) $(CFLAGS) $(LFLAGS) -o $(EXTENSION).so $(SOURCE_FILES) -L$(LIBDIR)  -l$(PAHO_LIB) 
	cp $(EXTENSION).so $(SA_ENGINE_HOME)/bin/

	
# paho's own Makefile honours CC, and links -lssl -lcrypto out of whatever the
# compiler's library path is.  A cross build therefore needs nothing but the
# right CC: the arm32 base CC carries --sysroot, and that sysroot holds only
# STATIC OpenSSL archives (docker-specifications/arm32), so the library comes
# out with no DT_NEEDED on libssl/libcrypto and runs on every board regardless
# of which OpenSSL it ships.  CARCH goes in through CFLAGS, which paho appends.
$(SA_ENGINE_HOME)/bin/libpaho-mqtt3as.so:
	-@rm $(SA_ENGINE_HOME)/bin/libpaho-mqtt3as.so*
	test -f paho.mqtt.c/Makefile || ( git submodule init && git submodule update )
	cd paho.mqtt.c && $(MAKE) clean && $(MAKE) CFLAGS="$(CARCH)"
	cp -f paho.mqtt.c/build/output/libpaho-mqtt3as.so.1.3 $(SA_ENGINE_HOME)/bin/libpaho-mqtt3as.so.1.3
	cd $(SA_ENGINE_HOME)/bin/ && ln -sf libpaho-mqtt3as.so.1.3 libpaho-mqtt3as.so.1
	cd $(SA_ENGINE_HOME)/bin/ && ln -sf libpaho-mqtt3as.so.1 libpaho-mqtt3as.so
 
clean:
	-rm -f *.so
	-cd $(SA_ENGINE_HOME)/bin/ && rm -f libpaho-mqtt3as.so* libsa_mqtt.so
	-cd $(SA_ENGINE_HOME)/sa.engine/bin/ && rm -f libpaho-mqtt3as.so* libsa_mqtt.so

test: all
	-pkill -9 mosquitto ; pkill -9 sa.engine
	cp $(EXTENSION).so $(SA_ENGINE_HOME)/bin/
	mosquitto -d > /dev/null
	sa.engine -O test/test.osql
	pkill -9 mosquitto

bench:
	-pkill -9 mosquitto ; pkill -9 sa.engine
	cp $(EXTENSION).so $(SA_ENGINE_HOME)/bin/
	mosquitto -d > /dev/null
	sa.engine -O test/benchmark.osql
	pkill -9 mosquitto

build_docker: Dockerfile
	docker build . -t sa.mqtt.builder:1

win_run_docker:
	docker run -ti --mount type=bind,source="%cd%",target=/sa.mqtt  sa.mqtt.builder:1 /bin/bash

linux_run_docker:
	docker run -ti --mount type=bind,source="$(pwd)",target=/sa.mqtt  sa.mqtt.builder:1 /bin/bash

