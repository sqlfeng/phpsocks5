C++ = g++

ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CCFLAGS = -fPIC -Wall -D$(os) -finline-functions -O3

CCFLAGS += -DNO_BUSY_WAITING
#CCFLAGS += -DCUSTOM_CC

ifeq ($(arch), IA32)
   CCFLAGS += -DIA32
endif

ifeq ($(arch), POWERPC)
   CCFLAGS += -mcpu=powerpc
endif

ifeq ($(arch), SPARC)
   CCFLAGS += -mcpu=sparc
endif

ifeq ($(arch), IA64)
   CCFLAGS += -DIA64
endif

ifeq ($(arch), AMD64)
   CCFLAGS += -DAMD64
endif
SRC = $(shell ls *.c)
OBJS = $(SRC:%.c=%.o)
DIR = $(shell pwd)

all: libminIni.so libminIni.a 

%.o: %.c %.h 
	$(C++) $(CCFLAGS) $< -c

libminIni.so: $(OBJS)
ifneq ($(os), BSD)
	$(C++) -shared -o $@ $^
else
	$(C++) -dynamiclib -o libminIni.dylib -lstdc++ -lpthread -lm $^
endif

libminIni.a: $(OBJS)
	ar -rcs $@ $^

clean:
	rm -f *.o *.so *.dylib *.a 

install:
	export LD_LIBRARY_PATH=$(DIR):$$LD_LIBRARY_PATH
