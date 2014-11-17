# Resumable Prime Finder for up to 64 bit numbers.
resumable-prime-finder: resumable-prime-finder.c
	gcc resumable-prime-finder.c -o resumable-prime-finder

# LargeUInt rules.
large-u-int-test: large-u-int.o large-u-int-test.o
	gcc large-u-int.o large-u-int-test.o -o large-u-int-test

large-u-int-test.o: large-u-int-test.c large-u-int.h
	gcc -c large-u-int-test.c

# Resumable Prime Finder supporting large unsigned integers.
large-u-int-resumable-prime-finder: large-u-int-resumable-prime-finder.o large-u-int.o
	gcc large-u-int-resumable-prime-finder.o large-u-int.o -o large-u-int-resumable-prime-finder

large-u-int-resumable-prime-finder.o: large-u-int-resumable-prime-finder.c large-u-int.h
	gcc -c large-u-int-resumable-prime-finder.c

# Random Prime Finder to find a single very large prime.
random-prime-finder: random-prime-finder.o large-u-int.o
	gcc random-prime-finder.o large-u-int.o -o random-prime-finder

random-prime-finder.o: random-prime-finder.c large-u-int.h
	gcc -c random-prime-finder.c

# Next Prime Finder to find a single prime from a starting integer.
next-prime-finder: next-prime-finder.o large-u-int.o
	gcc next-prime-finder.o large-u-int.o -o next-prime-finder

next-prime-finder.o: next-prime-finder.c large-u-int.h
	gcc -c next-prime-finder.c

large-u-int.o: large-u-int.c large-u-int.h
	gcc -c large-u-int.c

clean:
	rm *.o large-u-int-test resumable-prime-finder large-u-int-resumable-prime-finder random-prime-finder next-prime-finder
