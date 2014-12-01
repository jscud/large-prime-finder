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

#include "bit-u-int.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void FindNearbyPrime(BitUInt* candidate) {
  // If the starting point is even, make it odd.
  candidate->bits[0] = 1;

  BitUInt quotient;
  BitUInt remainder;

  // Establish the limit of the highest divisor we need to try.
  BitUInt max_divisor;
  //LargeUIntApproximateSquareRoot(candidate, &max_divisor);
  BitUIntClone(candidate, &max_divisor);
  BitUIntHalve(&max_divisor);

  // To report progress, track when we have tried each 2% of the possible
  // divisors.
  BitUInt one_fiftieth_max;
  BitUInt next_reporting_milestone;
  BitUInt fifty;
  fifty.num_bits = 6;
  fifty.bits[0] = 0;
  fifty.bits[1] = 1;
  fifty.bits[2] = 0;
  fifty.bits[3] = 0;
  fifty.bits[4] = 1;
  fifty.bits[5] = 1;
  BitUIntDiv(&max_divisor, &fifty, &one_fiftieth_max, &remainder);
  BitUIntClone(&one_fiftieth_max, &next_reporting_milestone);

  printf("Starting with possible prime ");
  BitUIntBase10Print(candidate);
  printf("\nMaximum divisor: ");
  BitUIntPrint(&max_divisor);
  printf("\nProgress: 0|-------20|-------40|-------60|-------80|------100|");
  printf("\n           x");
  fflush(stdout);

  // Try divisors starting with the smallest possible: 3.
  BitUInt divisor;
  divisor.num_bits = 2;
  divisor.bits[0] = 1;
  divisor.bits[1] = 1;
  BitUInt ignored;
  while (BitUIntCompare(&divisor, &max_divisor) >= 0) {
    BitUIntDiv(candidate, &divisor, &ignored, &remainder);
    if (remainder.num_bits == 0) {
      // Add two to the candidate to see if the next odd number is prime.
      BitUIntInc(candidate);
      BitUIntInc(candidate);
      // Start over with the lowest possible divisor (3).
      divisor.num_bits = 2;
      divisor.bits[0] = 1;
      divisor.bits[1] = 1;

      printf("\nTrying a new possible prime ");
      BitUIntBase10Print(candidate);
      printf("\nMaximum divisor: ");
      BitUIntPrint(&max_divisor);
      printf("\nProgress: 0|-------20|-------40|-------60|-------80|");
      printf("------100|\n           x");
      fflush(stdout);

      // New candidate so find a new cap for divisors.
      //LargeUIntApproximateSquareRoot(candidate, &max_divisor);
      BitUIntClone(candidate, &max_divisor);
      BitUIntHalve(&max_divisor);

      // Report the new candidate and reset our progress reporting.
      BitUIntDiv(&max_divisor, &fifty, &one_fiftieth_max, &remainder);
      BitUIntClone(&one_fiftieth_max, &next_reporting_milestone);
    } else {
      BitUIntInc(&divisor);
      BitUIntInc(&divisor);
      if (BitUIntCompare(&divisor, &next_reporting_milestone) < 1) {
        printf("x");
        fflush(stdout);
        BitUIntAdd(&one_fiftieth_max, &next_reporting_milestone);
      }
    }
  }

  // We ran out of divisors so the value stored in candidate is prime.
}

void PrintPrime(BitUInt* prime) {
  BitUIntPrint(prime);
  printf(" # int value: ");
  BitUIntBase10Print(prime);
  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <starting number in little endian binary format>\n",
           argv[0]);
    printf("For example %s 0111\n", argv[0]);
    return 1;
  }
  BitUInt prime;
  BitUIntLoad(strlen(argv[1]), argv[1], &prime);
  FindNearbyPrime(&prime);
  printf("\nPrime:\n");
  PrintPrime(&prime);
  printf("\n");

  return 0;
}

