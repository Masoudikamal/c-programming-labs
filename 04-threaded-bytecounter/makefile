# makefile for task4_threads

INCLDIR = ./include
CC      = gcc
CFLAGS  = -std=c89 -Wall -Wextra -pedantic -O2 -pthread
CFLAGS += -I$(INCLDIR)

OBJDIR  = obj

_DEPS    = task4_threads.h
DEPS     = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS    = task4_threads.o
OBJS     = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task4_threads: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~

