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

  example = "101";
  BitUIntLoad(strlen(example), example, &a);
  BitUIntDouble(&a);
  CheckBitUInt("0101", &a, "doubled bits should be 0101");
}

void TestShiftIncAndDec() {
  BitUInt a;
  char* example = "11101";
  BitUIntLoad(strlen(example), example, &a);
  BitUIntShiftInc(2, &a);
  CheckBitUInt("0011101", &a, "shifted bits should be 0011101");
  
  BitUIntLoad(strlen(example), example, &a);
  BitUIntShiftDec(2, &a);
  CheckBitUInt("101", &a, "shifted bits should be 101");

  a.num_bits = 0;
  BitUIntShiftInc(1, &a);
  Check(0 == a.num_bits, "shifting 0 should still be 0");
}

void TestAdd() {
  BitUInt a, b;
  char* a_str = "001";
  char* b_str = "11";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntAdd(&b, &a);
  CheckBitUInt("111", &a, "added a should be 7");
  CheckBitUInt("11", &b, "b should be unchanged after addition");

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

void TestSub() {
  BitUInt a, b;
  char* a_str = "001";
  char* b_str = "11";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntSub(&b, &a);
  CheckBitUInt("1", &a, "subtracted a should be 1");
  CheckBitUInt("11", &b, "b should be unchanged after subtraction");

  a_str = "1";
  b_str = "1";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntSub(&b, &a);
  Check(0 == a.num_bits, "sub(1, 1) should be 0");
}

void TestIncDec() {
  BitUInt a;
  a.num_bits = 0;
  BitUIntInc(&a);
  CheckBitUInt("1", &a, "incremented 0 should be 1");

  BitUIntInc(&a);
  CheckBitUInt("01", &a, "incremented 1 should be 2");

  BitUIntInc(&a);
  CheckBitUInt("11", &a, "incremented 2 should be 3");

  BitUIntDec(&a);
  CheckBitUInt("01", &a, "decremented 3 should be 2");

  BitUIntDec(&a);
  CheckBitUInt("1", &a, "decremented 2 should be 1");

  BitUIntDec(&a);
  Check(0 == a.num_bits, "decremented 1 should be 0");
}

void TestCompare() {
  BitUInt a, b;
  char* a_str = "101";
  char* b_str = "11";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  Check(-1 == BitUIntCompare(&a, &b),
        "A 3 bit int should be larger than a 2 bit int");
  Check(1 == BitUIntCompare(&b, &a),
        "A 2 bit int should be smaller than a 3 bit int");
  Check(0 == BitUIntCompare(&a, &a), "An int should be equal to itself");

  b_str = "011";
  BitUIntLoad(strlen(b_str), b_str, &b);
  Check(1 == BitUIntLessThan(&a, &b), "101 should be less than 011");
  Check(1 == BitUIntCompare(&a, &b), "011 should be greater than 101");
  Check(0 == BitUIntLessThan(&b, &a), "011 should not be less than 101");
  Check(-1 == BitUIntCompare(&b, &a), "Expect -1 for comparing 011 to 101");

  b_str = "001";
  BitUIntLoad(strlen(b_str), b_str, &b);
  Check(0 == BitUIntLessThan(&a, &b), "101 should be not be less than 001");
  Check(-1 == BitUIntCompare(&a, &b), "Expect -1 for comparing 001 to 101");
  Check(1 == BitUIntLessThan(&b, &a), "001 should be less than 101");
  Check(1 == BitUIntCompare(&b, &a), "Expect 1 for comparing 001 to 101");

  Check(0 == BitUIntLessThan(&a, &a), "101 should not be less than itself");
  Check(1 == BitUIntEqual(&a, &a), "101 should be equal to itself");
  Check(1 == BitUIntLessThanOrEqual(&a, &a),
        "101 self less than or equal is 1");
  Check(0 == BitUIntCompare(&a, &a), "101 self compare is 0");
}

void TestMultiply() {
  BitUInt a, b;
  char* a_str = "101";
  char* b_str = "1";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);

  BitUIntMul(&b, &a);
  CheckBitUInt("101", &a, "5 times 1 should be 5");

  b_str = "01";
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntMul(&b, &a);
  CheckBitUInt("0101", &a, "5 times 2 should be 10");

  a_str = "101";
  b_str = "11";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntMul(&b, &a);
  CheckBitUInt("1111", &a, "5 times 3 should be 15");

  a_str = "1011101";
  b_str = "11001";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntLoad(strlen(b_str), b_str, &b);
  BitUIntMul(&b, &a);
  CheckBitUInt("11100111011", &a, "93 times 19 should be 1767");
}

