# Resumable Prime Finder for up to 64 bit numbers.
resumable-prime-finder: resumable-prime-finder.c
	gcc -O3 -std=c99 resumable-prime-finder.c -o resumable-prime-finder

# LargeUInt rules.
large-u-int-test: large-u-int.o large-u-int-test.o
	gcc -O3 large-u-int.o large-u-int-test.o -o large-u-int-test

large-u-int-test.o: large-u-int-test.c large-u-int.h
	gcc -c -O3 -std=c99 large-u-int-test.c

large-u-int.o: large-u-int.c large-u-int.h
	gcc -c -O3 -std=c99 large-u-int.c

# Resumable Prime Finder supporting large unsigned integers.
large-u-int-resumable-prime-finder: large-u-int-resumable-prime-finder.o large-u-int.o
	gcc -O3 large-u-int-resumable-prime-finder.o large-u-int.o -o large-u-int-resumable-prime-finder

large-u-int-resumable-prime-finder.o: large-u-int-resumable-prime-finder.c large-u-int.h
	gcc -c -O3 -std=c99 large-u-int-resumable-prime-finder.c

# Random Prime Finder to find a single very large prime.
random-prime-finder: random-prime-finder.o large-u-int.o
	gcc -O3 random-prime-finder.o large-u-int.o -o random-prime-finder

random-prime-finder.o: random-prime-finder.c large-u-int.h
	gcc -c -O3 -std=c99 random-prime-finder.c

# Next Prime Finder to find a single prime from a starting integer.
next-prime-finder: next-prime-finder.o large-u-int.o
	gcc -O3 next-prime-finder.o large-u-int.o -o next-prime-finder

next-prime-finder.o: next-prime-finder.c large-u-int.h
	gcc -c -O3 -std=c99 next-prime-finder.c

# Next Prime Finder using the binary large integer library.
next-prime-finder-bits: next-prime-finder-bits.o bit-u-int.o
	gcc -O3 next-prime-finder-bits.o bit-u-int.o -o next-prime-finder-bits

next-prime-finder-bits.o: next-prime-finder-bits.c bit-u-int.h
	gcc -c -O3 -std=c99 next-prime-finder-bits.c

# Next Prime Finder using The GNU Multiple Precision Arithmetic Library
next-prime-finder-gmp: next-prime-finder-gmp.c
	gcc -o next-prime-finder-gmp -O3 -std=c99 next-prime-finder-gmp.c -lgmp -lm

probable-random-prime-finder: probable-random-prime-finder.c
	gcc -o probable-random-prime-finder -O3 -std=c99 probable-random-prime-finder.c -lgmp -lm

base-x-to-base-y: base-x-to-base-y.c
	gcc -o base-x-to-base-y -O3 -std=c99 base-x-to-base-y.c -lgmp -lm

# BitUInt rules.
bit-u-int-test: bit-u-int.o bit-u-int-test.o
	gcc -O3 bit-u-int.o bit-u-int-test.o -o bit-u-int-test

bit-u-int-test.o: bit-u-int-test.c bit-u-int.h
	gcc -c -O3 -std=c99 bit-u-int-test.c

bit-u-int.o: bit-u-int.c bit-u-int.h
	gcc -c -O3 -std=c99 bit-u-int.c


clean:
	rm -f *.o large-u-int-test resumable-prime-finder large-u-int-resumable-prime-finder random-prime-finder next-prime-finder bit-u-int-test next-prime-finder-bits next-prime-finder-gmp probable-random-prime-finder base-x-to-base-y
