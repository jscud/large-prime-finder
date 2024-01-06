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

Try it right now in a Cloud Shell virtual machine:

<a href="https://console.cloud.google.com/cloudshell/open?git_repo=https://github.com/jscud/large-prime-finder&tutorial=tutorial.md">
<img alt="Open in Cloud Shell" src ="http://gstatic.com/cloudssh/images/open-btn.png">
</a>

# Getting Started Tutorial

The Large Prime Finder includes programs that are capable of finding prime
numbers that have tens or hunreds of digits.

This guide will show you how to run two of the programs that find prime numbers
in different ways.

1. **resumable-prime-finder**: looks for primes in order and stores them in a
file.
1. **probable-random-prime-finder**: generates a random number which is
probably a prime, that has the specified number digits.

Ready? Lets go 😊

## Resumable Prime Finder - Set up

This program finds prime numbers sequentially and stores them in a file named
"primes" in your current directory.

When the program is started, it looks in the primes file for the largest prime
you have found so far and starts searching for prime numbers from there.

### Build the prime finder

Compile the prime finder code by running

```bash
make resumable-prime-finder
```

### Ready to run

If no error message is shown, you should now be ready to run the prime finder.

## Resumable Prime Finder - Running

### Examine your current list of primes

Before running the prime finder, look at the primes that are already in the
file by running:

```bash
tail primes
```

Now run the prime finder to add more prime numbers to the primes file.

```bash
./resumable-prime-finder
```

### Stop the prime finder

The prime finder will run for as long as you let it and the primes file will
continue to grow. To stop the resumable prime finder, hit Ctrl+c.

It may take a few seconds for the prime finder to halt.

### Look at the results

To see the last few primes that you found as you ran the prime finder, run

```bash
tail primes
```

Next, you will learn about the probable prime finder.

## Probable Prime Finder - Set up

If you want to find a few large prime numbers, searching primes in order may
not be the best way to look for them. There are a lot of prime numbers and
storing them all in a file can take a lot of space.

Unlike an exhaustive search for primes, the probable prime finder will find a
single number which has a high probability of being prime.

### Installing prerequisites

The probable prime finder uses The GNU Multiple Precision Arithmetic Library
(GMP) for working with large numbers. Before building the prime finder,
install GMP by running the following:

```bash
sudo apt-get install libgmp3-dev
```

### Build the probable prime finder

Compile the probable prime finder by running:

```bash
make probable-random-prime-finder
```

### Ready to run

If no error message is shown, you should now be ready to run the prime finder.


## Probable Prime Finder - Finding a small prime

This prime finder attempts to find a prime number with a number of digits that
you request. Since verifying that a number is prime can take a long time, it
is possible to set a time limit for how long the prime finder will work at
trying to verify the random number is prime.

As the program runs, it will display a message if a random number was rejected
as non prime and show that it is trying another candidate number.

With a small number of digits, it is often possible to quickly find a number
and completely verify that it is prime. Try finding a 10-digit prime by
running the following:

```bash
./probable-random-prime-finder 10 2
```

This gives the prime finder two minutes to try to verify each candidate. With
that amount of time, it should be possible to show that the number is
definitely prime.

The output should look like something like:

```
Starting prime verification (2 minutes)
Prime:
14490602325933967
```

Note that if you'd rather ask for a prime number that is a certain number of
bits long, instead of specifying number of digits, you can specify the number
of bits by putting a letter b in front of the number. For example, this will
generate a 32-bit prime number:

```bash
./probable-random-prime-finder b32 2
```
    
## Probable Prime Finder -  Finding a large number that is probably prime

For larger numbers, it may take a very long time to determine if the number
is definitely prime, so you may want to limit the time the program spends on
testing each number.

Here is an example that atteps to find a 30-digit prime number with a time
limit of two minutes for running a sequential prime check.

```bash
./probable-random-prime-finder 30 2
```

Two minutes is likely not enough time to verify a number this large is prime,
so the output will indicate that the program thinks this number is a
"probable prime" as opposed to definitely prime.

The output in that case will look like this:

```
Starting prime verification (2 minutes)
Probable prime:
976742075811260374847156524901
```

How probable is it that this number is prime? The prime finder uses GMP's
`mpz_probab_prime_p` function to test the candidate number with 100 reps of
the Miller-Rabin probabilistic primality tests. The GMP docs stat that "A
composite number will be identified as a prime with a probability of less
than 4^(-reps)." After this check, trial division is used until the time
limit is reached. Hopefully this meets your needs.
