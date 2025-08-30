# makefile for task 6 (fetch + decrypt)
INCLDIR = ./include
CC      = gcc
CFLAGS  = -O2 -std=c89 -pedantic -Wall -Wextra
CFLAGS += -I$(INCLDIR)

OBJDIR  = obj
SRCDIR  = ./src

_DEPS    = tea.h
DEPS     = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS    = fetch.o decrypt.o tea.o
OBJS     = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

all: fetch decrypt

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

fetch: $(OBJDIR)/fetch.o $(OBJDIR)/tea.o
	$(CC) -o $@ $^ $(CFLAGS)

decrypt: $(OBJDIR)/decrypt.o $(OBJDIR)/tea.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: all clean
clean:
	rm -rf $(OBJDIR)/*.o fetch decrypt *~ core $(INCLDIR)/*~ body.bin header.txt



