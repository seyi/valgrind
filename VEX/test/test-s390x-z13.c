/*
 * To build:
 * gcc test-s390x-z13.c -march=z13 -Wall -Wextra -pedantic -o test-s390x-z13
 */

/*
 *  s390x z13 instructions test
 *
 *  Copyright (c) 2017 Vadim Barkov
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#define _GNU_SOURCE
#include "assert.h"
#include "math.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "test-s390x-vector.h"
#include <string.h>

/* Constant value for immediate instructions' tests */
#define IMMEDIATE_VALUE 0x0abc
#define IMMEDIATE_VALUE_STR "0x0abc"

/* Useful macros for testing all possible values and printing message after it
 */
#define test_each(type, var, instruction)                                      \
   {                                                                           \
      type##_foreach((var), test_##instruction((var)));                        \
   }

#define test_range(var, from, to, instruction)                                 \
   {                                                                           \
      bool result = true;                                                      \
      __int_foreach(var, from, to, result &= (test_##instruction((var))));     \
      if (result)                                                              \
         printPassed(instruction);                                             \
      else                                                                     \
         printFailed(instruction);                                             \
   }

#define __int_foreach(var, minValue, maxValue, statement)                      \
   {                                                                           \
      for (var = minValue; var < maxValue; var++) {                            \
         statement;                                                            \
      }                                                                        \
                                                                               \
      { statement; }                                                           \
   }

#define uint32_foreach(var, statement)                                         \
   __int_foreach(var, 0, UINT32_MAX, statement)
#define int32_foreach(var, statement)                                          \
   __int_foreach(var, INT32_MIN, INT32_MAX, statement)
#define uint64_foreach(var, statement)                                         \
   __int_foreach(var, 0, UINT64_MAX, statement)

/* load and zero rightmost byte */
static bool test_lzrf(const uint32_t testedValue) {
   uint32_t valueAfterTest = testedValue;

   __asm__ volatile("lzrf %%r6, %0\n"
                    "st %%r6, %0\n"
                    :
                    : "g"(valueAfterTest));

   bool testPassed = (valueAfterTest == (testedValue & 0xffffff00));
   if (!testPassed) {
      fprintf(stderr, "ERROR lzrf : %x -> %x | expected: %x\n", testedValue,
              valueAfterTest, (testedValue & 0xffffff00));
   }

   return testPassed;
}

/* load and zero rightmost byte 64bit*/
static bool test_lzrg(const uint64_t testedValue) {
   uint64_t valueAfterTest = testedValue;

   __asm__ volatile("lzrg %%r6, %0\n"
                    "stg %%r6, %0\n"
                    :
                    : "g"(valueAfterTest));

   bool testPassed = (valueAfterTest == (testedValue & 0xffffffffffffff00UL));
   if (!testPassed) {
      fprintf(stderr, "ERROR lzrg : %lx -> %lx | expected: %lx\n", testedValue,
              valueAfterTest, (testedValue & 0xffffffffffffff00UL));
   }

   return testPassed;
}

/* load logical and zero rightmost byte */
static bool test_llzrgf(const uint32_t testedValue) {
   uint64_t valueAfterTest = 0;
   uint64_t expectedValue;

   __asm__ volatile("llzrgf %%r6, %0\n"
                    "st %%r6, %1\n"
                    :
                    : "g"(testedValue), "g"(valueAfterTest));

   expectedValue = (((uint64_t)testedValue << 32) & 0xffffff0000000000UL);
   bool testPassed = (valueAfterTest == expectedValue);
   if (!testPassed) {
      fprintf(stderr, "ERROR llzrgf : %x -> %lx | expected: %lx\n", testedValue,
              valueAfterTest, expectedValue);
   }

   return testPassed;
}

/* compare instructions */
#define __compare_r "cr"
#define __compare_m "c"

/* load high on condition */
#define declare_load_high_on_condition(TESTED_INSTRUCTION, CONDITION_SYMBOL,   \
                                       ARGUMENT_ASM_TYPE)                      \
   bool test_##TESTED_INSTRUCTION(const int32_t testedValue) {                 \
      bool testPassed;                                                         \
      uint64_t expectedValue;                                                  \
      const uint64_t valueBeforeTest = 0x6666666699999999UL;                   \
      const uint64_t overrideValue = 0xeeeeeeeeeeeeeeeeUL;                     \
      const int32_t invertedValue = testedValue ^ 0xffffffff;                  \
      uint64_t valueAfterTest = valueBeforeTest;                               \
      if (testedValue CONDITION_SYMBOL invertedValue) {                        \
         expectedValue = 0xeeeeeeee99999999UL /* high from overrideValue and   \
                                                 low from valueBeforeTest*/    \
             ;                                                                 \
      } else {                                                                 \
         expectedValue = valueBeforeTest;                                      \
      }                                                                        \
                                                                               \
      __asm__ volatile(                                                        \
          "cr %[testedValue], %[invertedValue]\n" #TESTED_INSTRUCTION          \
          " %[valueAfterTest], %[overrideValue]\n"                             \
          : [valueAfterTest] "=r"(valueAfterTest)                              \
          : [testedValue] "r"(testedValue),                                    \
            [invertedValue] "r"(invertedValue),                                \
            [overrideValue] #ARGUMENT_ASM_TYPE(overrideValue),                 \
            "[valueAfterTest]"(valueAfterTest));                               \
                                                                               \
      testPassed = (valueAfterTest == expectedValue);                          \
      if (!testPassed) {                                                       \
         fprintf(stderr,                                                       \
                 "ERROR " #TESTED_INSTRUCTION ": %x -> %lx | expected: %lx\n", \
                 testedValue, valueAfterTest, expectedValue);                  \
      }                                                                        \
      return testPassed;                                                       \
   }

