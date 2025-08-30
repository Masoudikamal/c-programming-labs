# makefile for task 5

INCLDIR = ./include
CC      = gcc
CFLAGS  = -O2 -std=c89 -pedantic -Wall -Wextra
CFLAGS += -I$(INCLDIR)

OBJDIR  = obj
SRCDIR  = ./src

_DEPS   = ewpdef.h task5_server.h
DEPS    = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS   = task5_main.o task5_server.o
OBJS    = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task5: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task5


