# makefile for flights

INCLDIR = ./include
CC      = gcc
CFLAGS  = -O2 -std=c89 -pedantic -Wall -Wextra -fsanitize=address -D_FORTIFY_SOURCE=2
CFLAGS  += -I$(INCLDIR)

OBJDIR  = obj
SRCDIR  = ./src

_DEPS   = flights.h
DEPS    = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS   = list.o main.o
OBJS    = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

flights: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ flights


