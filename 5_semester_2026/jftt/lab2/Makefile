CC = gcc
CFLAGS = -O2 -Wall -Wno-unused-function
PROGS = zad1 zad2 zad3 zad4

all: $(PROGS)

%: %.l
	flex -o $@.c $<
	$(CC) $(CFLAGS) -o $@ $@.c

clean:
	rm -f $(PROGS) $(PROGS:=.c)
