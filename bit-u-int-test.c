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

void CheckBitUInt(char* expected, BitUInt* this, char* message) {
  char buffer[100];
  BitUIntStore(this, 100, buffer);
  Check(0 == strncmp(expected, buffer, 100), message);
}

void TestLoadAndStore() {
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

  example = "010000";
  BitUIntLoad(strlen(example), example, &a);
  Check(a.bits[0] == 0, "load and trim bit 0 should be 0");
  Check(a.bits[1] == 1, "load and trim bit 1 should be 1");
  Check(a.num_bits == 2, "loaded num bits should be 2");

  char buffer[30];
  int buffer_size = 30;
  BitUIntStore(&a, buffer_size, buffer);
  Check(0 == strncmp(buffer, "01", buffer_size),
        "String buffer should contain 01");
}

void TestClone() {
  BitUInt a;
  BitUInt b;

  char* example = "11101";
  BitUIntLoad(strlen(example), example, &a);
  BitUIntClone(&a, &b);
  Check(b.bits[0] == 1, "clone bit 0 should be 1");
  Check(b.bits[1] == 1, "clone bit 1 should be 1");
  Check(b.bits[2] == 1, "clone bit 2 should be 1");
  Check(b.bits[3] == 0, "clone bit 3 should be 0");
  Check(b.bits[4] == 1, "clone bit 4 should be 1");
  Check(b.num_bits == 5, "cloned num bits should be 5");
}

void TestDoubleAndHalve() {
  BitUInt a;
  char* example = "11101";
  BitUIntLoad(strlen(example), example, &a);
  BitUIntDouble(&a);
  Check(a.num_bits == 6, "doubled num bits should be 6");
  CheckBitUInt("011101", &a, "doubled bits should be 011101");

  int dropped = BitUIntHalve(&a);
  Check(0 == dropped, "first halve should yield a 0");
  CheckBitUInt("11101", &a, "halved bits should be back to 11101");
  dropped = BitUIntHalve(&a);
  CheckBitUInt("1101", &a, "halved bits should be 1101");
  Check(1 == dropped, "second halve should yield a 1");
}

void TestAdd() {
  BitUInt a, b;
  char* a_str = "001";
  char* b_str = "11";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntAdd(&b, &a);
  CheckBitUInt("111", &a, "added a should be 7");
  CheckBitUInt("11", &b, "b should be unchanged");

  CheckBitUInt("111", &a, "added a should be 7");
  BitUIntAdd(&b, &a);
  CheckBitUInt("0101", &a, "added a should be 10");

  a_str = "001";
  b_str = "01101"; 
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntAdd(&b, &a);
  CheckBitUInt("01011", &a, "added a should be 26");
  
  a_str = "1";
  b_str = "1";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntAdd(&b, &a);
  CheckBitUInt("01", &a, "added a should be 2");
}

int main() {
  TestLoadAndStore();
  TestClone();
  TestDoubleAndHalve();
  TestAdd();
  printf("All tests passed\n");
}
