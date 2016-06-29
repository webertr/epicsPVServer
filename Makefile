# #objects = $(addprefix $(OBJDIR)/, bindSocketTest.o getHTMLFile.o listenTest.o main.o printIPAddress.o socketTest.o)

# INCLUDEDIR = $(shell pwd)/include
# SRCDIR = $(shell pwd)/src
# OBJDIR = $(shell pwd)/obj
# LIBS  = -lpthread
# make --just-print to debug
# make -B will force a make

vpath %.c  src
vpath %.h  include

OUT = lib/alib.a
CC = gcc
ODIR = obj
SDIR = src
INC = -Imyfunctions.h
LIBS = -lpthread

_OBJS = main.o bindSocketTest.o getHTMLFile.o listenTest.o printIPAddress.o socketTest.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

pv_server: $(OBJS)
	$(CC) -o pv_server $(OBJS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(SDIR)/*~
