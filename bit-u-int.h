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

// A string buffer to hold a LargeUInt's base 10 represenation could be up to
// three times the number of bytes with one more byte for the trailing null
// terminator. This should be a safe overestimate.
#define BASE_10_BIT_U_INT_BUFFER_SIZE MAX_NUM_BIT_U_INT_BITS / 3 + 1

typedef struct {
  int num_bits;
  uint8_t bits[MAX_NUM_BIT_U_INT_BITS];
} BitUInt;

// Sends the binary representation of the integer to stdout. The bits are
// printed in little endian order, least significant bit first.
void BitUIntPrint(const BitUInt* this);

// Prints out the base 10 (decimal) equivalent of the value stored in the
// binary unsigned interger. This lists high order digits first as one would
// typically see an integer written.
void BitUIntBase10Print(const BitUInt* this);

// Reads the text representation of a binary unsigned integer from a string
// and stores loaded value into the provided location.
void BitUIntLoad(int buffer_size, char* buffer, BitUInt* this);

// Writes a text representation of this binary unsigned integer into the
// provided string buffer.
void BitUIntStore(const BitUInt* this, int buffer_size, char* buffer);

// Writes the number as decimal text, with the high order digits listed
// first.
void BitUIntBase10Store(const BitUInt* this, int buffer_size, char* buffer);

// Copies the value from the first argument into the second argument.
void BitUIntClone(const BitUInt* that, BitUInt* this);

// Removes leading zeros from the binary unsigned integer.
void BitUIntTrim(BitUInt* this);

// Adds 1 to the provided binary unsigned integer.
void BitUIntInc(BitUInt* this);

// Subtracts 1 from the provided binary unsigned integer.
void BitUIntDec(BitUInt* this);

// Double the value of the unsigned integer.
void BitUIntDouble(BitUInt* this);

// Reduces the value of the insiged integer to half and returns the low order
// bit from the binary number which has been dropped.
int BitUIntHalve(BitUInt* this);

// Shifts the bits in the integer to higher values, filling in zeros on low
// order bits.
void BitUIntShiftInc(int num_bits, BitUInt* this);

// Shifts bits in the integer to lower values. The low order bits are dropped.
void BitUIntShiftDec(int num_bits, BitUInt* this);

// Adds the first argument to the second and stores in the second argument.
void BitUIntAdd(BitUInt* that, BitUInt* this);

// Subtracts the first argument from the second argument. The value of the
// second argument is set to the difference of this and that.
void BitUIntSub(const BitUInt* that, BitUInt* this);

// Multiplies the second argument by the first, storing the result in the
// location provided in the second argument. 
void BitUIntMul(const BitUInt* that, BitUInt* this);

// Divides the numerator by the denominator storing the results in the
// quotient and remainder.
void BitUIntDiv(const BitUInt* numerator, const BitUInt* denominator,
                BitUInt* quotient, BitUInt* remainder);

// Finds an integer that is close to the square root of the first argument
// without being less than the actual square root. Intended for a rough
// overestimate of the square root.
void BitUIntApproximateSquareRoot(const BitUInt* this, BitUInt* root);

// Compares two large unsigned integers, returning 0 if they are equal, 1 if
// the second is greater than the first, and -1 if the first is greater than
// the second.
// this == that --> 0
// this < that  --> 1
// this > that  --> -1
int BitUIntCompare(const BitUInt* this, const BitUInt* that);

// Returns 1 if the first argument is less than the second, otherwise 0.
int BitUIntLessThan(const BitUInt* this, const BitUInt* that);

// Returns 1 if the first argument is less than the second or if both are
// equal. Otherwise returns 0.
int BitUIntLessThanOrEqual(const BitUInt* this, const BitUInt* that);

// Returns 1 if both arguments are equal, otherwise 0.
int BitUIntEqual(const BitUInt* this, const BitUInt* that);


#endif
