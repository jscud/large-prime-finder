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

#ifndef LARGE_U_INT_H
#define LARGE_U_INT_H

#include <stdio.h>
#include <stdint.h>

#define MAX_NUM_LARGE_U_INT_BYTES 30

typedef struct {
  int num_bytes_;
  uint8_t bytes_[MAX_NUM_LARGE_U_INT_BYTES];
} LargeUInt;

// The human readable format for large ints is in the following form: The
// number of bytes is listed first in hexidecimal using 2 bytes in little
// endian order. So for example 0A00 means that the number's value fits in 10
// bytes. The number of bytes is followed by an underscore as a seperator,
// then the bytes in the number's value are listed in hexidecimal in little
// endian order. As an example, the number 0x3D4A50 (4,016,720 in base 10)
// would be represented as 0300_054A3D.
void LargeUIntPrint(const LargeUInt* this, FILE* out);

// Reads the next available LargeUInt from the file and stores the value in
// the provided LargeUInt.
void LargeUIntRead(FILE* in, LargeUInt* this);

#endif
