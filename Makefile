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

CC = cc

CFLAGS = -O2 -Wall -Werror -DUNIX=1 -DLINUX=1 -DNO_HEAP_TRACKING=1 -DHIGH_PERFORMANCE=1 $(CARCH) $(XFLAGS) $(INCLUDES)

LIBDIR = $(SA_ENGINE_HOME)/bin

INCLUDES = -I$(SA_ENGINE_HOME)/C -Iinclude
PAHO_LIB=:libpaho-mqtt3as.so

							  
all: $(SOURCE_FILES)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(EXTENSION).so $(SOURCE_FILES) -L$(LIBDIR) -lsa.kernel -l$(PAHO_LIB) 
	cp $(EXTENSION).so $(SA_ENGINE_HOME)/bin/

clean:
	rm -f *.so

test: all
	-pkill -9 mosquitto ; pkill -9 sa.engine
	cp $(EXTENSION).so $(SA_ENGINE_HOME)/bin/
	mosquitto -d > /dev/null
	sa.engine -O test/test.osql
	pkill -9 mosquitto
