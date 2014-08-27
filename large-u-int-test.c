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

  LargeUIntLoad(13, "0400_1F055ED0", &a);
  LargeUIntLoad(13, "0400_49531D1C", &b);
  Check(-1 == LargeUIntCompare(&a, &b),
        "0xD0... should be greater than 0x1C...");
}

void TestClone() {
  LargeUInt a, b;
  LargeUIntLoad(11, "0300_AABBCC", &a);
  LargeUIntClone(&a, &b);
  Check(0 == LargeUIntCompare(&a, &b), "Cloned int should equal original");
}

void TestShift() {
  LargeUInt a;
  LargeUIntLoad(11, "0300_AABBCC", &a);
  LargeUIntByteShift(&a);
  CheckLargeUInt("0400_00AABBCC", &a, "Shift should add low order zero");

  LargeUIntLoad(5, "0000_", &a);
  LargeUIntByteShift(&a);
  CheckLargeUInt("0000_", &a, "Shifting zero should produce zero");
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

void TestSubAndDecrement() {
  LargeUInt a, b;
  LargeUIntLoad(11, "0300_00000F", &a);
  LargeUIntLoad(7, "0100_03", &b);
  LargeUIntSub(&b, &a); // 983040 - 3 = 983037
  CheckLargeUInt("0300_FDFF0E", &a, "Difference should be 983037");

  // In base 10: 85632148 - 5298632 = 80333516
  // In base 16: 0x51AA494 - 0x50D9C8 = 0x4C9CACC
  LargeUIntLoad(13, "0400_94A41A05", &a);
  LargeUIntLoad(11, "0300_C8D950", &b);
  LargeUIntSub(&b, &a);
  CheckLargeUInt("0400_CCCAC904", &a, "Difference should be 80333516");

  LargeUIntLoad(11, "0300_00000F", &a);
  LargeUIntDecrement(&a);
  CheckLargeUInt("0300_FFFF0E", &a, "After decrement should be 983039");

  LargeUIntLoad(7, "0100_01", &a);
  LargeUIntDecrement(&a);
  CheckLargeUInt("0000_", &a, "After decrement should be 0");
}

void TestMultiply() {
  LargeUInt a, b;
  LargeUIntLoad(7, "0100_05", &a);
  LargeUIntLoad(7, "0100_03", &b);
  LargeUIntMultiply(&b, &a);
  CheckLargeUInt("0100_0F", &a, "Result should be 15");

  // In base 10: 85632148 * 5298632 = 453733239621536
  // In base 16: 0x51AA494 * 0x50D9C8 = 0x19CAB009207A0
  LargeUIntLoad(13, "0400_94A41A05", &a);
  LargeUIntLoad(11, "0300_C8D950", &b);
  LargeUIntMultiply(&b, &a);
  CheckLargeUInt("0700_A0079200AB9C01", &a,
                 "Result should be 453,733,239,621,536");
} 

void TestDivide() {
  LargeUInt n, d, q, r;
  LargeUIntLoad(7, "0100_0F", &n);
  LargeUIntLoad(7, "0100_05", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0100_03", &q, "Quotient should be 3");
  CheckLargeUInt("0000_", &r, "Remainder should be 0");

  LargeUIntLoad(7, "0100_15", &n);
  LargeUIntLoad(7, "0100_05", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0100_04", &q, "Quotient should be 4");
  CheckLargeUInt("0100_01", &r, "Remainder should be 1");

  LargeUIntLoad(7, "0100_0E", &n);
  LargeUIntLoad(7, "0100_05", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0100_02", &q, "Quotient should be 2");
  CheckLargeUInt("0100_04", &r, "Remainder should be 4");

  LargeUIntLoad(7, "0100_07", &n);
  LargeUIntLoad(7, "0100_08", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0000_", &q, "Quotient should be 0");
  CheckLargeUInt("0100_07", &r, "Remainder should be 7");

  // In base 10: 15746896 / 3 = 5248965 r 1
  // In base 16: 0xF04750 / 3 = 0x5017C5 r 1
  LargeUIntLoad(11, "0300_5047F0", &n);
  LargeUIntLoad(7, "0100_03", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0300_C51750", &q,
                 "Quotient for three byte number should be 5,248,965");
  CheckLargeUInt("0100_01", &r,
                 "Remainder for three byte number should be 1");

  // In base 10: 2558063199 / 32561 = 78562 r 5917
  // In base 16: 0x9878F25F / 0x7F31 = 0x132E2 r 0x171D
  LargeUIntLoad(13, "0400_5FF27898", &n);
  LargeUIntLoad(9, "0200_317F", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0300_E23201", &q, "Quotient should be 78,562");
  CheckLargeUInt("0200_1D17", &r, "Remainder should be 5,917");

  // In base 10: 981238718624873549 / 471683913 = 2080288709 r 194035232
  // In base 16: 0xD9E0F6A6F7EBC4D / 0x1C1D5349 = 0x7BFEAFC5 r 0xB90BE20
  LargeUIntLoad(21, "0800_4DBC7E6F6A0F9E0D", &n);
  LargeUIntLoad(13, "0400_49531D1C", &d);
  LargeUIntDivide(&n, &d, &q, &r);
  CheckLargeUInt("0400_C5AFFE7B", &q, "Quotient should be 2,080,288,709");
  CheckLargeUInt("0400_20BE900B", &r, "Remainder should be 194,035,232");
}

int main(void) {
  TestGetSetAndNumBytes();
  TestLoadAndStore();
  TestGrowAndTrim();
  TestCompare();
  TestClone();
  TestShift();
  TestAddAndIncrement();
  TestSubAndDecrement();
  TestMultiply();
  TestDivide();
  printf("All tests passed\n");
}
