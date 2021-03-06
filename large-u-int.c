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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char kHexBytes[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Exits the program after sending the message to stderr.
static void ErrorOut(char* message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

static int HexCharToNibble(char hex_char) {
  if (hex_char >= '0' && hex_char <= '9') {
    return hex_char - '0';
  } else if (hex_char >= 'A' && hex_char <= 'F') {
    return hex_char + 10 - 'A';
  } else {
    return -1;
  }
}

void LargeUIntPrint(const LargeUInt* this, FILE* out) {
  int i;
  fprintf(out, "%c%c%c%c_",
          kHexBytes[this->num_bytes_ >> 4 & 0x0F],
          kHexBytes[this->num_bytes_ & 0x0F], 
          kHexBytes[this->num_bytes_ >> 12 & 0x0F],
          kHexBytes[this->num_bytes_ >> 8 & 0x0F]);
  for (i = 0; i < this->num_bytes_; i++) {
    fprintf(out, "%c%c", kHexBytes[this->bytes_[i] >> 4 & 0x0F],
            kHexBytes[this->bytes_[i] & 0x0F]);
  }
}

void LargeUIntBase10Print(const LargeUInt* this, FILE* out) {
  char str_buffer[BASE_10_LARGE_U_INT_BUFFER_SIZE];
  LargeUIntBase10Store(this, BASE_10_LARGE_U_INT_BUFFER_SIZE, str_buffer);
  fprintf(out, "%s", str_buffer);
}

// Processes one input character to set the value of this. Returns 1 to
// continue parsing the 0 when we've reached the end of this large unsigned
// integer. The starting state is -1.
static int ParseCharacter(int current, LargeUInt* this, int* state) {
  if (current == '#') {
    *state = -6;  // skip comment state
    return 1;
  }

  if (*state == -6) {
    // Comments end with a newline character.
    if (current == '\n') {
      *state = -1;
    }
    return 1;
  }

  if (*state == -5) {  // Looking for the _ between num_bytes and byte values.
    if (current == '_') {
      *state = 0;  // Start looking for a byte's value.
    }
    return 1;
  }

  int current_value = HexCharToNibble(current);
  if (current_value == -1) {
    // Skip over non hex characters.
    return 1;
  }

  switch (*state) {
    case -1:  // Looking for char 1 of 4 in num_bytes.
      this->num_bytes_ = current_value << 4;
      *state = -2;
      return 1;
    case -2:  // Looking for char 2 of 4 in num_bytes.
      this->num_bytes_ += current_value;
      *state = -3;
      return 1;
    case -3:  // Looking for char 3 of 4 in num_bytes.
      this->num_bytes_ += current_value << 12;
      *state = -4;
      return 1;
    case -4:  // Looking for char 4 of 4 in num_bytes.
      this->num_bytes_ += current_value << 8;
      *state = -5;  // Reached the end of num_bytes.
      return 1;
  }

  if (*state % 2 == 0) {
    this->bytes_[*state / 2] = current_value << 4;
    (*state)++;  // Has read the upper nibble of the byte.
  } else {
    this->bytes_[(*state - 1) / 2] += current_value;
    (*state)++;
    if ((*state / 2) >= this->num_bytes_) {
      return 0;
    } 
  }
  return 1;
}

void LargeUIntRead(FILE* in, LargeUInt* this) {
  if (in == NULL) {
    ErrorOut("Invalid file, unable to read LargeUInt.");
  }

  int current = fgetc(in);
  int state = -1;  // start state
  this->num_bytes_ = 0; 

  while (1) {
    if (feof(in)) {
      break;
    }
    
    if (ParseCharacter(current, this, &state) == 0) {
      return;
    }

    current = fgetc(in);
  }
}

int LargeUIntBufferSize(const LargeUInt* this) {
  return 5 + 2 * this->num_bytes_;
}

void LargeUIntStore(const LargeUInt* this, int buffer_size, char* buffer) {
  if (LargeUIntBufferSize(this) > buffer_size) {
    ErrorOut("Insufficient space for value in the provided buffer.");
  }

  buffer[0] = kHexBytes[this->num_bytes_ >> 4 & 0x0F];
  buffer[1] = kHexBytes[this->num_bytes_ & 0x0F];
  buffer[2] = kHexBytes[this->num_bytes_ >> 12 & 0x0F];
  buffer[3] = kHexBytes[this->num_bytes_ >> 8 & 0x0F];
  buffer[4] = '_';

  int i = 0;
  int j = 5;
  for (; i < this->num_bytes_; i++) {
    buffer[j] = kHexBytes[this->bytes_[i] >> 4 & 0x0F];
    j++;
    buffer[j] = kHexBytes[this->bytes_[i] & 0x0F];
    j++;
  }
  buffer[j] = '\0';
}

void LargeUIntBase10Store(
    const LargeUInt* this, int buffer_size, char* buffer) {
  char internal_buffer[BASE_10_LARGE_U_INT_BUFFER_SIZE];
  int num_digits = 0;
  LargeUInt reduced_this;
  LargeUInt quotient;
  LargeUInt remainder;
  LargeUInt ten;
  ten.num_bytes_ = 1;
  ten.bytes_[0] = 10;

  LargeUIntDivide(this, &ten, &quotient, &remainder);
  while (remainder.num_bytes_ > 0 || quotient.num_bytes_ > 0) {
    if (remainder.num_bytes_ == 0) {
      internal_buffer[num_digits] = 0;
    } else {
      internal_buffer[num_digits] = remainder.bytes_[0];
    }
    LargeUIntClone(&quotient, &reduced_this);
    num_digits++;
    LargeUIntDivide(&reduced_this, &ten, &quotient, &remainder);
  }

  if (num_digits > buffer_size - 1) {
    ErrorOut("Insufficient space in buffer to store base ten string.");
  }

  int i;
  for (i = 0; i < num_digits; i++) {
    buffer[i] = '0' + internal_buffer[num_digits - i - 1];
  }
  buffer[i] = '\0';
}

void LargeUIntLoad(int buffer_size, char* buffer, LargeUInt* this) {
  if (buffer == NULL) {
    ErrorOut("Invalid input buffer, unable to load LargeUInt.");
  }

  int state = -1;  // start state
  int i;
  int parse_result = 1;
  this->num_bytes_ = 0;
  for (i = 0; i < buffer_size && buffer[i] != '\0' && parse_result; i++) {
    parse_result = ParseCharacter(buffer[i], this, &state);
  }
}

void LargeUIntInit(int starting_size, LargeUInt* this) {
  if (starting_size < 0 || starting_size > MAX_NUM_LARGE_U_INT_BYTES) {
    ErrorOut("Invalis size when initializing a large integer.");
  }
  this->num_bytes_ = starting_size;
}

void LargeUIntGrow(LargeUInt* this) {
  if (this->num_bytes_ == MAX_NUM_LARGE_U_INT_BYTES) {
    ErrorOut("Unable to grow large integer.");
  }
  this->num_bytes_++;
}

void LargeUIntTrim(LargeUInt* this) {
  while (this->num_bytes_ > 0 && this->bytes_[this->num_bytes_ - 1] == 0) {
    this->num_bytes_--;
  }
}

void LargeUIntSetByte(int value, int index, LargeUInt* this) {
  if (index < 0 || index >= this->num_bytes_) {
    ErrorOut("Index out of bounds when setting byte.");
  }
  if (value < 0 || value > 255) {
    ErrorOut("Invalid value when setting byte.");
  }
  this->bytes_[index] = value;
}

int LargeUIntGetByte(int index, const LargeUInt* this) {
  if (index < 0 || index >= this->num_bytes_) {
    ErrorOut("Index out of bounds when getting byte.");
  }
  return this->bytes_[index];
}

int LargeUIntNumBytes(const LargeUInt* this) {
  return this->num_bytes_;
}

int LargeUIntCompare(const LargeUInt* this, const LargeUInt* that) {
  if (this->num_bytes_ > that->num_bytes_) {
    return -1;
  } else if (this->num_bytes_ < that->num_bytes_) {
    return 1;
  } else {
    int i;
    // Start with the most significant byte.
    for (i = this->num_bytes_ - 1; i >= 0; i--) {
      if (this->bytes_[i] > that->bytes_[i]) {
        return -1;
      } else if (this->bytes_[i] < that->bytes_[i]) {
        return 1;
      }
    }
  }
  return 0;
}

int LargeUIntLessThan(const LargeUInt* this, const LargeUInt* that) {
  return LargeUIntCompare(this, that) == 1;
}

int LargeUIntLessThanOrEqual(const LargeUInt* this, const LargeUInt* that) {
  return LargeUIntCompare(this, that) > -1;
}

int LargeUIntEqual(const LargeUInt* this, const LargeUInt* that) {
  return LargeUIntCompare(this, that) == 0;
}

void LargeUIntClone(const LargeUInt* that, LargeUInt* this) {
  this->num_bytes_ = that->num_bytes_;
  memmove(this->bytes_, that->bytes_, this->num_bytes_);
}

void LargeUIntByteShiftInc(LargeUInt* this) {
  if (this->num_bytes_ == MAX_NUM_LARGE_U_INT_BYTES) {
    ErrorOut("Unable to byte shift large integer.");
  }
  int i;
  for (i = this->num_bytes_; i > 0; i--) {
    this->bytes_[i] = this->bytes_[i - 1];
  }
  this->num_bytes_++;
  this->bytes_[0] = 0;
  LargeUIntTrim(this);
}

int LargeUIntByteShiftDec(LargeUInt* this) {
  if (this->num_bytes_ < 1) {
    ErrorOut("Unable to decrease shift an integer of zero.");
  }

  int lowest_byte = this->bytes_[0];
  int i;
  for (i = 0; i < this->num_bytes_ - 1; i++) {
    this->bytes_[i] = this->bytes_[i + 1];
  }
  this->num_bytes_--;
  return lowest_byte;
}

void LargeUIntMultiByteShiftInc(int num_bytes, LargeUInt* this) {
  if (this->num_bytes_ + num_bytes > MAX_NUM_LARGE_U_INT_BYTES) {
    ErrorOut("Unable to byte shift large integer.");
  }
  if (this->num_bytes_ == 0) {
    return;
  }
  int i;
  for (i = this->num_bytes_ - 1; i >= 0; i--) {
    this->bytes_[i + num_bytes] = this->bytes_[i];
  }
  this->num_bytes_ += num_bytes;
  memset(this->bytes_, 0, num_bytes);
}

void LargeUIntMultiByteShiftDec(int num_bytes, LargeUInt* this) {
  if (this->num_bytes_ - num_bytes < 0) {
    ErrorOut("Unable to decrease shift an integer by this many bytes.");
  }
  this->num_bytes_ -= num_bytes;
  int i;
  for (i = 0; i < this->num_bytes_; i++) {
    this->bytes_[i] = this->bytes_[i + num_bytes];
  }
}

void LargeUIntAdd(const LargeUInt* that, LargeUInt* this) {
  int carry = 0;
  int value = 0;
  for (int i = 0; i < this->num_bytes_ || i < that->num_bytes_; i++) {
    if (i >= this->num_bytes_) {
      value = that->bytes_[i] + carry;
      LargeUIntGrow(this);
    } else if (i < that->num_bytes_) {
      value = this->bytes_[i] + that->bytes_[i] + carry;
    } else if (carry > 0) {
      value = this->bytes_[i] + carry;
    } else {
      value = this->bytes_[i];
    }
    if (value > 255) {
      value -= 256;
      carry = 1;
    } else {
      carry = 0;
    }
    this->bytes_[i] = value;
    value = 0;
  }
  if (carry > 0) {
    LargeUIntGrow(this);
    this->bytes_[this->num_bytes_ - 1] = carry;
  }
}

void LargeUIntAddByte(int byte, LargeUInt* this) {
  if (byte < 0 || byte > 255) {
    ErrorOut("Byte value in addition should be between 0 and 255.");
  }
  int carry = byte;
  int value = 0;
  for (int i = 0; i < this->num_bytes_ && carry > 0; i++) {
    value = this->bytes_[i] + carry;
    if (value > 255) {
      value -= 256;
      carry = 1;
    } else {
      carry = 0;
    }
    this->bytes_[i] = value;
  }
  if (carry == 1) {
    LargeUIntGrow(this);
    this->bytes_[this->num_bytes_ - 1] = carry;
  }
}

void LargeUIntIncrement(LargeUInt* this) {
  LargeUIntAddByte(1, this);
}

void LargeUIntSub(const LargeUInt* that, LargeUInt* this) {
  int diff = LargeUIntCompare(that, this);
  if (diff == 0) {
    LargeUIntInit(0, this);
    return;
  } else if (diff < 0) {
    ErrorOut("Subtraction would have caused a negative value.");
  }

  // We now know that this is larger than that.
  int borrow = 0;
  int value = 0;
  for (int i = 0; i < this->num_bytes_; i++) {
    if (i < that->num_bytes_) {
      value = this->bytes_[i] - that->bytes_[i] - borrow;
    } else {
      value = this->bytes_[i] - borrow;
    }
    if (value < 0) {
      value += 256;
      borrow = 1;
    } else {
      borrow = 0;
    }
    this->bytes_[i] = value;
  }
  while (this->num_bytes_ > 0 && this->bytes_[this->num_bytes_ - 1] == 0) {
    this->num_bytes_--;
  }
}

void LargeUIntDecrement(LargeUInt* this) {
  while (this->num_bytes_ > 0 && this->bytes_[this->num_bytes_ - 1] == 0) {
    this->num_bytes_--;
  }
  if (this->num_bytes_ == 0) {
    ErrorOut("Unable to decrement an integer of value 0.");
  }

  int borrow = 1;
  int value = 0;
  for (int i = 0; i < this->num_bytes_ && borrow == 1; i++) {
    value = this->bytes_[i] - borrow;
    if (value < 0) {
      value += 256;
      borrow = 1;
    } else {
      borrow = 0;
    }
    this->bytes_[i] = value;
  }

  while (this->num_bytes_ > 0 && this->bytes_[this->num_bytes_ - 1] == 0) {
    this->num_bytes_--;
  }
}

void LargeUIntMultiply(const LargeUInt* that, LargeUInt* this) {
  // Multiply by zero sets this to zero.
  if (that->num_bytes_ == 0) {
    this->num_bytes_ = 0;
  }

  // Multiplied based on repeated additions. Inefficient, but easy to verify.
  LargeUInt value;
  LargeUIntClone(this, &value);

  LargeUInt additions;
  LargeUIntClone(that, &additions);
  // Decrement since this is already value multiplied by one.
  LargeUIntDecrement(&additions);

  while(additions.num_bytes_ > 0) {
    LargeUIntDecrement(&additions);
    LargeUIntAdd(&value, this);
  }
}

void LargeUIntDivide(const LargeUInt* numerator, const LargeUInt* denominator,
                     LargeUInt* quotient, LargeUInt* remainder) {
  quotient->num_bytes_ = 0;
  if (numerator->num_bytes_ == 0) {
    remainder->num_bytes_ = 0;
    return;
  }

  // The remainder starts out with the full value of the numerator.
  LargeUIntClone(numerator, remainder);

  LargeUInt repeated_divisor;
  LargeUIntClone(denominator, &repeated_divisor);

  LargeUInt quotient_segment;
  int divisor_shifts;
  int i;

  while (LargeUIntLessThanOrEqual(denominator, remainder)) {
    divisor_shifts = 0;
    // We byte shift the divisor until it has the same number of bytes as
    // the amount that we still have left from the numerator.
    divisor_shifts = remainder->num_bytes_ - repeated_divisor.num_bytes_;
    LargeUIntMultiByteShiftInc(divisor_shifts, &repeated_divisor);
    // If using the same number of bytes means that we have multiplied the
    // divisor too many times, backtrack on one of the shifts.
    if (LargeUIntLessThan(remainder, &repeated_divisor)) {
      divisor_shifts--;
      LargeUIntByteShiftDec(&repeated_divisor);
    }
    // We translate the number of byte shifts performed into the number that
    // we have multiplied the divisor by, to store in the quotient.
    quotient_segment.num_bytes_ = 1;
    quotient_segment.bytes_[0] = 0;
    // Subtract the multiplied divisor into what is left of the numerator as
    // many times as is possible while having a remainder left over.
    while (LargeUIntLessThanOrEqual(&repeated_divisor, remainder)) {
      LargeUIntSub(&repeated_divisor, remainder);
      LargeUIntIncrement(&quotient_segment);
    }
    // Reset the divisor to determine the next round of multiplications that
    // can fit in the remainder left from this round.
    LargeUIntClone(denominator, &repeated_divisor);

    // Add the number of subtractions at the correct byte in the quotient.
    LargeUIntMultiByteShiftInc(divisor_shifts, &quotient_segment);
    LargeUIntAdd(&quotient_segment, quotient);
  }

  LargeUIntTrim(quotient);
  LargeUIntTrim(remainder);
}

void LargeUIntMod(const LargeUInt* numerator, const LargeUInt* divisor,
                  LargeUInt* remainder) {
  if (numerator->num_bytes_ == 0) {
    remainder->num_bytes_ = 0;
    return;
  }

  LargeUIntClone(numerator, remainder);

  LargeUInt repeated_divisor;
  LargeUIntClone(divisor, &repeated_divisor);
  while (LargeUIntLessThanOrEqual(divisor, remainder)) {
    int bytes_diff = remainder->num_bytes_ - repeated_divisor.num_bytes_ - 1;
    if (bytes_diff >= 1) {
      LargeUIntMultiByteShiftInc(bytes_diff, &repeated_divisor);
    }
    while (LargeUIntLessThanOrEqual(&repeated_divisor, remainder)) {
      LargeUIntSub(&repeated_divisor, remainder);
    }
    LargeUIntClone(divisor, &repeated_divisor);
  }

  LargeUIntTrim(remainder);
}

void LargeUIntApproximateSquareRoot(const LargeUInt* this, LargeUInt* root) {
  LargeUInt two;
  two.num_bytes_ = 1;
  two.bytes_[0] = 2;

  LargeUInt remainder;
  LargeUInt estimate;
  // Start by using an estimate of just above half of the input.
  LargeUIntDivide(this, &two, &estimate, &remainder);
  LargeUIntIncrement(&estimate);

  LargeUInt quotient;
  LargeUInt next_estimate;
  next_estimate.num_bytes_ = 0;

  int divided_evenly = 0;
  int comparison;
  while (1) {
    LargeUIntDivide(this, &estimate, &quotient, &remainder);
    if (remainder.num_bytes_ == 0) {
      divided_evenly = 1;
    } else {
      divided_evenly = 0;
    }
    LargeUIntAdd(&estimate, &quotient);
    LargeUIntDivide(&quotient, &two, &next_estimate, &remainder);

    comparison = LargeUIntCompare(&next_estimate, &estimate);
    if (comparison < 1) {
      if (comparison < 0) {
        divided_evenly = 0;
      }
      break;
    }

    LargeUIntClone(&next_estimate, &estimate);
  }

  LargeUIntClone(&estimate, root);
  if (divided_evenly == 0) {
    LargeUIntIncrement(root);
  }
}
