/*
 * Copyright 2024 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Finds a number which has a high probability of being prime and spends
// a configurable amount of time performaing an exaustive check by trial
// division to prove that the number is prime. If the provided time limit
// is exceeded, the number is reported as probably prime.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <gmp.h>

char CANDIDATE_POSSIBLE_MOST_SIGNIFICANT_DIGIT[] = "123456789";

char CANDIDATE_POSSIBLE_LEAST_SIGNIFICANT_DIGIT[] = "1379";

int FIRST_FEW_PRIMES[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
    53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211,
    223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293,
    307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389,
    397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479,
    487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587,
    593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673,
    677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773,
    787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881,
    883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991,
    997, 1009, 1013, 1019};

int FIRST_FEW_PRIMES_COUNT =
    sizeof(FIRST_FEW_PRIMES) / sizeof(FIRST_FEW_PRIMES[0]);

void FillCandidateRandomly(mpz_t candidate, int num_digits) {
  char buffer[num_digits + 1];
  buffer[0] = CANDIDATE_POSSIBLE_MOST_SIGNIFICANT_DIGIT[rand() % 9];
  for (int i = 1; i < num_digits - 1; i++) {
    buffer[i] = '0' + (rand() % 10);
  }
  // The last digit of a prime number is one of 1, 3, 7, or 9 in base 10.
  buffer[num_digits - 1] = 
      CANDIDATE_POSSIBLE_LEAST_SIGNIFICANT_DIGIT[(rand() % 4)];
  buffer[num_digits] = '\0';
  mpz_init_set_str(candidate, buffer, 10);
}

void FillCandidateRandomlyByBits(mpz_t candidate, int num_bits) {
  char buffer[num_bits + 1];
  buffer[0] = '1';
  for (int i = 1; i < num_bits - 1; i++) {
    buffer[i] = '0' + (rand() & 1);
  }
  // Final digit must always be 1 since all primes are odd.
  buffer[num_bits - 1] = '1';
  buffer[num_bits] = '\0';
  mpz_init_set_str(candidate, buffer, 2);
}

// Reports whether a number is certainly not prime (0), certainly prime (2) or
// probably prime (1) after spending around timeout minutes trying to find a
// divisor. The timeout is a loose limit and doesn't start until the initial
// probable prime check is complete. The timeout is used just during the spot
// check trial divison.
int IsPrime(mpz_t candidate, int timeout) {
  int candidate_status = mpz_probab_prime_p(candidate, 100);
  if (candidate_status == 1) {
    printf("Starting prime verification (%i minutes)\n", timeout);
    mpz_t remainder;
    mpz_t limit;
    mpz_t divisor;
    time_t start_time, current_time;
    const int timeout_seconds = timeout * 60;
    int sample_counter = 0;
    mpz_init(remainder);
    mpz_init(limit);
    mpz_init(divisor);
    time(&start_time);

    // Start by checking the first few primes which should be less than the
    // largest possible divisor for a less than sever digit number.
    for (int i = 0; i < FIRST_FEW_PRIMES_COUNT; i++) {
      mpz_set_ui(divisor, FIRST_FEW_PRIMES[i]);
      mpz_mod(remainder, candidate, divisor);
      if (mpz_size(remainder) == 0) {
        // The candidate is not prime.
        return 0;
      }
    }

    // Check divisors sequentially starting with the next prime after the last
    // prime in the FIRST_FEW_PRIMES array.
    mpz_set_ui(divisor, 1021);
    mpz_root(limit, candidate, 2);
    while (mpz_cmp(limit, divisor) > 0) {
      mpz_mod(remainder, candidate, divisor);
      if (mpz_size(remainder) == 0) {
        // The candidate is not prime.
        return 0;
      }

      if (sample_counter == 10000) {
        sample_counter = 0;
        time(&current_time);
        if (difftime(current_time, start_time) > timeout_seconds) {
          // Ran out of time so we report the candidate as possibly prime.
          return 1;
        }
      }

      sample_counter++;

      mpz_add_ui(divisor, divisor, 2);
    }

    // We reached the limit without finding a divisor, so the candidate is
    // certainly prime.
    return 2;
  }
  return candidate_status;
}

void FindNearbyPrime(mpz_t candidate, int timeout) {
  mpz_t remainder;
  mpz_init(remainder);

  // If the candidate is even, add 1 to make it odd.
  mpz_mod_ui(remainder, candidate, 2);
  if (mpz_size(remainder) == 0) {
    mpz_add_ui(candidate, candidate, 1);
  }

  int candidate_counter = 1;
  int candidate_status = IsPrime(candidate, timeout);
  while (candidate_status == 0) {
    printf("Trying new candidate (%i)\n", candidate_counter);
    candidate_counter++;
    mpz_add_ui(candidate, candidate, 2);
    candidate_status = IsPrime(candidate, timeout);
  }

  if (candidate_status == 1) {
    printf("Probable prime:\n%s\n", mpz_get_str(NULL, 10, candidate));
  } else {
    printf("Prime:\n%s\n", mpz_get_str(NULL, 10, candidate));
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <num digits> <max minutes to run>\n", argv[0]);
    printf("For example %s 20 5\n", argv[0]);
    return 1;
  }
  srand(time(0));

  mpz_t candidate;
  // Using a number like b32 mean a 32 bit prime instead of 32 digits.
  if (argv[1][0] == 'b' || argv[1][0] == 'B') {
    // Read the number of bits skipping the leading b.
    int num_bits = atoi(argv[1] + sizeof(char));
    printf("Number of bits: %i\n", num_bits);
    FillCandidateRandomlyByBits(candidate, num_bits);
  } else {
    int num_digits = atoi(argv[1]);
    FillCandidateRandomly(candidate, num_digits);
  }

  printf("Starting.\n");
  FindNearbyPrime(candidate, atoi(argv[2]));
  return 0;
}