declare_load_high_on_condition(locfhre, ==, r) declare_load_high_on_condition(
    locfhrne, !=,
    r) declare_load_high_on_condition(locfhrh, >,
                                      r) declare_load_high_on_condition(locfhrl,
                                                                        <, r)

    declare_load_high_on_condition(locfhe, ==, m) declare_load_high_on_condition(
        locfhne, !=,
        m) declare_load_high_on_condition(locfhh, >,
                                          m) declare_load_high_on_condition(locfhl,
                                                                            <,
                                                                            m)

/* store high on condition */
#define declare_store_high_on_condition(TESTED_INSTRUCTION, CONDITION_SYMBOL,  \
                                        ARGUMENT_ASM_TYPE)                     \
   bool test_##TESTED_INSTRUCTION(const int32_t testedValue) {                 \
      bool testPassed;                                                         \
      uint64_t expectedValue;                                                  \
      const uint64_t valueBeforeTest = 0x6666666699999999UL;                   \
      const uint64_t overrideValue = 0xeeeeeeeeeeeeeeeeUL;                     \
      const int32_t invertedValue = testedValue ^ 0xffffffff;                  \
      uint64_t valueAfterTest = valueBeforeTest;                               \
      if (testedValue CONDITION_SYMBOL invertedValue) {                        \
         expectedValue = 0xeeeeeeee99999999UL /* high from overrideValue and   \
                                                 low from valueBeforeTest*/    \
             ;                                                                 \
      } else {                                                                 \
         expectedValue = valueBeforeTest;                                      \
      }                                                                        \
                                                                               \
      __asm__ volatile(                                                        \
          "cr %[testedValue], %[invertedValue]\n" #TESTED_INSTRUCTION          \
          " %[overrideValue], %[valueAfterTest]\n"                             \
          : [valueAfterTest] "=" #ARGUMENT_ASM_TYPE(valueAfterTest)            \
          : [testedValue] "r"(testedValue),                                    \
            [invertedValue] "r"(invertedValue),                                \
            [overrideValue] "r"(overrideValue));                               \
                                                                               \
      testPassed = (valueAfterTest == expectedValue);                          \
      if (!testPassed) {                                                       \
         fprintf(stderr,                                                       \
                 "ERROR " #TESTED_INSTRUCTION ": %x -> %lx | expected: %lx\n", \
                 testedValue, valueAfterTest, expectedValue);                  \
      }                                                                        \
      return testPassed;                                                       \
   }

        declare_store_high_on_condition(stocfhe, ==, m) declare_store_high_on_condition(
            stocfhne, !=,
            m) declare_store_high_on_condition(stocfhh, >,
                                               m) declare_store_high_on_condition(stocfhl,
                                                                                  <
                                                                                  ,
                                                                                  m)

