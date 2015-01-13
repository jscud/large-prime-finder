#include <stdio.h>
#include <gmp.h>

void FindNearbyPrime(mpz_t candidate) {
  mpz_t divisor;
  mpz_t limit;
  mpz_init(divisor);
  mpz_init(limit);
  mpz_set_ui(divisor, 2);
  mpz_t remainder;
  mpz_init(remainder);

  mpz_mod(remainder, candidate, divisor);
  if (mpz_size(remainder) == 0) {
    mpz_add_ui(candidate, candidate, 1);
  }
  
  // The candidate should now be odd.
  printf("Starting with possible prime %s\n", mpz_get_str(NULL, 10, candidate));

  mpz_set_ui(divisor, 3);
  mpz_root(limit, candidate, 2);
  // Round up the square root (may not really be necessary).
  mpz_add_ui(limit, limit, 1);
  while (mpz_cmp(limit, divisor) > 0) {
    mpz_mod(remainder, candidate, divisor);
    if (mpz_size(remainder) == 0) {
      printf("%s is not prime (divisible by %s)\n",
             mpz_get_str(NULL, 10, candidate), mpz_get_str(NULL, 10, divisor));
      // Incremend the candidate by 2.
      mpz_add_ui(candidate, candidate, 2);
      // Start over with a divisor of 3.
      mpz_set_ui(divisor, 3);
      mpz_root(limit, candidate, 2);
      mpz_add_ui(limit, limit, 1);
    } else {
      // Try the next higher divisor (step by 2).
      mpz_add_ui(divisor, divisor, 2);
    }
  }

  // We ran out of divisors so the value stored in candidate is prime.
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <starting number>\n", argv[0]);
    printf("For example %s 9087213\n", argv[0]);
    return 1;
  }
  mpz_t prime;
  mpz_init_set_str(prime, argv[1], 10);

  FindNearbyPrime(prime);
  printf("\nPrime:\n%s\n", mpz_get_str(NULL, 10, prime));

  return 0;
}
