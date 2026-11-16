all: zad1 zad2 zad3 zad4

%: %.l
	flex -o $@.c $<
	gcc -o $@ $@.c

clean:
	rm -f zad1 zad2 zad3 zad4 *.c
