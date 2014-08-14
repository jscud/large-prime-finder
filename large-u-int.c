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

const int kMaxNumLargeUIntBytes = 30;

static const char kHexBytes[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

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

