C++ = g++

ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CCFLAGS = -Wall -g -D$(os) -I../minIni -O2 -finline-functions 

ifeq ($(arch), IA32)
   CCFLAGS += -DIA32 #-mcpu=pentiumpro -march=pentiumpro -mmmx -msse
endif

ifeq ($(arch), POWERPC)
   CCFLAGS += -mcpu=powerpc
endif

ifeq ($(arch), IA64)
   CCFLAGS += -DIA64
endif

LDFLAGS = ../minIni/libminIni.a  -lstdc++ -lpthread -lm -ldl `curlpp-config --libs`

ifeq ($(os), UNIX)
   LDFLAGS += -lsocket
endif

SRC= $(shell ls *.cpp)
OBJS= $(SRC:%.cpp=%.o)
DIR = $(shell pwd)

all: client

client:$(OBJS)
	$(C++) -o $@ $^  $(LDFLAGS)

%.o: %.cpp
	$(C++) $(CCFLAGS) $< -c

clean:
	rm -f *.o
	rm -f client

install:
	export PATH=$(DIR):$$PATH
