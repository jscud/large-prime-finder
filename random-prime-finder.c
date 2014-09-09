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

#include "large-u-int.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

void FillCandidateRandomly(int num_bytes, LargeUInt* candidate) {
  LargeUIntInit(num_bytes, candidate);
  int i;
  for (i = 0; i < num_bytes; i++) {
    LargeUIntSetByte(rand() % 256, i, candidate);
  }
}

void FindNearbyPrime(LargeUInt* candidate) {
  if (LargeUIntGetByte(0, candidate) % 2 == 0) {
    LargeUIntIncrement(candidate);
  }

  LargeUInt quotient;
  LargeUInt remainder;

  // Establish the limit of the highest divisor we need to try.
  LargeUInt max_divisor;
  LargeUIntApproximateSquareRoot(candidate, &max_divisor);

  // To report progress, track when we have tried each 2% of the possible
  // divisors.
  LargeUInt one_fiftieth_max;
  LargeUInt next_reporting_milestone;
  LargeUInt fifty;
  LargeUIntInit(1, &fifty);
  LargeUIntSetByte(50, 0, &fifty);
  LargeUIntDivide(&max_divisor, &fifty, &one_fiftieth_max, &remainder);
  LargeUIntClone(&one_fiftieth_max, &next_reporting_milestone);

  printf("Starting with possible prime ");
  LargeUIntBase10Print(candidate, stdout);
  printf("\nMaximum divisor: ");
  LargeUIntPrint(&max_divisor, stdout);
  printf("\nProgress: 0|-------20|-------40|-------60|-------80|------100|");
  printf("\n           x");
  fflush(stdout);

  // Try divisors starting with the smallest possible: 3.
  LargeUInt divisor;
  LargeUIntInit(1, &divisor);
  LargeUIntSetByte(3, 0, &divisor);
  while (LargeUIntCompare(&divisor, &max_divisor) >= 0) {
    LargeUIntMod(candidate, &divisor, &remainder);
    if (LargeUIntNumBytes(&remainder) == 0) {
      LargeUIntAddByte(2, candidate);
      LargeUIntInit(1, &divisor);
      LargeUIntSetByte(3, 0, &divisor);

      printf("\nTrying a new possible prime ");
      LargeUIntBase10Print(candidate, stdout);
      printf("\nMaximum divisor: ");
      LargeUIntPrint(&max_divisor, stdout);
      printf("\nProgress: 0|-------20|-------40|-------60|-------80|");
      printf("------100|\n           x");
      fflush(stdout);

      // New candidate so find a new cap for divisors.
      LargeUIntApproximateSquareRoot(candidate, &max_divisor);

      // Report the new candidate and reset our progress reporting.
      LargeUIntDivide(&max_divisor, &fifty, &one_fiftieth_max, &remainder);
      LargeUIntClone(&one_fiftieth_max, &next_reporting_milestone);
    } else {
      LargeUIntAddByte(2, &divisor);
      if (LargeUIntCompare(&divisor, &next_reporting_milestone) < 1) {
        printf("x");
        fflush(stdout);
        LargeUIntAdd(&one_fiftieth_max, &next_reporting_milestone);
      }
    }
  }

  // We ran out of divisors so the value stored in candidate is prime.
}

void PrintPrime(LargeUInt* prime, FILE* out) {
  LargeUIntPrint(prime, out);
  fprintf(out, " # int value: ");
  LargeUIntBase10Print(prime, out);
  fprintf(out, "\n");
}

void GenerateRandomPrime(int num_bytes) {
  LargeUInt candidate;
  FillCandidateRandomly(num_bytes, &candidate);
  FindNearbyPrime(&candidate);
  printf("\nPrime:\n");
  PrintPrime(&candidate, stdout);
  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <number of bytes in the desired prime>\n", argv[0]);
    printf("For example %s 8\n", argv[0]);
    return 1;
  }
  int num_bytes = atoi(argv[1]);
  if (num_bytes < 1 || num_bytes > MAX_NUM_LARGE_U_INT_BYTES) {
    printf("Invalid number of bytes (%i) must be between 1 and %i\n",
           num_bytes, MAX_NUM_LARGE_U_INT_BYTES);
    return 1;
  }
  srand(time(0));
  GenerateRandomPrime(num_bytes);
  return 0;
}

