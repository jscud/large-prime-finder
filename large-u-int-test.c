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

void CheckLargeUInt(char* expected, LargeUInt* this, char* message) {
  char buffer[65];
  LargeUIntStore(this, 65, buffer);
  Check(0 == strncmp(expected, buffer, 65), message);
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

  char* example = "0300_BC0007";
  LargeUIntLoad(strlen(example), example, &a_int);
  Check(3 == LargeUIntNumBytes(&a_int), "Num bytes from loaded should be 3");
  Check(188 == LargeUIntGetByte(0, &a_int), "First loaded byte should be 188");
  Check(0 == LargeUIntGetByte(1, &a_int), "Second loaded byte should be 0");
  Check(7 == LargeUIntGetByte(2, &a_int), "Third loaded byte should be 7");
  CheckLargeUInt(example, &a_int, "String buffer should match example");
}

void TestGrowAndTrim() {
  LargeUInt num;
  char* numstr = "0300_000001";
  LargeUIntLoad(strlen(numstr), numstr, &num);
  Check(3 == LargeUIntNumBytes(&num), "Initially should have 3 bytes");
  LargeUIntGrow(&num);
  LargeUIntSetByte(0, 3, &num);
  Check(4 == LargeUIntNumBytes(&num), "Should have 4 bytes after growing");
  LargeUIntTrim(&num);
  Check(3 == LargeUIntNumBytes(&num), "Back down to 3 bytes after trimming");
  LargeUIntSetByte(0, 2, &num);
  LargeUIntTrim(&num);
  Check(0 == LargeUIntNumBytes(&num), "Down to 0 bytes after trimming");
}

void TestCompare() {
  LargeUInt a, b;
  LargeUIntLoad(11, "0300_431232", &a);
  LargeUIntLoad(9, "0200_4312", &b);
  Check(-1 == LargeUIntCompare(&a, &b),
        "A 3 byte int should be larger than a 2 byte int");
  Check(1 == LargeUIntCompare(&b, &a),
        "A 2 byte int should be smaller than a 3 byte int");
  Check(0 == LargeUIntCompare(&a, &a), "An int should be equal to itself");

  LargeUIntLoad(13, "0400_00001101", &b);
  Check(1 == LargeUIntCompare(&a, &b),
        "A 3 byte int should be smaller than a 4 byte int");
  Check(-1 == LargeUIntCompare(&b, &a),
        "A 4 byte int should be larger than a 3 byte int");

  LargeUIntLoad(11, "0300_431132", &b);
  Check(-1 == LargeUIntCompare(&a, &b),
        "0x321243 should be greater than 0x321143");

  LargeUIntLoad(11, "0300_431232", &b);
  Check(0 == LargeUIntCompare(&a, &b), "0x321243 should equal 0x321243");
}

void TestClone() {
  LargeUInt a, b;
  LargeUIntLoad(11, "0300_AABBCC", &a);
  LargeUIntClone(&a, &b);
  Check(0 == LargeUIntCompare(&a, &b), "Cloned int should equal original");
}

void TestAddAndIncrement() {
  LargeUInt a, b;
  LargeUIntLoad(11, "0300_FFFFFF", &a);
  LargeUIntLoad(7, "0100_02", &b);
  LargeUIntAdd(&b, &a);
  CheckLargeUInt("0100_02", &b, "After add, b should be unchanged");
  CheckLargeUInt("0400_01000001", &a, "Add should carry to grow a");

  LargeUIntLoad(7, "0100_BB", &a);
  LargeUIntLoad(11, "0300_010099", &b);
  LargeUIntAdd(&b, &a);
  CheckLargeUInt("0300_BC0099", &a, "Add should expand a to the size of b");

  LargeUIntLoad(11, "0300_FFFFFF", &a);
  LargeUIntIncrement(&a);
  CheckLargeUInt("0400_00000001", &a, "Increment should carry to grow a");
}

void TestSub() {
  LargeUInt a, b;
  LargeUIntLoad(11, "0300_00000F", &a);
  LargeUIntLoad(7, "0100_03", &b);
  LargeUIntSub(&b, &a); // 983040 - 3 = 983037
  CheckLargeUInt("0300_FDFF0E", &a, "Difference should be 983037");
}

void TestDivide() {
  LargeUInt n, d, q, r;
  LargeUIntLoad(7, "0100_0F", &n);
  LargeUIntLoad(7, "0100_05", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0100_03", &q, "Quotient should be 3");
  CheckLargeUInt("0000_", &r, "Remainder should be 0");
}

int main(void) {
  TestGetSetAndNumBytes();
  TestLoadAndStore();
  TestGrowAndTrim();
  TestCompare();
  TestClone();
  TestAddAndIncrement();
  TestSub();
  TestDivide();
  printf("All tests passed\n");
}