#define __format_uint32_t "%x"
#define __format_uint64_t "%lx"
#define __error_format_uint64_t                                                \
   "ERROR " #TESTED_INSTRUCTION ": %x -> %lx | expected: %lx\n"
#define __halfword_valueBefore_uint32_t 0x66669999
#define __halfword_valueBefore_uint64_t 0x6666666699999999UL
/* load halfword immediate on condition */
#define declare_load_halfword_immediate_on_condition(                          \
    TESTED_INSTRUCTION, ARGUMENT_TYPE, CONDITION_SYMBOL, ARGUMENT_ASM_TYPE)    \
   bool test_##TESTED_INSTRUCTION(const int32_t testedValue) {                 \
      bool testPassed;                                                         \
      ARGUMENT_TYPE expectedValue;                                             \
      const ARGUMENT_TYPE valueBeforeTest =                                    \
          __halfword_valueBefore_##ARGUMENT_TYPE;                              \
      const int32_t invertedValue = testedValue ^ 0xffffffff;                  \
      ARGUMENT_TYPE valueAfterTest = valueBeforeTest;                          \
      if (testedValue CONDITION_SYMBOL invertedValue) {                        \
         expectedValue = IMMEDIATE_VALUE;                                      \
      } else {                                                                 \
         expectedValue = valueBeforeTest;                                      \
      }                                                                        \
                                                                               \
      __asm__ volatile(                                                        \
          "cr %[testedValue], %[invertedValue]\n" #TESTED_INSTRUCTION          \
          " %[valueAfterTest], " IMMEDIATE_VALUE_STR "\n"                      \
          : [valueAfterTest] "=r"(valueAfterTest)                              \
          : [testedValue] "r"(testedValue),                                    \
            [invertedValue] "r"(invertedValue),                                \
            "[valueAfterTest]"(valueAfterTest));                               \
                                                                               \
      testPassed = (valueAfterTest == expectedValue);                          \
      if (!testPassed) {                                                       \
         fprintf(stderr, "ERROR " #TESTED_INSTRUCTION                          \
                         ": %x -> "__format_##ARGUMENT_TYPE                    \
                 " | expected: " __format_##ARGUMENT_TYPE "\n",                \
                 testedValue, valueAfterTest, expectedValue);                  \
      }                                                                        \
      return testPassed;                                                       \
   }

            declare_load_halfword_immediate_on_condition(
                lochie, uint32_t, ==,
                r) declare_load_halfword_immediate_on_condition(lochine,
                                                                uint32_t, !=, r)
                declare_load_halfword_immediate_on_condition(
                    lochih, uint32_t, >,
                    r) declare_load_halfword_immediate_on_condition(lochil,
                                                                    uint32_t, <,
                                                                    r)

                    declare_load_halfword_immediate_on_condition(
                        locghie, uint64_t, ==,
                        r) declare_load_halfword_immediate_on_condition(locghine,
                                                                        uint64_t,
                                                                        !=, r)
                        declare_load_halfword_immediate_on_condition(
                            locghih, uint64_t, >,
                            r) declare_load_halfword_immediate_on_condition(locghil,
                                                                            uint64_t,
                                                                            <,
                                                                            r)

