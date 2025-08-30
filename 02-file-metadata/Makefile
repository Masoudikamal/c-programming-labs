# makefile for filemeta

INCLDIR = ./include
CC      = gcc
CFLAGS  = -O2 -pedantic -Wall -Wextra -include stdio.h 
CFLAGS	+= -I$(INCLDIR)

OBJDIR  = obj
SRCDIR  = ./src

_DEPS   = hash.h count.h sum.h
DEPS    = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS   = main.o hash.o count.o sum.o
OBJS    = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

filemeta: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ pgexam25_output.bin filemeta



