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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Check(int condition, char* message) {
  if (!condition) {
    fprintf(stderr, "Condition failed: %s\n", message);
    exit(1);
  }
}

void TestGetSetAndNumBytes() {
  LargeUInt num;
  LargeUIntInit(3, &num);
  Check(3 == LargeUIntNumBytes(&num), "Num should be 3 bytes long.");
  LargeUIntSetByte(255, 0, &num);
  LargeUIntSetByte(1, 1, &num);
  LargeUIntSetByte(76, 2, &num);
  Check(255 == LargeUIntGetByte(0, &num), "Num byte 0 should be 255");
  Check(1 == LargeUIntGetByte(1, &num), "Num byte 0 should be 1");
  Check(76 == LargeUIntGetByte(2, &num), "Num byte 0 should be 76");
}

void TestLoadAndStore() {
  char a_str[30];
  LargeUInt a_int;
  LargeUIntInit(2, &a_int);
  LargeUIntSetByte(12, 0, &a_int); 
  LargeUIntSetByte(99, 1, &a_int);
  LargeUIntStore(&a_int, 30, a_str);
  Check(0 == strncmp(a_str, "0200_0C63", 10),
        "String buffer should contain 0200_0C63");
}

int main(void) {
  TestGetSetAndNumBytes();
  TestLoadAndStore();
  printf("All tests passed\n");
}
