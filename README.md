large-prime-finder
==================

Large Prime Finder includes small simple example programs that are capable of
finding prime numbers that have tens or hunreds of digits.

The first example is the resumable prime finder which calculates numbers that
are prime and writes them to a file called "primes". To compile and run do:

make resumable-prime-finder
./resumable-prime-finder

The progam will run until killed (control-c) or it finds the last prime that
will fit in an unsigned 64 bit number.

Upon start, the resumable prime finder will start looking for prime numbers
larger than the number at the end of the primes file.
