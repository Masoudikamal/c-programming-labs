# makefile for threads

INCLDIR = ./include
CC      = gcc
CFLAGS  = -std=c89 -Wall -Wextra -pedantic -O2 -pthread
CFLAGS += -I$(INCLDIR)

OBJDIR  = obj
SRCDIR  = ./src

_DEPS    = threads.h
DEPS     = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS    = threads.o
OBJS     = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

threads: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~