/* load halfword high immediate on condition */
#define declare_load_halfword_high_immediate_on_condition(                     \
    TESTED_INSTRUCTION, CONDITION_SYMBOL, ARGUMENT_ASM_TYPE)                   \
   bool test_##TESTED_INSTRUCTION(const int32_t testedValue) {                 \
      bool testPassed;                                                         \
      uint64_t expectedValue;                                                  \
      const uint64_t valueBeforeTest = __halfword_valueBefore_uint64_t;        \
      const int32_t invertedValue = testedValue ^ 0xffffffff;                  \
      uint64_t valueAfterTest = valueBeforeTest;                               \
      if (testedValue CONDITION_SYMBOL invertedValue) {                        \
         expectedValue = IMMEDIATE_VALUE;                                      \
         expectedValue <<= 32;                                                 \
         expectedValue +=                                                      \
             __halfword_valueBefore_uint64_t & 0x00000000ffffffffUL;           \
      } else {                                                                 \
         expectedValue = valueBeforeTest;                                      \
      }                                                                        \
                                                                               \
      __asm__ volatile(                                                        \
          "cr %[testedValue], %[invertedValue]\n" #TESTED_INSTRUCTION          \
          " %[valueAfterTest], " IMMEDIATE_VALUE_STR "\n"                      \
          : [valueAfterTest] "=r"(valueAfterTest)                              \
          : [testedValue] "r"(testedValue),                                    \
            [invertedValue] "r"(invertedValue),                                \
            "[valueAfterTest]"(valueAfterTest));                               \
                                                                               \
      testPassed = (valueAfterTest == expectedValue);                          \
      if (!testPassed) {                                                       \
         fprintf(stderr,                                                       \
                 "ERROR " #TESTED_INSTRUCTION ": %x -> %lx | expected: %lx\n", \
                 testedValue, valueAfterTest, expectedValue);                  \
      }                                                                        \
      return testPassed;                                                       \
   }

                            declare_load_halfword_high_immediate_on_condition(
                                lochhie, ==, r)
                                declare_load_halfword_high_immediate_on_condition(
                                    lochhine, !=, r)
                                    declare_load_halfword_high_immediate_on_condition(
                                        lochhih, >, r)
                                        declare_load_halfword_high_immediate_on_condition(
                                            lochhil, <, r)

                                            static void test_all_locfh() {
   int32_t signed32bit = 0;

   test_each(int32, signed32bit, locfhe);
   test_each(int32, signed32bit, locfhne);
   test_each(int32, signed32bit, locfhh);
   test_each(int32, signed32bit, locfhl);

   test_each(int32, signed32bit, locfhre);
   test_each(int32, signed32bit, locfhrne);
   test_each(int32, signed32bit, locfhrh);
   test_each(int32, signed32bit, locfhrl);
}

/* load count to block boundary */
#define declare_load_count_to_block_boundary(M_FIELD)                          \
   bool test_lcbb##M_FIELD(const uint32_t testedValue) {                       \
      bool testPassed;                                                         \
      const size_t boundary = 64 * pow(2, M_FIELD);                            \
      const uint32_t *testedPointer =                                          \
          (uint32_t *)(boundary -                                              \
                       ((testedValue < boundary) ? testedValue : 0));          \
      uint32_t valueAfterTest = 0;                                             \
      uint32_t expectedValue = boundary - ((size_t)testedPointer % boundary);  \
      if (expectedValue > 16)                                                  \
         expectedValue = 16;                                                   \
                                                                               \
      __asm__ volatile("lcbb %[valueAfterTest], %[testedPointer], " #M_FIELD   \
                       "\n"                                                    \
                       : [valueAfterTest] "=r"(valueAfterTest)                 \
                       : [testedPointer] "m"(*testedPointer));                 \
                                                                               \
      testPassed = (valueAfterTest == expectedValue);                          \
      if (!testPassed) {                                                       \
         fprintf(stderr, "ERROR lcbb" #M_FIELD ": %x -> %x | expected: %x\n",  \
                 testedValue, valueAfterTest, expectedValue);                  \
      }                                                                        \
      return testPassed;                                                       \
   }

