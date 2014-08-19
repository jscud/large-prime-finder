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

static const char kHexBytes[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Exits the program after sending the message to stderr.
static void ErrorOut(char* message) {
  fprintf(stderr, message);
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

void LargeUIntRead(FILE* in, LargeUInt* this) {
  if (in == NULL) {
    ErrorOut("Invalid file, unable to read LargeUInt.\n");
  }

  int current = fgetc(in);
  int state = 0;  // start state
  int num_bytes = 0;
  int byte_index = 0;
  int current_value = -1;
  int byte_value = 0;
  this->num_bytes_ = 0;
  while (1) {
    if (feof(in)) {
      break;
    }

    if (current == '#') {
      state = 7;  // skip comment state
    }

    if (state == 7) {
      // Comments end with a newline character.
      if (current == '\n') {
        state = 0;
      }
      current = fgetc(in);
      continue;
    }

    if (state == 4) {  // Looking for the _ between num_bytes and byte values.
      if (current == '_') {
        state = 5;  // Start looking for a byte's value.
      }
      current = fgetc(in);
      continue;
    }

    current_value = HexCharToNibble(current);
    if (current_value == -1) {
      // Skip over non hex characters.
      current = fgetc(in);
      continue;
    }

    switch (state) {
      case 0:  // Looking for char 1 of 4 in num_bytes.
        num_bytes = current_value << 4;
        state = 1;
        break;
      case 1:  // Looking for char 2 of 4 in num_bytes.
        num_bytes += current_value;
        state = 2;
        break;
      case 2:  // Looking for char 3 of 4 in num_bytes.
        num_bytes += current_value << 12;
        state = 3;
        break;
      case 3:  // Looking for char 4 of 4 in num_bytes.
        num_bytes += current_value << 8;
        state = 4;  // Reached the end of num_bytes.
        byte_index = 0;
        this->num_bytes_ = num_bytes;
        break;
      case 5:
        this->bytes_[byte_index] = current_value << 4;
        state = 6;  // Has read the upper nibble of the byte.
        break;
      case 6:
        this->bytes_[byte_index] += current_value;
        byte_index++;
        if (byte_index < num_bytes) {
          state = 5;
        } else {
          return;
        }
        break;
      case 7:
        break;
    }
    current = fgetc(in);
  }
}

void LargeUIntInit(int starting_size, LargeUInt* this) {
  if (starting_size < 0 || starting_size > MAX_NUM_LARGE_U_INT_BYTES) {
    ErrorOut("Invalis size when initializing a large integer");
  }
  this->num_bytes_ = starting_size;
}

int LargeUIntBufferSize(const LargeUInt* this) {
  return 5 + 2 * this->num_bytes_;
}

void LargeUIntSetByte(int value, int index, LargeUInt* this) {
  if (index < 0 || index >= this->num_bytes_) {
    ErrorOut("Index out of bounds when setting byte");
  }
  if (value < 0 || value > 255) {
    ErrorOut("Invalid value when setting byte");
  }
  this->bytes_[index] = value;
}

int LargeUIntGetByte(int index, const LargeUInt* this) {
  if (index < 0 || index >= this->num_bytes_) {
    ErrorOut("Index out of bounds when getting byte");
  }
  return this->bytes_[index];
}

int LargeUIntNumBytes(const LargeUInt* this) {
  return this->num_bytes_;
}
