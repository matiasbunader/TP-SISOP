PROCESS_NAME=matelib
IDIR =./include
SHARED_IDIR = ../utils/include
CC=gcc
CFLAGS=-I$(IDIR) -I$(SHARED_IDIR) -g -Wall

ODIR=./obj
LDIR=./lib
SRCDIR =./src
LIBS=-lcommons -lpthread -lreadline -lcunit -lrt

SHARED_SRCDIR = ../utils/src
SHARED_ODIR = ../utils/obj

DEPS = $(wildcard $(IDIR)/*.h)

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c,$(ODIR)/%.o,$(SRC))

LNAME = lib$(PROCESS_NAME)

$(ODIR)/$(PROCESS_NAME).o: $(SRCDIR)/$(PROCESS_NAME).c $(DEPS) $(SHARED_DEPS)
	$(CC) -c -fpic -o $@ $< $(CFLAGS)

$(LDIR)/$(LNAME).so: $(ODIR)/$(PROCESS_NAME).o
	$(CC) -fpic -shared -o $(LDIR)/$(LNAME).so.1.0.0 $(ODIR)/$(PROCESS_NAME).o -Wl,-soname,$(LNAME).so.1

link: $(LDIR)/$(LNAME).so
	ln -s $(LNAME).so.1.0.0 lib/$(LNAME).so.1 && ln -s $(LNAME).so.1 lib/$(LNAME).so

all: $(ODIR)/$(PROCESS_NAME).o $(LDIR)/$(LNAME).so link

$(SHARED_ODIR)/%.o: $(SHARED_SRCDIR)/%.c $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_DEPS = $(wildcard $(SHARED_IDIR)/*.h)

SHARED_SRC = $(wildcard $(SHARED_SRCDIR)/*.c)
SHARED_OBJ = $(patsubst $(SHARED_SRCDIR)/%.c,$(SHARED_ODIR)/%.o,$(SHARED_SRC))

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(PROCESS_NAME) $(SHARED_ODIR)/*.o $(LDIR)/$(LNAME).so*