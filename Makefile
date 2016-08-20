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
OUT = lib/alib.a
CC = gcc
ODIR = obj
SDIR = src
INC = -I$(INCLUDEDIR) -I/usr/src/epics/base-3.15.3/include -I/usr/src/epics/base-3.15.3/include/os/Linux -I/usr/src/epics/base-3.15.3/include/compiler/gcc -I/usr/src/epics/base-3.15.3/src/ca/client/tools
LIBS = -lpthread -ldbCore -lca -lCom
LIBSDIR = -L$(PATH1)

_OBJS = simpleWebServer.o robustIO.o tool_lib.o cagetfuze.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

all: pv_server adder camonitor

pv_server: $(OBJS)
	$(CC) -o pv_server $(OBJS) $(INC) $(LIBSDIR) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(INC)

.PHONY: clean

adder: $(SDIR)/adder.c
	$(CC) -o cgi-bin/adder $(SDIR)/adder.c $(INC) $(LIBSDIR) $(LIBS)

camonitor: $(SDIR)/camonitor.c
	$(CC) -o cgi-bin/camonitor $(SDIR)/camonitor.c $(INC) $(LIBSDIR) $(LIBS)

clean:
	rm -f $(ODIR)/*.o
	rm -f $(SDIR)/*~