void TestDivide() {
  BitUInt n, d, q, r;
 
  char* n_str = "101";
  char* d_str = "1";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntLoad(strlen(d_str), d_str, &d);
  BitUIntDiv(&n, &d, &q, &r);
  CheckBitUInt("101", &q, "5 divided by 1 should be 5");
  Check(0 == r.num_bits, "1 should divide evenly into 5");

  d_str = "01";
  BitUIntLoad(strlen(d_str), d_str, &d);
  BitUIntDiv(&n, &d, &q, &r);
  CheckBitUInt("01", &q, "5 divided by 2 should be 2");
  CheckBitUInt("1", &r, "5 divided by 2 should have a remainder of 1");

  d_str = "11";
  BitUIntLoad(strlen(d_str), d_str, &d);
  BitUIntDiv(&n, &d, &q, &r);
  CheckBitUInt("1", &q, "5 divided by 3 should be 1");
  CheckBitUInt("01", &r, "5 divided by 3 should have a remainder of 2");

  d_str = "011";
  BitUIntLoad(strlen(d_str), d_str, &d);
  BitUIntDiv(&n, &d, &q, &r);
  Check(0 == q.num_bits, "5 divided by 6 should be 0");
  CheckBitUInt("101", &r, "5 divided by 6 should have a remainder of 5");

  d_str = "101";
  BitUIntLoad(strlen(d_str), d_str, &d);
  BitUIntDiv(&n, &d, &q, &r);
  CheckBitUInt("1", &q, "5 divided by 5 should be 1");
  Check(0 == r.num_bits, "5 divided by 5 should have a remainder of 0");

  n_str = "10001111011";
  d_str = "1011101";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntLoad(strlen(d_str), d_str, &d);
  BitUIntDiv(&n, &d, &q, &r);
  CheckBitUInt("11001", &q, "1777 divided by 93 should be 19");
  CheckBitUInt("0101", &r, "1777 divided by 93 should have a remainder of 10");
}

void TestBase10Store() {
  char dec_str[9];
  BitUInt a;
  char* a_str = "0011";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntBase10Store(&a, 9, dec_str);
  Check(0 == strncmp(dec_str, "12", 9), "0011 should be 12 in base 10");

  a_str = "01010010110001";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntBase10Store(&a, 9, dec_str);
  Check(0 == strncmp(dec_str, "9034", 9),
        "01010010110001 should be 9034 in base 10");

  a_str = "11001001011101";
  BitUIntLoad(strlen(a_str), a_str, &a);
  BitUIntBase10Store(&a, 9, dec_str);
  Check(0 == strncmp(dec_str, "11923", 9),
        "11001001011101 should be 11923 in base 10");
}

void TestApproximateSquareRoot() {
  BitUInt n, root;
  char* n_str;
  
  n_str = "001";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("01", &root, "Approximate root of 4 should be 2");

  n_str = "1001";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("11", &root, "Approximate root of 9 should be 3");

  n_str = "0010011";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("0101", &root, "Approximate root of 100 should be 10");

  n_str = "1100011";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("0101", &root, "Approximate root of 99 should be 10");

  n_str = "000111011101";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("111011", &root, "Approximate root of 3,000 should be 55");
  
  // In base 10: 43985512 squared is 1934725265902144
  // In base 16: 0x29F2A68 squared is 0x6DF9F54364A40
  n_str = "000000100101001001101100001010101111100111111011011";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("00010110010101001111100101", &root,
               "Root of 1,934,725,265,902,144 should be 43,985,512");

  // Add 1 to the previous square, should bump up to the next integer.
  n_str = "100000100101001001101100001010101111100111111011011";
  BitUIntLoad(strlen(n_str), n_str, &n);
  BitUIntApproximateSquareRoot(&n, &root);
  CheckBitUInt("10010110010101001111100101", &root,
               "Root of 1,934,725,265,902,145 should be 43,985,513");
}


int main() {
  TestLoadAndStore();
  TestClone();
  TestDoubleAndHalve();
  TestShiftIncAndDec();
  TestAdd();
  TestSub();
  TestIncDec();
  TestCompare();
  TestMultiply();
  TestDivide();
  TestBase10Store();
  TestApproximateSquareRoot();
  printf("All tests passed\n");
}
