#ifndef S390_TEST_VECTOR_H
#define S390_TEST_VECTOR_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

/* Colors for output */
#ifndef S390_TEST_NOCOLORS
#   define RED "\033[31m"
#   define GREEN "\033[32m"
#   define RESET "\033[0m"
#endif

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

#define SMART_RETURN_R64(insn)                        \
   bool result = after == expected;                   \
   if (!result) {                                     \
      printf("[ERROR] %s:\n", #insn);                 \
      printf("after: %lx\n", (uint64_t) after);       \
      printf("expected: %lx\n", (uint64_t) expected); \
   }                                                  \
   return result;

#endif
