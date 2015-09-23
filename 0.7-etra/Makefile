# SUSI Library
# (C) Advantech 2010
#

SUSI_LIB = libsusi.so.0.0.7
SONAME = libsusi.so
STATIC = libsusi.a

CC = gcc
LD = ld
LN = ln
AR = ar
LDFLAGS = -m elf_i386
ARFLAGS = rc
CFLAGS = -fno-stack-protector -O2 -fPIC -m32
LIBS =
STRIP = strip --strip-unneeded

OBJS = susi.o smbus.o gpio.o watchdog.o hwm.o iomem.o

all: $(SUSI_LIB) $(STATIC)

$(SUSI_LIB): $(OBJS) susi.h i2c-dev.h
	$(LD) $(LDFLAGS) -shared $(OBJS) -o $@ $(LIBS)
	$(STRIP) $@
	$(LN) -s $(SUSI_LIB) $(SONAME) 

$(STATIC): $(OBJS) susi.h i2c-dev.h
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(STRIP) $@

.PHONY:
clean:
	rm -f *.o $(SUSI_LIB) $(SONAME) $(STATIC)
