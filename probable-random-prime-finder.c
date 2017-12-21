/*
 * Copyright 2014 Google Inc. All rights reserved.
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

void FillCandidateRamdomly(mpz_t candidate, int num_digits) {
  char buffer[num_digits + 1];
  for (int i = 0; i < num_digits; i++) {
    buffer[i] = '0' + (rand() % 10);
  }
  buffer[num_digits] = '\0';
  mpz_init_set_str(candidate, buffer, 10);
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

    mpz_set_ui(divisor, 3);
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

  int num_digits = atoi(argv[1]);

  mpz_t candidate;
  FillCandidateRamdomly(candidate, num_digits);
  printf("Starting.\n");
  FindNearbyPrime(candidate, atoi(argv[2]));
  return 0;
}

