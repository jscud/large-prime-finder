/*
 * Copyright 2017 Google Inc. All rights reserved.
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

// Converts a number given on the command line from a starting base to the
// desired base. Usage:
// ./base-x-to-y <starting base> <ending base> <number in starting base>
// The bases can be from 2 to 32.
// Example, converting a number from decimal to hexidecimal (base 16):
// ./base-x-to-y 10 16 255

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <gmp.h>

int main(int argc, char *argv[]) {
  // ./base-x-to-y <starting base> <ending base> <number in starting base>
  if (argc < 4) {
    printf("Usage: %s <starting base> <ending base> <number in starting base>\n", argv[0]);
    printf("For example %s 10 16 255\n", argv[0]);
    return 1;
  }
  int starting_base = atoi(argv[1]);
  int ending_base = atoi(argv[2]);
  if (starting_base < 2 || starting_base > 32 ||
      ending_base < 2 || ending_base > 32) {
    printf("A base must be between 2 and 32.");
    return 1;
  }

  mpz_t number;
  mpz_init_set_str(number, argv[3], starting_base);
  printf("%s\n", mpz_get_str(NULL, ending_base, number));
  return 0;
}

