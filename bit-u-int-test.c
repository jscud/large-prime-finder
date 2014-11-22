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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Check(int condition, char* message) {
  if (!condition) {
    fprintf(stderr, "Condition failed: %s\n", message);
    exit(1);
  }
}

void TestLoad() {
  BitUInt a;

  char* example = "0101";
  BitUIntLoad(strlen(example), example, &a);
  Check(a.bits[0] == 0, "loaded bit 0 should be 0");
  Check(a.bits[1] == 1, "loaded bit 1 should be 1");
  Check(a.bits[2] == 0, "loaded bit 2 should be 0");
  Check(a.bits[3] == 1, "loaded bit 3 should be 1");
  Check(a.num_bits == 4, "loaded num bits should be 4");

  example = "10121001";
  BitUIntLoad(strlen(example), example, &a);
  Check(a.bits[0] == 1, "loaded bit 0 should be 1");
  Check(a.bits[1] == 0, "loaded bit 1 should be 0");
  Check(a.bits[3] == 1, "loaded bit 2 should be 1");
  Check(a.num_bits == 3, "loaded num bits should be 3");
}

int main() {
  TestLoad();
  printf("All tests passed\n");
}
