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

void PrintPrime(LargeUInt* prime, FILE* out) {
  LargeUIntPrint(prime, out);
  fprintf(out, " # int value: ");
  LargeUIntBase10Print(prime, out);
  fprintf(out, "\n");
}

void FindNearbyPrime(LargeUInt* candidate) {
  if (LargeUIntGetByte(0, candidate) % 2 == 0) {
    LargeUIntIncrement(candidate);
  }

  LargeUInt quotient;
  LargeUInt remainder;

  LargeUInt max_divisor;
  LargeUIntApproximateSquareRoot(candidate, &max_divisor);
  LargeUInt divisor;
  LargeUIntInit(1, &divisor);
  LargeUIntSetByte(3, 0, &divisor);
  while (LargeUIntCompare(&divisor, &max_divisor) >= 0) {
    LargeUIntDivide(candidate, &divisor, &quotient, &remainder);
    if (LargeUIntNumBytes(&remainder) == 0) {
      LargeUIntAddByte(2, candidate);
      LargeUIntInit(1, &divisor);
      LargeUIntSetByte(3, 0, &divisor);
      LargeUIntApproximateSquareRoot(candidate, &max_divisor);
    } else {
      LargeUIntAddByte(2, &divisor);
    }
  }

  // We ran out of divisors so the value stored in candidate is prime.
}

void LoadNextPrime(FILE* primes, LargeUInt* prime) {
  LargeUIntRead(primes, prime);
}

void FindHighestPrime(char* filename, LargeUInt* prime) {
  FILE* primes = fopen(filename, "r");
  if (primes == NULL) {
    LargeUIntInit(0, prime);
    return;
  }

  LargeUInt next_prime;
  LoadNextPrime(primes, &next_prime);
  while (LargeUIntNumBytes(&next_prime) != 0) {
    LargeUIntClone(&next_prime, prime);
    LoadNextPrime(primes, &next_prime);
  }
  fclose(primes);
  return;
}

void AppendPrime(char* filename, LargeUInt* prime) {
  FILE* primes = fopen(filename, "a");
  PrintPrime(prime, primes);
  fclose(primes);
}

void GeneratePrimes(char* filename) {
  // Start by finding the higest prime that we have so far.
  LargeUInt candidate;
  printf("Looking for highest prime already found.\n");
  FindHighestPrime(filename, &candidate);
  printf("Starting from highest prime found so far: ");
  PrintPrime(&candidate, stdout);
  printf("\n");

  // Add two to start trying new primes.
  LargeUIntAddByte(2, &candidate);

  while(1) {
    FindNearbyPrime(&candidate);
    AppendPrime(filename, &candidate);
    printf("Found prime: ");
    PrintPrime(&candidate, stdout);
    LargeUIntAddByte(2, &candidate);
  }
}

int main() {
  GeneratePrimes("primes");
}

