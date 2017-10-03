#ifndef S390_TEST_VECTOR_H
#define S390_TEST_VECTOR_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

/* Colors for output */
#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#define printPassed(insn) \
   printf(RESET "%15s :\t " GREEN "PASSED" RESET "\n", #insn)
#define printFailed(insn) \
   printf(RESET "%15s :\t " RED "FAILED" RESET "\n", #insn)

#define test(insn)          \
   {                        \
      if (test_##insn())    \
         printPassed(insn); \
      else                  \
         printFailed(insn); \
   }

#ifdef __GNUC__
/* GCC complains about __int128 with -pedantic */
/* Hope that we will have int128_t in C standard someday. */
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

typedef union {
   uint8_t u8[16];
   int8_t s8[16];

   uint16_t u16[8];
   int16_t s16[8];

   uint32_t u32[4];
   int32_t s32[4];

   uint64_t u64[2];
   int64_t s64[2];

   unsigned __int128 u128[1];
   __int128 s128[1];
} V128;

#define SMART_RETURN_V128(insn)                                        \
   bool result = memcmp(expected.u8, after.u8, sizeof(expected)) == 0; \
   if (!result) {                                                      \
      printf("[ERROR] %s:\n", #insn);                                  \
      printf("before:   ");                                            \
      printHex(before);                                                \
      printf("after:    ");                                            \
      printHex(after);                                                 \
      printf("expected: ");                                            \
      printHex(expected);                                              \
   }                                                                   \
   return result;

#define SMART_RETURN_V128_2op(insn)                                    \
   bool result = memcmp(expected.u8, after.u8, sizeof(expected)) == 0; \
   if (!result) {                                                      \
      printf("[ERROR] %s:\n", #insn);                                  \
      printf("op1:      ");                                            \
      printHex(op1);                                                   \
      printf("op2:      ");                                            \
      printHex(op2);                                                   \
      printf("after:    ");                                            \
      printHex(after);                                                 \
      printf("expected: ");                                            \
      printHex(expected);                                              \
   }                                                                   \
   return result;

#define SMART_RETURN_R64(insn)             \
   bool result = after == expected;        \
   if (!result) {                          \
      printf("[ERROR] %s:\n", #insn);      \
      printf("after: %lx\n", after);       \
      printf("expected: %lx\n", expected); \
   }                                       \
   return result;

#define SMART_RETURN_V128x2(insn)                            \
   bool result = memcmp(expected1.u8, after1.u8, 16) == 0 && \
                 memcmp(expected2.u8, after2.u8, 16) == 0;   \
   if (!result) {                                            \
      printf("[ERROR] %s:\n", #insn);                        \
      printf("after1:  ");                                   \
      printHex(after1);                                      \
      printf("expected1: ");                                 \
      printHex(expected1);                                   \
      printf("after2:    ");                                 \
      printHex(after2);                                      \
      printf("expected2: ");                                 \
      printHex(expected2);                                   \
   }                                                         \
   return result;

#define SMART_RETURN_V128_VV(insn)                                     \
   bool result = memcmp(expected.u8, after.u8, sizeof(expected)) == 0; \
   if (!result) {                                                      \
      printf("[ERROR] %s:\n", #insn);                                  \
      printf("before1:   ");                                           \
      printHex(before1);                                               \
      printf("before2:   ");                                           \
      printHex(before2);                                               \
                                                                       \
      printf("after:     ");                                           \
      printHex(after);                                                 \
      printf("expected:  ");                                           \
      printHex(expected);                                              \
   }                                                                   \
   return result;

void printHex(const V128 value) {
   printf("%016lx | %016lx\n", value.u64[0], value.u64[1]);
}

/* vector integer stuff goes here. Some usefull macros for generating
   test functions. */

#define __b_max 16
#define __h_max 8
#define __hw_max __h_max
#define __f_max 4
#define __g_max 2
#define __q_max 1
#define ___max __b_max

#define __bs s8
#define __hs s16
#define __hws __hs
#define __fs s32
#define __gs s64
#define __bu u8
#define __hu u16
#define __hwu u16
#define __fu u32
#define __gu u64
#define __qu u128
#define __qs s128
#define __u __bs
#define __s __bu

#define __wider_bs s16
#define __wider_hs s32
#define __wider_hws __wider_hs
#define __wider_fs s64
#define __wider_bu u16
#define __wider_hu u32
#define __wider_hwu __wider_hu
#define __wider_fu u64
#define __wider_u __wider_bs
#define __wider_s __wider_bu

#define __bm "0"
#define __hm "1"
#define __hwm __hm
#define __fm "2"
#define __gm "3"
#define __qm "4"

#define get_elem_same(variable, postfix, sign) variable.__##postfix##sign
#define get_elem_wider(variable, postfix, sign) variable.__wider_##postfix##sign

#define __get_from_full(postfix) 0
#define __get_from_low(postfix) __get_from_full(postfix)
#define __get_from_high(postfix) ((__##postfix##_max) / 2)
#define __get_from_odd(postfix) 1
#define __get_from_even(postfix) __get_from_full(postfix)

#define __get_to_full(postfix) __##postfix##_max
#define __get_to_low(postfix) __get_from_high(postfix)
#define __get_to_high(postfix) __get_to_full(postfix)
#define __get_to_odd(postfix) __get_to_full(postfix)
#define __get_to_even(postfix) __get_to_full(postfix)

#define __get_delta_full(postfix) 1
#define __get_delta_low(postfix) __get_delta_full(postfix)
#define __get_delta_high(postfix) __get_delta_full(postfix)
#define __get_delta_odd(postfix) 2
#define __get_delta_even(postfix) __get_delta_odd(postfix)

#define declare_vv_binary(insn, postfix, sign, operation, range, outputType)   \
                                                                               \
   bool test_##insn##postfix() {                                               \
      size_t inputElem, outputElem;                                            \
      V128 op1 = {.u8 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};           \
      V128 op2 = {.u8 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x88, \
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};           \
      V128 expected;                                                           \
      V128 after;                                                              \
      outputElem = 0;                                                          \
      for (inputElem = __get_from_##range(postfix);                            \
           inputElem < __get_to_##range(postfix);                              \
           inputElem += __get_delta_##range(postfix)) {                        \
         get_elem_##outputType(expected, postfix, sign)[outputElem] =          \
             operation(get_elem_same(op1, postfix, sign)[inputElem],           \
                       get_elem_same(op2, postfix, sign)[inputElem]);          \
         outputElem++;                                                         \
      }                                                                        \
                                                                               \
      __asm__ volatile(                                                        \
          "vl %%v1, %[op1]\n"                                                  \
          "vl %%v2, %[op2]\n" #insn #postfix                                   \
          " %%v3, %%v1, %%v2\n"                                                \
          "vst %%v3, %[result]\n"                                              \
          : [result] "=m"(after)                                               \
          : [op1] "m"(op1), [op2] "m"(op2));                                   \
                                                                               \
      SMART_RETURN_V128_2op(insn##postfix);                                    \
   }

#define declare_vv_binary_all_with_q(insn, signCharacter, operation)          \
   declare_vv_binary(insn, b, signCharacter, operation, full, same)           \
       declare_vv_binary(insn, h, signCharacter, operation, full, same)       \
           declare_vv_binary(insn, f, signCharacter, operation, full, same)   \
               declare_vv_binary(insn, g, signCharacter, operation, full,     \
                                 same)                                        \
                   declare_vv_binary(insn, q, signCharacter, operation, full, \
                                     same)

#define declare_vv_binary_all_without_q(insn, signCharacter, operation)     \
   declare_vv_binary(insn, b, signCharacter, operation, full, same)         \
       declare_vv_binary(insn, h, signCharacter, operation, full, same)     \
           declare_vv_binary(insn, f, signCharacter, operation, full, same) \
               declare_vv_binary(insn, g, signCharacter, operation, full,   \
                                 same)

#define declare_vv_binary_all_without_qg(insn, signCharacter, operation) \
   declare_vv_binary(insn, b, signCharacter, operation, full, same)      \
       declare_vv_binary(insn, h, signCharacter, operation, full, same)  \
           declare_vv_binary(insn, f, signCharacter, operation, full, same)

#define declare_vv_binary_all_signed_with_q(insn, operation) \
   declare_vv_binary_all_with_q(insn, s, operation)

#define declare_vv_binary_all_unsigned_with_q(insn, operation) \
   declare_vv_binary_all_with_q(insn, u, operation)

#define declare_vv_binary_all_signed_without_q(insn, operation) \
   declare_vv_binary_all_without_q(insn, s, operation)

#define declare_vv_binary_all_signed_without_qg(insn, operation) \
   declare_vv_binary_all_without_qg(insn, s, operation)

#define declare_vv_binary_all_unsigned_without_q(insn, operation) \
   declare_vv_binary_all_without_q(insn, u, operation)

#define declare_vv_binary_all_unsigned_without_qg(insn, operation) \
   declare_vv_binary_all_without_qg(insn, u, operation)

#define declare_vv_unary(insn, postfix, sign, operation)                      \
   bool test_##insn##postfix() {                                              \
      size_t elem;                                                            \
      V128 before = {.u8 = {0x00, 0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,   \
                            0x88, 0x09, 0x0a, 0x0b, 0x0c, 0xdd, 0x0e, 0xff}}; \
      V128 expected;                                                          \
      V128 after;                                                             \
      for (elem = 0; elem < __##postfix##_max; elem++) {                      \
         get_elem_same(expected, postfix, sign)[elem] =                       \
             operation(get_elem_same(before, postfix, sign)[elem]);           \
      }                                                                       \
                                                                              \
      __asm__ volatile("vl %%v1, %[before]\n" #insn                           \
                       " %%v3, %%v1, " __##postfix##m                         \
                       "\n"                                                   \
                       "vst %%v3, %[result]\n"                                \
                       : [result] "=m"(after)                                 \
                       : [before] "m"(before));                               \
                                                                              \
      SMART_RETURN_V128(insn##postfix);                                       \
   }

#define declare_vv_unary_all_without_q(insn, signCharacter, operation) \
   declare_vv_unary(insn, b, signCharacter, operation)                 \
       declare_vv_unary(insn, h, signCharacter, operation)             \
           declare_vv_unary(insn, f, signCharacter, operation)         \
               declare_vv_unary(insn, g, signCharacter, operation)

#define declare_vv_unary_all_signed_without_q(insn, operation) \
   declare_vv_unary_all_without_q(insn, s, operation)

#define declare_vv_unary_all_unsigned_without_q(insn, operation) \
   declare_vv_unary_all_without_q(insn, u, operation)

#define __saturated_s(val, bitCount) \
   ((val) < INT##bitCount##_MIN      \
        ? INT##bitCount##_MIN        \
        : ((val) > INT##bitCount##_MAX) ? INT##bitCount##_MAX : val)

#define __saturated_u(val, bitCount) \
   (((val) > UINT##bitCount##_MAX) ? UINT##bitCount##_MAX : (val))

#define saturated(val, sign, bitCount) (__saturated_##sign(val, bitCount))

#endif
