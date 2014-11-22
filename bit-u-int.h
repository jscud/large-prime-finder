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

#ifndef BIT_U_INT_H
#define BIT_U_INT_H

#include <stdint.h>

#define MAX_NUM_BIT_U_INT_BITS 200

typedef struct {
  int num_bits;
  uint8_t bits[MAX_NUM_BIT_U_INT_BITS];
} BitUInt;

// Reads the text representation of a binary unsigned integer from a string
// and stores loaded value into the provided location.
void BitUIntLoad(int buffer_size, char* buffer, BitUInt* this);

// Writes a text representation of this binary unsigned integer into the
// provided string buffer.
void BitUIntStore(const BitUInt* this, int buffer_size, char* buffer);

// Copies the value from the first argument into the second argument.
void BitUIntClone(const BitUInt* that, BitUInt* this);

// Removes leading zeros from the binary unsigned integer.
void BitUIntTrim(BitUInt* this);

#endif
