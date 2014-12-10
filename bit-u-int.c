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
#include <string.h>

void BitUIntPrint(const BitUInt* this) {
  int i;
  for (i = 0; i < this->num_bits; i++) {
    printf("%i", this->bits[i]);
  }
}

void BitUIntBase10Print(const BitUInt* this) {
  char str_buffer[BASE_10_BIT_U_INT_BUFFER_SIZE];
  BitUIntBase10Store(this, BASE_10_BIT_U_INT_BUFFER_SIZE, str_buffer);
  printf("%s", str_buffer);
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

void BitUIntBase10Store(const BitUInt* this, int buffer_size, char* buffer) {
  char internal_buffer[BASE_10_BIT_U_INT_BUFFER_SIZE];
  int num_digits = 0;
  int i, power;
  BitUInt reduced_this;
  BitUInt quotient;
  BitUInt remainder;
  BitUInt ten;
  ten.num_bits = 4;
  ten.bits[0] = 0;
  ten.bits[1] = 1;
  ten.bits[2] = 0;
  ten.bits[3] = 1;

  BitUIntDiv(this, &ten, &quotient, &remainder);
  while (remainder.num_bits > 0 || quotient.num_bits > 0) {
    internal_buffer[num_digits] = 0;
    power = 1;
    for (i = 0; i < remainder.num_bits; i++) {
      internal_buffer[num_digits] += remainder.bits[i] * power;
      power *= 2;
    }
    BitUIntClone(&quotient, &reduced_this);
    num_digits++;
    BitUIntDiv(&reduced_this, &ten, &quotient, &remainder);
  }

  assert(num_digits < buffer_size - 1);

  for (i = 0; i < num_digits; i++) {
    buffer[i] = '0' + internal_buffer[num_digits - i - 1];
  }
  buffer[i] = '\0';
}


void BitUIntClone(const BitUInt* that, BitUInt* this) {
  this->num_bits = that->num_bits;
  int i;
  for (i = 0; i < that->num_bits; i++) {
    this->bits[i] = that->bits[i];
  }
}

void BitUIntTrim(BitUInt* this) {
  while (this->num_bits > 0 && this->bits[this->num_bits - 1] == 0) {
    this->num_bits--;
  }
}

void BitUIntInc(BitUInt* this) {
  int i;
  for (i = 0; i < this->num_bits; i++) {
    if (this->bits[i] == 1) {
      this->bits[i] = 0;
    } else {
      this->bits[i] = 1;
    	  return;
    }
  }
  this->num_bits++;
  this->bits[i] = 1;
}

void BitUIntDec(BitUInt* this) {
  assert(this->num_bits > 0);
  int i;
  for (i = 0; i < this->num_bits; i++) {
    if (this->bits[i] == 1) {
      this->bits[i] = 0;
      BitUIntTrim(this);
      return;
    } else {
      this->bits[i] = 1;
    }
  }
}

void BitUIntDouble(BitUInt* this) {
  assert(this->num_bits < MAX_NUM_BIT_U_INT_BITS);
  int i;
  this->num_bits++;
  for (i = this->num_bits - 1; i >= 1; i--) {
    this->bits[i] = this->bits[i - 1];
  }
  this->bits[0] = 0;
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

void BitUIntShiftInc(int num_bits, BitUInt* this) {
  assert(this->num_bits + num_bits < MAX_NUM_BIT_U_INT_BITS);
  if (this->num_bits == 0) {
    return;
  }
  int i;
  for (i = this->num_bits - 1; i >= 0; i--) {
    this->bits[i + num_bits] = this->bits[i];
  }
  this->num_bits += num_bits;
  memset(this->bits, 0, num_bits); 
}

void BitUIntShiftDec(int num_bits, BitUInt* this) {
  assert(this->num_bits - num_bits >= 0);
  this->num_bits -= num_bits;
  int i;
  for (i = 0; i < this->num_bits; i++) {
    this->bits[i] = this->bits[i + num_bits];
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

void BitUIntSub(const BitUInt* that, BitUInt* this) {
  assert(BitUIntLessThanOrEqual(that, this));
  int i;
  int borrow = 0;
  int diff;
  for (i = 0; i < that->num_bits || i < this->num_bits; i++) {
    if (i < that->num_bits && i < this->num_bits) {
      diff = this->bits[i] - that->bits[i] - borrow;
      borrow = diff < 0;
      this->bits[i] = diff & 1;
    } else if (i < this->num_bits) {
      diff = this->bits[i] - borrow;
      borrow = diff < 0;
      this->bits[i] = diff & 1;
    } 
  }
  BitUIntTrim(this);
}

void BitUIntMul(const BitUInt* that, BitUInt* this) {
  if (0 == that->num_bits) {
    this->num_bits = 0;
  }

  BitUInt original_this;
  BitUIntClone(this, &original_this);
  int i;
  // Step backwards through that, adding where there is a 1 and shifting for
  // each position.
  for (i = that->num_bits - 2; i >= 0; i--) {
    BitUIntDouble(this);
    if (that->bits[i]) {
      BitUIntAdd(&original_this, this);
    }
  }
}

void BitUIntDiv(const BitUInt* numerator, const BitUInt* denominator,
                BitUInt* quotient, BitUInt* remainder) {
  assert(denominator->num_bits > 0);
  quotient->num_bits = 0;
  remainder->num_bits = 0;
  if (numerator->num_bits == 0) {
    quotient->num_bits = 0;
    remainder->num_bits = 0;
    return;
  }

  assert(numerator->num_bits > 0);
  assert(numerator->bits[numerator->num_bits - 1] == 1);

  int numerator_index = numerator->num_bits;
  BitUInt partial_numerator;
  partial_numerator.num_bits = 0;

  while (BitUIntLessThanOrEqual(&partial_numerator, denominator) &&
         numerator_index > 0) {
    while (BitUIntLessThan(&partial_numerator, denominator) &&
           numerator_index > 0) {
      numerator_index--;
      if (partial_numerator.num_bits > 0) {
        BitUIntDouble(&partial_numerator);
      }
      if (quotient->num_bits > 0) {
        BitUIntDouble(quotient);
      }
      partial_numerator.bits[0] = numerator->bits[numerator_index];
      if (partial_numerator.num_bits == 0 && partial_numerator.bits[0] == 1) {
        partial_numerator.num_bits = 1;
      }

      BitUIntTrim(&partial_numerator);
    }
    if (BitUIntLessThanOrEqual(denominator, &partial_numerator)) {
      BitUIntSub(denominator, &partial_numerator);
      BitUIntTrim(&partial_numerator);
      if (quotient->num_bits == 0) {
        quotient->num_bits++;
      }
      quotient->bits[0] = 1;
    }
  }

  BitUIntClone(&partial_numerator, remainder);
}

void BitUIntApproximateSquareRoot(const BitUInt* this, BitUInt* root) {
  BitUInt two;
  two.num_bits = 2;
  two.bits[0] = 0;
  two.bits[1] = 1;

  BitUInt remainder;
  BitUInt estimate;
  // Start by using an estimate of just above half of the input.
  BitUIntDiv(this, &two, &estimate, &remainder);
  BitUIntInc(&estimate);

  BitUInt quotient;
  BitUInt next_estimate;
  next_estimate.num_bits = 0;

  int divided_evenly = 0;
  int comparison;
  while (1) {
    BitUIntDiv(this, &estimate, &quotient, &remainder);
    if (remainder.num_bits == 0) {
      divided_evenly = 1;
    } else {
      divided_evenly = 0;
    }
    BitUIntAdd(&estimate, &quotient);
    BitUIntDiv(&quotient, &two, &next_estimate, &remainder);

    comparison = BitUIntCompare(&next_estimate, &estimate);
    if (comparison < 1) {
      if (comparison < 0) {
        divided_evenly = 0;
      }
      break;
    }

    BitUIntClone(&next_estimate, &estimate);
  }

  BitUIntClone(&estimate, root);
  if (divided_evenly == 0) {
    BitUIntInc(root);
  }
}

int BitUIntCompare(const BitUInt* this, const BitUInt* that) {
  if (this->num_bits != that->num_bits) {
    return this->num_bits < that->num_bits ? 1 : -1;
  }

  int i;
  for (i = this->num_bits - 1; i >= 0; i--) {
    if (this->bits[i] != that->bits[i]) {
      return this->bits[i] < that->bits[i] ? 1 : -1;
    }
  }
  return 0;
}

int BitUIntLessThan(const BitUInt* this, const BitUInt* that) {
  return BitUIntCompare(this, that) == 1;
}

int BitUIntLessThanOrEqual(const BitUInt* this, const BitUInt* that) {
  return BitUIntCompare(this, that) > -1;
}

int BitUIntEqual(const BitUInt* this, const BitUInt* that) {
  return BitUIntCompare(this, that) == 0;
}
