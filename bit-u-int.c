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

#include <assert.h>
#include <stdio.h>

void BitUIntPrint(const BitUInt* this) {
  int i;
  for (i = 0; i < this->num_bits; i++) {
    printf("%i", this->bits[i]);
  }
}

void BitUIntLoad(int buffer_size, char* buffer, BitUInt* this) {
  int i;
  char current;
  this->num_bits = 0;
  for (i = 0; i < buffer_size; i++) {
    assert(i < MAX_NUM_BIT_U_INT_BITS);
    current = buffer[i];
    if (current == '0' || current == '1') {
      this->bits[i] = current - '0';
      this->num_bits++;
    } else {
      break;
    }
  }
  BitUIntTrim(this);
}

void BitUIntStore(const BitUInt* this, int buffer_size, char* buffer) {
  assert(this->num_bits < buffer_size);
  int i;
  for (i = 0; i < this->num_bits; i++) {
    buffer[i] = this->bits[i] + '0';
  }
  buffer[i] = 0;
}

void BitUIntClone(const BitUInt* that, BitUInt* this) {
  this->num_bits = that->num_bits;
  int i;
  for (i = 0; i < that->num_bits; i++) {
    this->bits[i] = that->bits[i];
  }
}

void BitUIntTrim(BitUInt* this) {
  while (this->bits[this->num_bits - 1] == 0) {
    this->num_bits--;
  }
}

void BitUIntDouble(BitUInt* this) {
  assert(this->num_bits < MAX_NUM_BIT_U_INT_BITS);
  int i;
  for (i = this->num_bits; i > 1; i--) {
    this->bits[i + 1] = this->bits[i];
  }
  this->bits[0] = 0;
  this->num_bits++;
}

int BitUIntHalve(BitUInt* this) {
  if (this->num_bits > 0) {
    int low_bit = this->bits[0];
    int i;
    for (i = 1; i < this->num_bits; i++) {
      this->bits[i - 1] = this->bits[i];
    }
    this->num_bits--;
    return low_bit;
  } else {
    return 0;
  }
}

void BitUIntAdd(BitUInt* that, BitUInt* this) {
  int carry = 0;
  int sum;
  int i;
  for (i = 0; i < that->num_bits || i < this->num_bits; i++) {
    if (i < that->num_bits && i < this->num_bits) {
      sum = this->bits[i] + that->bits[i] + carry;
      carry = sum > 1;
      this->bits[i] = sum & 1;
    } else if (i < that->num_bits) {
      sum = that->bits[i] + carry;
      carry = sum > 1;
      this->bits[i] = sum & 1;
      this->num_bits++;
    } else if (i < this->num_bits) {
      sum = this->bits[i] + carry;
      carry = sum > 1;
      this->bits[i] = sum & 1;
    } 
  }
  if (carry) {
    this->bits[i] = 1;
    this->num_bits++;
  }
}
