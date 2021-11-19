PROCESS_NAME=carpincho
IDIR =./include
SHARED_IDIR = ../utils/include
CC=gcc
CFLAGS=-I$(IDIR) -I$(SHARED_IDIR) -g -Wall -L ../matelib/lib

ODIR=./obj
SRCDIR =./src
LIBS=-lcommons -lpthread -lreadline -lcunit -lrt -lmatelib

SHARED_SRCDIR = ../utils/src
SHARED_ODIR = ../utils/obj

DEPS = $(wildcard $(IDIR)/*.h)

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c,$(ODIR)/%.o,$(SRC))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(SHARED_ODIR)/%.o: $(SHARED_SRCDIR)/%.c $(SHARED_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_DEPS = $(wildcard $(SHARED_IDIR)/*.h)

SHARED_SRC = $(wildcard $(SHARED_SRCDIR)/*.c)
SHARED_OBJ = $(patsubst $(SHARED_SRCDIR)/%.c,$(SHARED_ODIR)/%.o,$(SHARED_SRC))

$(PROCESS_NAME): $(OBJ) $(SHARED_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(PROCESS_NAME) $(SHARED_ODIR)/*.o 