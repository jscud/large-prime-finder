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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

const char HEX_BYTES[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                          'A', 'B', 'C', 'D', 'E', 'F'};

void BigIntPrint(uint_fast64_t x, FILE *out) {
  int num_bytes;
  if (x <= 0xFF) {
    num_bytes = 1;
  } else if (x <= 0xFFFF) {
    num_bytes = 2;
  } else if (x <= 0xFFFFFF) {
    num_bytes = 3;
  } else if (x <= 0xFFFFFFFF) {
    num_bytes = 4;
  } else {
    printf("reached size limit");
    exit(1);
  }

  fprintf(out, "0%c00_", HEX_BYTES[num_bytes]);
  uint_fast64_t x_copy = x;
  while (x > 0) {
    fprintf(out, "%c%c", HEX_BYTES[x >> 4 & 0x0F], HEX_BYTES[x & 0x0F]);
    x >>= 8;
  }
  fprintf(out, " # int value: %llu\n", x_copy);
}

int HexCharToNibble(char hex_char) {
  if (hex_char >= '0' && hex_char <= '9') {
    return hex_char - '0';
  } else if (hex_char >= 'A' && hex_char <= 'F') {
    return hex_char + 10 - 'A';
  } else {
    return -1;
  }
}

uint_fast64_t FindNearbyPrime(uint_fast64_t candidate) {
  if (candidate % 2 == 0) {
    candidate++;
  }

  uint_fast64_t max_divisor = candidate / 2;
  for (uint_fast64_t divisor = 3; divisor <= max_divisor;) {
    if (candidate % divisor == 0) {
      candidate += 2;
      divisor = 3;
      max_divisor = candidate / 2;
    } else {
      max_divisor = candidate / divisor;
      divisor += 2;
    }
  }

  // We ran out of divisors so candidate is prime.
  return candidate;
}

uint_fast64_t LoadNextPrime(FILE* primes) {
  if (primes == NULL) {
    return 0;
  }

  int current = fgetc(primes);
  int state = 0;
  int num_bytes = 0;
  int byte_index = 0;
  int current_value = -1;
  uint_fast64_t result = 0;
  int shift = 0;
  while (1) {
    if (feof(primes)) {
      break;
    }

    if (current == '#') {
      state = 10;
    }

    if (state == 10) {
      if (current == '\n') {
        state = 0;
      }
      current = fgetc(primes);
      continue;
    }

    current_value = HexCharToNibble(current);
    if (current_value == -1) {
      // Skip over non hex characters.
      current = fgetc(primes);
      continue;
    }

    switch (state) {
      // First char of 8 in the num_bytes.
      case 0:
        num_bytes = current_value << 4;
        state = 1;
        break;
      case 1:
        num_bytes += current_value;
        state = 2;
        break;
      case 2:
        num_bytes += current_value << 12;
        state = 3;
        break;
      case 3:
        num_bytes += current_value << 8;
        state = 8;
        break;
      case 4:
        num_bytes += current_value << 20;
        state = 5;
        break;
      case 5:
        num_bytes += current_value << 16;
        state = 6;
        break;
      case 6:
        num_bytes += current_value << 28;
        state = 7;
        break;
      case 7:
        num_bytes += current_value << 24;
        state = 8;  // Reached the end of num_bytes;
        result = 0;
        byte_index = 0;
        break;
      case 8:
        result += ((current_value << 4) << shift);
        state = 9;
        break;
      case 9:
        result += (current_value << shift);
        byte_index++;
        shift += 8;
        if (byte_index < num_bytes) {
          state = 8;
        } else {
          return result;
        }
        break;
      case 10:
        break;
    }
    current = fgetc(primes);
  }
  return 0;
}

uint_fast64_t FindHighestPrime(char* filename) {
  FILE* primes = fopen(filename, "r");
  if (primes == NULL) {
    return 0;
  }

  uint_fast64_t next_prime = LoadNextPrime(primes);
  uint_fast64_t result;
  while (next_prime != 0) {
    result = next_prime;
    next_prime = LoadNextPrime(primes);
  }
  fclose(primes);
  return result;
}

void AppendPrime(char* filename, uint_fast64_t prime) {
  FILE* primes = fopen(filename, "a");
  BigIntPrint(prime, primes);
  fclose(primes);
}

void GeneratePrimes(char* filename) {
  // Start by finding the higest prime that we have so far.
  printf("Looking for highest prime already found.\n");
  uint_fast64_t candidate = FindHighestPrime(filename);
  printf("Starting from highest prime found so far: ");
  BigIntPrint(candidate, stdout);

  uint_fast64_t first_candidate = candidate;
  uint_fast64_t prime;

  // Add two to start trying new primes.
  candidate += 2;

  while(candidate > first_candidate) {
    prime = FindNearbyPrime(candidate);
    AppendPrime(filename, prime);
    printf("Found prime: ");
    BigIntPrint(prime, stdout);
    candidate = prime + 2;
  }
}

int main() {
  GeneratePrimes("primes");
}