declare_load_count_to_block_boundary(0) declare_load_count_to_block_boundary(1)
    declare_load_count_to_block_boundary(2)
        declare_load_count_to_block_boundary(3)
            declare_load_count_to_block_boundary(4)
                declare_load_count_to_block_boundary(5)
                    declare_load_count_to_block_boundary(6)

                        static void test_all_lzr() {
   uint32_t unsigned32bit = 0;

   test_each(uint32, unsigned32bit, lzrf);
   test_each(uint32, unsigned32bit, lzrg);
}

static void test_all_stocfh() {
   int32_t signed32bit = 0;

   test_each(int32, signed32bit, stocfhne);
   test_each(int32, signed32bit, stocfhe);
   test_each(int32, signed32bit, stocfhh);
   test_each(int32, signed32bit, stocfhl);
}

static void test_all_lochi() {
   int32_t signed32bit = 0;

   test_each(int32, signed32bit, locghine);
   test_each(int32, signed32bit, locghine);
   test_each(int32, signed32bit, locghih);
   test_each(int32, signed32bit, locghil);

   test_each(int32, signed32bit, lochine);
   test_each(int32, signed32bit, lochie);
   test_each(int32, signed32bit, lochil);
   test_each(int32, signed32bit, lochih);
}

static void test_all_lochhi() {
   int32_t signed32bit = 0;

   test_each(int32, signed32bit, lochhine);
   test_each(int32, signed32bit, lochhie);
   test_each(int32, signed32bit, lochhih);
   test_each(int32, signed32bit, lochhil);
}

static void test_all_lcbb() {
   size_t tested = 0;

   test_range(tested, 0, 64, lcbb0);
   test_range(tested, 0, 128, lcbb1);
   test_range(tested, 0, 256, lcbb2);
   test_range(tested, 0, 512, lcbb3);
   test_range(tested, 0, 1024, lcbb4);
   test_range(tested, 0, 2048, lcbb5);
   test_range(tested, 0, 4096, lcbb6);
}

void test_long_all() {
   uint64_t unsigned64bit = 0;

   test_all_lcbb();
   test_all_lochhi();
   test_all_lochi();
   test_all_stocfh();
   test_all_locfh();
   test_all_lzr();
   test_each(uint64, unsigned64bit, llzrgf);
}

#define SHORT_TESTS_UNSIGNED_FROM 0
#define SHORT_TESTS_SIGNED_FROM -0xffff
#define SHORT_TESTS_TO 0xffff
void test_short_all() {
   uint32_t unsigned32bit = 0;
   int32_t signed32bit = 0;
   uint64_t unsigned64bit = 0;

   test_range(unsigned32bit, SHORT_TESTS_UNSIGNED_FROM, SHORT_TESTS_TO, lzrf);
   test_range(unsigned64bit, SHORT_TESTS_UNSIGNED_FROM, SHORT_TESTS_TO, lzrg);
   test_range(unsigned64bit, SHORT_TESTS_UNSIGNED_FROM, SHORT_TESTS_TO, llzrgf);

   /* stocfh */
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, stocfhne);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, stocfhe);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, stocfhh);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, stocfhl);

   /* locfh */
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhe);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhne);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhh);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhl);

   /* locfhr */
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhre);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhrne);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhrh);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locfhrl);

   /* lochhi */
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochhine);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochhie);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochhih);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochhil);

   /* lochi */
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochine);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochie);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochih);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, lochil);

   /* locghi */
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locghine);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locghie);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locghih);
   test_range(signed32bit, SHORT_TESTS_SIGNED_FROM, SHORT_TESTS_TO, locghil);

   test_all_lcbb(); /* These test is not long, so we can run it on all range */
}

int main(int argc, char *argv[]) {
   bool shouldRunLongTests = false;

   if (argc > 1)
      if (strcmp(argv[1], "--long") == 0)
         shouldRunLongTests = true;

   printf("Tests started:\n");
   if (shouldRunLongTests)
      test_long_all();
   else
      test_short_all();

   printf("Tests ended.\n");
   return 0;
}
