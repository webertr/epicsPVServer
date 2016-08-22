# #objects = $(addprefix $(OBJDIR)/, bindSocketTest.o getHTMLFile.o listenTest.o simpleWebServer.o printIPAddress.o socketTest.o)

INCLUDEDIR = $(shell pwd)/include
# SRCDIR = $(shell pwd)/src
# OBJDIR = $(shell pwd)/obj
# LIBS  = -lpthread
# make --just-print to debug
# make -B will force a make

vpath %.c  src
vpath %.h  include

PATH1=/usr/src/epics/base-3.15.3/lib/linux-x86_64
CC = gcc
ODIR = obj
SDIR = src
INC = -I$(INCLUDEDIR) -I/usr/src/epics/base-3.15.3/include -I/usr/src/epics/base-3.15.3/include/os/Linux -I/usr/src/epics/base-3.15.3/include/compiler/gcc -I/usr/src/epics/base-3.15.3/src/ca/client/tools
LIBS = -lpthread -ldbCore -lca -lCom
LIBSDIR = -L$(PATH1)

_OBJS1 = simpleWebServer.o robustIO.o tool_lib.o
OBJS1 = $(patsubst %,$(ODIR)/%,$(_OBJS1))

_OBJS2 = pvDisplay.o tool_lib.o cagetfuze.o
OBJS2 = $(patsubst %,$(ODIR)/%,$(_OBJS2))

_OBJS3 = adder.o
OBJS3 = $(patsubst %,$(ODIR)/%,$(_OBJS3))

all: pv_server cgi-bin/adder cgi-bin/pvDisplay

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(INC)

.PHONY: clean

pv_server: $(OBJS1)
	$(CC) -o pv_server $(OBJS1) $(INC) $(LIBSDIR) $(LIBS)

cgi-bin/pvDisplay: $(OBJS2)
	$(CC) -o cgi-bin/pvDisplay $(OBJS2) $(INC) $(LIBSDIR) $(LIBS)

cgi-bin/adder: $(OBJS3)
	$(CC) -o cgi-bin/adder $(OBJS3) $(INC) $(LIBSDIR) $(LIBS)

clean:
	rm -f $(ODIR)/*.o
	rm -f $(SDIR)/*~
