/*
 * To build:
 * gcc --std=c99 test-s390x-vector.c -march=z13 -Wall -Wextra -pedantic -o
 * test-s390x-vector
 */

#include "test-s390x-vector.h"

static bool test_vl_vst() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0x11, 0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   memset(after.u8, 0, sizeof(after));
   memcpy(expected.u8, before.u8, sizeof(before));

   __asm__ volatile(
       "vl %%v1, %[before]\n"
       "vst %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vl_vst);
}

static bool test_vlr() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0x11, 0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   memset(after.u8, 0, sizeof(after));
   memcpy(expected.u8, before.u8, sizeof(before));

   __asm__ volatile(
       "vl %%v22, %[before]\n"
       "vlr %%v1, %%v22\n"
       "vst %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vlr);
}

static bool test_vlrepg() {
   uint64_t value = 6;
   V128 expected;
   V128 after;
   V128 before;
   memset(after.u8, 0, sizeof(after));
   expected.u64[0] = value;
   expected.u64[1] = value;

   __asm__ volatile(
       "vlrepg %%v1, %[value]\n"
       "vst %%v1, %[after]\n"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vlrepg);
}

static bool test_vlrepf() {
   uint32_t value = 7;
   V128 expected;
   V128 after;
   V128 before;
   memset(after.u8, 0, sizeof(after));

   expected.u32[0] = value;
   expected.u32[1] = value;
   expected.u32[2] = value;
   expected.u32[3] = value;

   __asm__ volatile(
       "vlrepf %%v2, %[value]\n"
       "vst %%v2, %[after]\n"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vlrepf);
}

static bool test_vlreph() {
   uint16_t value = 8;
   V128 expected;
   V128 after;
   V128 before;
   memset(after.u8, 0, sizeof(after));

   expected.u16[0] = value;
   expected.u16[1] = value;
   expected.u16[2] = value;
   expected.u16[3] = value;
   expected.u16[4] = value;
   expected.u16[5] = value;
   expected.u16[6] = value;
   expected.u16[7] = value;

   __asm__ volatile(
       "vlreph %%v3, %[value]\n"
       "vst    %%v3, %[after]"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vlreph);
}

static bool test_vlrepb() {
   uint8_t value = 9;
   V128 expected;
   V128 after;
   V128 before;
   memset(after.u8, 0, sizeof(after));
   memset(expected.u8, value, sizeof(expected));

   __asm__ volatile(
       "vlrepb %%v4, %[value]\n"
       "vst    %%v4, %[after]"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vlrepb);
}

static bool test_vle() {
   uint8_t first = 0xaa;
   uint16_t second = 0xbbbb;
   uint32_t third = 0xcccccccc;
   uint64_t fourth = 0xddddddddddddddddUL;
   V128 before;
   V128 expected = {.u8 = {0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xcc,
                           0xcc, 0xcc, 0xcc, 0xbb, 0xbb, 0xaa, 0x55}};
   V128 after;

   before.u8[15] = 0x55;
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vleb %%v1, %[first], 14\n"
       "vleh %%v1, %[second], 6\n"
       "vlef %%v1, %[third], 2\n"
       "vleg %%v1, %[fourth], 0\n"
       "vst  %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [first] "m"(first), [second] "m"(second),
         [third] "m"(third), [fourth] "m"(fourth));

   SMART_RETURN_V128(vle);
}

static bool test_vlei_pos() {
   V128 before;
   V128 expected = {.u8 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0xdd, 0x00,
                           0x00, 0x1c, 0xcc, 0x1b, 0xbb, 0x1a, 0x55}};
   V128 after;

   before.u8[15] = 0x55;
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vleib %%v1, 0x091a, 14\n"
       "vleih %%v1, 0x1bbb, 6\n"
       "vleif %%v1, 0x1ccc, 2\n"
       "vleig %%v1, 0x1ddd, 0\n"
       "vst  %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vlei_pos);
}

static bool test_vlei_neg() {
   V128 before;
   V128 expected = {.u8 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe2, 0x23, 0xff,
                           0xff, 0xe3, 0x34, 0xe4, 0x45, 0xe6, 0x55}};
   V128 after;

   before.u8[15] = 0x55;
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vleib %%v1, -0x091a, 14\n"
       "vleih %%v1, -0x1bbb, 6\n"
       "vleif %%v1, -0x1ccc, 2\n"
       "vleig %%v1, -0x1ddd, 0\n"
       "vst  %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vlei_neg);
}

static bool test_vlgvb() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   uint64_t expected;
   uint64_t after;

   expected = before.u8[10];
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vlgvb %[after], %%v1, 10(%[zero])\n"
       : [after] "=r"(after)
       : [before] "m"(before), [zero] "r"(zero));

   SMART_RETURN_R64(vlgvb);
}

static bool test_vlgvh() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   uint64_t expected;
   uint64_t after;

   expected = before.u16[5];
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vlgvh %[after], %%v1, 5(%[zero])\n"
       : [after] "=r"(after)
       : [before] "m"(before), [zero] "r"(zero));

   SMART_RETURN_R64(vlgvh);
}

static bool test_vlgvf() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   uint64_t expected;
   uint64_t after;

   expected = before.u32[2];
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vlgvf %[after], %%v1, 2(%[zero])\n"
       : [after] "=r"(after)
       : [before] "m"(before), [zero] "r"(zero));

   SMART_RETURN_R64(vlgvf);
}

static bool test_vlgvg() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   uint64_t expected;
   uint64_t after;

   expected = before.u64[1];
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vlgvg %[after], %%v1, 1(%[zero])\n"
       : [after] "=r"(after)
       : [before] "m"(before), [zero] "r"(zero));

   SMART_RETURN_R64(vlgvg);
}

static bool test_vgbm() {
   V128 after;
   V128 before;
   V128 expected = {.u8 = {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
                           0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00}};

   __asm__ volatile(
       "vgbm %%v4, 0b1100001110010110\n"
       "vst  %%v4, %[after]\n"
       : [after] "=m"(after));

   SMART_RETURN_V128(vgbm);
}

static bool test_vgmb() {
   V128 after;
   V128 before;
   V128 expected;

   memset(expected.u8, 0x1e, sizeof(expected));
   __asm__ volatile(
       "vgmb %%v4, 3, 6\n"
       "vst %%v4, %[after]\n"
       : [after] "=m"(after));

   SMART_RETURN_V128(vgmb);
}

static bool test_vgmh() {
   V128 after;
   V128 before;
   V128 expected = {.u8 = {0x0f, 0xfe, 0x0f, 0xfe, 0x0f, 0xfe, 0x0f, 0xfe, 0x0f,
                           0xfe, 0x0f, 0xfe, 0x0f, 0xfe, 0x0f, 0xfe}};

   __asm__ volatile(
       "vgmh %%v4, 4, 14\n"
       "vst %%v4, %[after]\n"
       : [after] "=m"(after));

   SMART_RETURN_V128(vgmh);
}

static bool test_vgmf() {
   V128 after;
   V128 before;
   V128 expected = {.u8 = {0x1f, 0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x1f,
                           0xff, 0xff, 0x00, 0x1f, 0xff, 0xff, 0x00}};

   __asm__ volatile(
       "vgmf %%v4, 3, 23\n"
       "vst %%v4, %[after]\n"
       : [after] "=m"(after));

   SMART_RETURN_V128(vgmf);
}

static bool test_vgmg() {
   V128 after;
   V128 before;
   V128 expected = {.u8 = {0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f,
                           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00}};

   __asm__ volatile(
       "vgmg %%v5, 3, 55\n"
       "vst %%v5, %[after]\n"
       : [after] "=m"(after));

   SMART_RETURN_V128(vgmg);
}

static bool test_vllezb() {
   V128 after;
   V128 expected;
   V128 before;
   uint8_t value = 0xaa;

   memset(expected.u8, 0, sizeof(expected));
   expected.u8[7] = value;
   __asm__ volatile(
       "vllezb %%v5, %[value]\n"
       "vst   %%v5, %[after]\n"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vllezb);
}

static bool test_vllezh() {
   V128 after;
   V128 expected;
   V128 before;
   uint16_t value = 0xaabb;

   memset(expected.u8, 0, sizeof(expected));
   expected.u16[3] = value;
   __asm__ volatile(
       "vllezh %%v5, %[value]\n"
       "vst   %%v5, %[after]\n"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vllezh);
}

static bool test_vllezf() {
   V128 after;
   V128 expected;
   V128 before;
   uint32_t value = 0xaabbccdd;

   memset(expected.u8, 0, sizeof(expected));
   expected.u32[1] = value;
   __asm__ volatile(
       "vllezf %%v5, %[value]\n"
       "vst   %%v5, %[after]\n"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vllezf);
}

static bool test_vllezg() {
   V128 after;
   V128 expected;
   V128 before;
   uint64_t value = 0xaabbccddeeff1122UL;

   memset(expected.u8, 0, sizeof(expected));
   expected.u64[0] = value;
   __asm__ volatile(
       "vllezg %%v5, %[value]\n"
       "vst   %%v5, %[after]\n"
       : [after] "=m"(after)
       : [value] "m"(value));

   SMART_RETURN_V128(vllezg);
}

static bool test_vgef() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   V128 pattern = {.u8 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                          0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};
   uint32_t pointer[4] = {4, 0, 8, 12};

   memcpy(expected.u8, before.u8, sizeof(before));
   expected.u32[1] = *(uint32_t *)(pattern.u8 + pointer[1]);

   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vl   %%v2, %[pointer]\n"
       "vgef %%v1, 0(%%v2, %[base]), 1\n"
       "vst  %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [base] "r"(&pattern), [pointer] "m"(pointer));

   SMART_RETURN_V128(vgef);
}

static bool test_vgeg() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   V128 pattern = {.u8 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                          0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};
   uint64_t pointer[2] = {0, 8};

   memcpy(expected.u8, before.u8, sizeof(before));
   expected.u64[1] = *(uint64_t *)(pattern.u8 + pointer[1]);

   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vl   %%v2, %[pointer]\n"
       "vgeg %%v1, 0(%%v2, %[base]), 1\n"
       "vst  %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [base] "r"(&pattern), [pointer] "m"(pointer));

   SMART_RETURN_V128(vgef);
}

static bool test_vlm() {
   V128 after1;
   V128 after2;

   uint8_t expected[32] = {0x01, 0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x01,
                           0x01, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x01,
                           0x02, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x02,
                           0x02, 0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x02};
   V128 expected1, expected2;
   memcpy(expected1.u8, expected, 16);
   memcpy(expected2.u8, expected + 16, 16);

   __asm__ volatile(
       "vlm %%v5, %%v6, %[from]\n"
       "vst %%v5, %[after1]\n"
       "vst %%v6, %[after2]\n"
       : [after1] "=m"(after1.u8), [after2] "=m"(after2.u8)
       : [from] "m"(expected));

   SMART_RETURN_V128x2(vlm);
}

static bool test_vlvgp() {
   V128 before;
   V128 after;
   V128 expected;

   uint64_t value1 = 0x55;
   uint64_t value2 = 0xaa;

   expected.u64[0] = value1;
   expected.u64[1] = value2;

   __asm__ volatile(
       "vlvgp %%v1, %[value1], %[value2]\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [value1] "r"(value1), [value2] "r"(value2));

   SMART_RETURN_V128(vlvgp);
}

static bool test_vlvgb() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   V128 after;
   V128 expected;
   uint8_t value = 0x99;

   memcpy(expected.u8, before.u8, sizeof(expected));
   expected.u8[4] = value;

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vlvgb %%v1, %[value], 4(%[zero])\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [zero] "r"(zero), [value] "r"(value));

   SMART_RETURN_V128(vlvgb);
}

static bool test_vlvgh() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   V128 after;
   V128 expected;
   uint16_t value = 0x9999;

   memcpy(expected.u8, before.u8, sizeof(expected));
   expected.u16[2] = value;
   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vlvgh %%v1, %[value], 2(%[zero])\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [zero] "r"(zero), [value] "r"(value));

   SMART_RETURN_V128(vlvgh);
}

static bool test_vlvgf() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   V128 after;
   V128 expected;
   uint32_t value = 0x99999999;

   memcpy(expected.u8, before.u8, sizeof(expected));
   expected.u32[3] = value;

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vlvgf %%v1, %[value], 3(%[zero])\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [zero] "r"(zero), [value] "r"(value));

   SMART_RETURN_V128(vlvghf);
}

static bool test_vlvgg() {
   uint64_t zero = 0;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   V128 after;
   V128 expected;
   uint64_t value = 0x9999999999999999UL;

   memcpy(expected.u8, before.u8, sizeof(expected));
   expected.u64[1] = value;
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vlvgg %%v1, %[value], 1(%[zero])\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [zero] "r"(zero), [value] "r"(value));

   SMART_RETURN_V128(vlvgg);
}

static bool test_vmrhb() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.u8[outputElement] = before1.u8[inputElement];
      outputElement++;
      expected.u8[outputElement] = before2.u8[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrhb %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrhb);
}

static bool test_vmrhh() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.u16[outputElement] = before1.u16[inputElement];
      outputElement++;
      expected.u16[outputElement] = before2.u16[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrhh %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrhh);
}

static bool test_vmrhf() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 0; inputElement < 2; inputElement++) {
      expected.u32[outputElement] = before1.u32[inputElement];
      outputElement++;
      expected.u32[outputElement] = before2.u32[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrhf %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrhf);
}

static bool test_vmrhg() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 0; inputElement < 1; inputElement++) {
      expected.u64[outputElement] = before1.u64[inputElement];
      outputElement++;
      expected.u64[outputElement] = before2.u64[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrhg %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrhg);
}

static bool test_vmrlb() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 8; inputElement < 16; inputElement++) {
      expected.u8[outputElement] = before1.u8[inputElement];
      outputElement++;
      expected.u8[outputElement] = before2.u8[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrlb %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrlb);
}

static bool test_vmrlh() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 4; inputElement < 8; inputElement++) {
      expected.u16[outputElement] = before1.u16[inputElement];
      outputElement++;
      expected.u16[outputElement] = before2.u16[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrlh %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrhh);
}

static bool test_vmrlf() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 2; inputElement < 4; inputElement++) {
      expected.u32[outputElement] = before1.u32[inputElement];
      outputElement++;
      expected.u32[outputElement] = before2.u32[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrlf %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrlf);
}

static bool test_vmrlg() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};

   for (inputElement = 1; inputElement < 2; inputElement++) {
      expected.u64[outputElement] = before1.u64[inputElement];
      outputElement++;
      expected.u64[outputElement] = before2.u64[inputElement];
      outputElement++;
   }
   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vmrlg %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vmrlg);
}

static bool test_vpkh() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.u8[outputElement] = before1.u16[inputElement];
      outputElement++;
   }
   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.u8[outputElement] = before2.u16[inputElement];
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpkh %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpkh);
}

static bool test_vpkf() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.u16[outputElement] = before1.u32[inputElement];
      outputElement++;
   }
   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.u16[outputElement] = before2.u32[inputElement];
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpkf %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpkf);
}

static bool test_vpkg() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 2; inputElement++) {
      expected.u32[outputElement] = before1.u64[inputElement];
      outputElement++;
   }
   for (inputElement = 0; inputElement < 2; inputElement++) {
      expected.u32[outputElement] = before2.u64[inputElement];
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpkg %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpkg);
}

static bool test_vperm() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};
   uint8_t pattern[16] = {0x01, 0x04, 0x05, 0x06, 0x02, 0x09, 0x03, 0x07,
                          0x18, 0x1a, 0x19, 0x1b, 0x1d, 0x1c, 0x1f, 0x1e};

   for (elem = 0; elem < 16; elem++) {
      if (pattern[elem] >= 16) {
         expected.u8[elem] = before2.u8[pattern[elem] - 16];
      } else {
         expected.u8[elem] = before1.u8[pattern[elem]];
      }
   }
   __asm__ volatile(
       "vl    %%v1, %[before1]\n"
       "vl    %%v2, %[before2]\n"
       "vl    %%v3, %[pattern]\n"
       "vperm %%v4, %%v1, %%v2, %%v3\n"
       "vst   %%v4, %[after]\n"
       : [after] "=m"(after)
       :
       [before1] "m"(before1), [before2] "m"(before2), [pattern] "m"(pattern));

   SMART_RETURN_V128_VV(vpkg);
}

static bool test_vpdi() {
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   expected.u64[0] = ((10 & 2) != 0) ? before1.u64[0] : before2.u64[1];
   expected.u64[1] = ((10 & 8) != 0) ? before1.u64[1] : before2.u64[2];

   __asm__ volatile(
       "vl    %%v1, %[before1]\n"
       "vl    %%v2, %[before2]\n"
       "vpdi  %%v4, %%v1, %%v2, 10\n"
       "vst   %%v4, %[after]\n"
       : [after] "=m"(after.u8)
       : [before1] "m"(before1.u8), [before2] "m"(before2.u8));

   SMART_RETURN_V128_VV(vpdi);
}

static bool test_vscef() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   uint32_t pointer[4] = {0, 4, 8, 12};

   memset(expected.u8, 0, sizeof(expected));
   memset(after.u8, 0, sizeof(after));
   *((uint32_t *)(expected.u8 + pointer[1])) = before.u32[1];
   __asm__ volatile(
       "vl   %%v1, %[before]\n"
       "vl   %%v2, %[pointer]\n"
       "vscef %%v1, 0(%%v2, %[base]), 1\n"
       :
       : [before] "m"(before), [base] "r"(&after), [pointer] "m"(pointer));

   SMART_RETURN_V128(vscef);
}

static bool test_vsceg() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x11,
                         0x22, 0x33, 0x33, 0x44, 0x55, 0x66, 0x66}};
   uint64_t pointer[4] = {0, 8};

   memset(expected.u8, 0, sizeof(expected));
   memset(after.u8, 0, sizeof(after));
   *((uint64_t *)(expected.u8 + pointer[1])) = before.u64[1];
   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vl    %%v2, %[pointer]\n"
       "vsceg %%v1, 0(%%v2, %[base]), 1\n"
       :
       : [before] "m"(before), [base] "r"(&after), [pointer] "m"(pointer));

   SMART_RETURN_V128(vsceg);
}

static bool test_vsegb() {
   V128 expected;
   V128 after;
   V128 before;

   memset(expected.u8, 0, sizeof(expected));
   memset(before.u8, 0, sizeof(before));
   before.s8[7] = -1;
   before.s8[15] = 1;
   expected.s64[0] = before.s8[7];
   expected.s64[1] = before.s8[15];

   __asm__ volatile(
       "vl    %%v2, %[before]\n"
       "vsegb %%v1, %%v2\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vsegb);
}

static bool test_vsegh() {
   V128 expected;
   V128 after;
   V128 before;

   memset(expected.u8, 0, sizeof(expected));
   memset(before.u8, 0, sizeof(before));
   before.s16[3] = -1;
   before.s16[7] = 1;
   expected.s64[0] = before.s16[3];
   expected.s64[1] = before.s16[7];

   __asm__ volatile(
       "vl    %%v2, %[before]\n"
       "vsegh %%v1, %%v2\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vsegh);
}

static bool test_vsegf() {
   V128 expected;
   V128 after;
   V128 before;

   memset(expected.u8, 0, sizeof(expected));
   memset(before.u8, 0, sizeof(before));
   before.s32[1] = -1;
   before.s32[3] = 1;
   expected.s64[0] = before.s32[1];
   expected.s64[1] = before.s32[3];

   __asm__ volatile(
       "vl    %%v2, %[before]\n"
       "vsegf %%v1, %%v2\n"
       "vst   %%v1, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vsegf);
}

static bool test_vste() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   memset(after.u8, 0, sizeof(after));
   memcpy(expected.u8, before.u8, sizeof(before));
   expected.u8[0] = 0x00;
   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vsteg %%v1, %[u64], 1\n"
       "vstef %%v1, %[u32], 1\n"
       "vsteh %%v1, %[u16], 1\n"
       "vsteb %%v1, %[u8], 1"
       : [u64] "=m"(after.u64[1]), [u32] "=m"(after.u32[1]),
         [u16] "=m"(after.u16[1]), [u8] "=m"(after.u8[1])
       : [before] "m"(before));

   SMART_RETURN_V128(vste);
}

static bool test_vstm() {
   V128 after1;
   V128 after2;

   uint8_t after[32];
   uint8_t before[32] = {0x01, 0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x01,
                         0x01, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x01,
                         0x02, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x02,
                         0x02, 0x00, 0x22, 0x33, 0x44, 0x55, 0x66, 0x02};
   V128 expected1, expected2;
   memcpy(expected1.u8, before, 16);
   memcpy(expected2.u8, before + 16, 16);

   __asm__ volatile(
       "vl  %%v1, %[before1]\n"
       "vl  %%v2, %[before2]\n"
       "vstm %%v1, %%v2, %[to]\n"
       : [to] "=m"(after)
       : [before1] "m"(before), [before2] "m"(before[16]));

   memcpy(after1.u8, after, 16);
   memcpy(after2.u8, after + 16, 16);

   SMART_RETURN_V128x2(vstm);
}

static bool test_vuphb() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 8; elem++) {
      expected.s16[elem] = before.s8[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuphb %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuphb);
}

static bool test_vuphh() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 4; elem++) {
      expected.s32[elem] = before.s16[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuphh %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuphh);
}

static bool test_vuphf() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 2; elem++) {
      expected.s64[elem] = before.s32[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuphf %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuphf);
}

static bool test_vuplhb() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 8; elem++) {
      expected.u16[elem] = before.u8[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuplhb %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuplhb);
}

static bool test_vuplhh() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 4; elem++) {
      expected.u32[elem] = before.u16[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuplhh %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuplhh);
}

static bool test_vuplhf() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 2; elem++) {
      expected.u64[elem] = before.u32[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuplhf %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuplhf);
}

static bool test_vuplb() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 8; elem < 16; elem++) {
      expected.s16[elem - 8] = before.s8[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuplb %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuplb);
}

static bool test_vuplhw() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 4; elem < 8; elem++) {
      expected.s32[elem - 4] = before.s16[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuplhw %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuplhw);
}

static bool test_vuplf() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 2; elem < 4; elem++) {
      expected.s64[elem - 2] = before.s32[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vuplf %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vuplf);
}

static bool test_vupllb() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 8; elem < 16; elem++) {
      expected.u16[elem - 8] = before.u8[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vupllb %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vupllb);
}

static bool test_vupllh() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 4; elem < 8; elem++) {
      expected.u32[elem - 4] = before.u16[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vupllh %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vupllh);
}

static bool test_vupllf() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 2; elem < 4; elem++) {
      expected.u64[elem - 2] = before.u32[elem];
   }

   __asm__ volatile(
       "vl    %%v1, %[before]\n"
       "vupllf %%v2, %%v1\n"
       "vst   %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vupllf);
}

static bool test_vrepb() {
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   memset(expected.u8, before.u8[1], sizeof(V128));

   __asm__ volatile(
       "vl     %%v1, %[before]\n"
       "vrepb  %%v2, %%v1, 1\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vrepb);
}

static bool test_vreph() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 8; elem++) {
      expected.u16[elem] = before.u16[1];
   }

   __asm__ volatile(
       "vl     %%v1, %[before]\n"
       "vreph  %%v2, %%v1, 1\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vreph);
}

static bool test_vrepf() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 4; elem++) {
      expected.u32[elem] = before.u32[1];
   }

   __asm__ volatile(
       "vl     %%v1, %[before]\n"
       "vrepf  %%v2, %%v1, 1\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vrepf);
}

static bool test_vrepg() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 2; elem++) {
      expected.u64[elem] = before.u64[1];
   }

   __asm__ volatile(
       "vl     %%v1, %[before]\n"
       "vrepg  %%v2, %%v1, 1\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before));

   SMART_RETURN_V128(vrepg);
}

static bool test_vrepib() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 16; elem++) {
      expected.u8[elem] = ((uint8_t)-0x0fee);
   }

   __asm__ volatile(
       "vrepib  %%v2, -0x0fee\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       :);

   SMART_RETURN_V128(vrepib);
}

static bool test_vrepih() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 8; elem++) {
      expected.s16[elem] = -0x0fee;
   }

   __asm__ volatile(
       "vrepih  %%v2, -0x0fee\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       :);

   SMART_RETURN_V128(vrepih);
}

static bool test_vrepif() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 4; elem++) {
      expected.s32[elem] = -0x0fee;
   }

   __asm__ volatile(
       "vrepif  %%v2, -0x0fee\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       :);

   SMART_RETURN_V128(vrepif);
}

static bool test_vrepig() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}};

   for (elem = 0; elem < 4; elem++) {
      expected.s64[elem] = -0x0fee;
   }

   __asm__ volatile(
       "vrepig  %%v2, -0x0fee\n"
       "vst    %%v2, %[after]\n"
       : [after] "=m"(after)
       :);

   SMART_RETURN_V128(vrepig);
}

static bool test_vpksh() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.s8[outputElement] = saturated(before1.s16[inputElement], s, 8);
      outputElement++;
   }
   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.s8[outputElement] = saturated(before2.s16[inputElement], s, 8);
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpksh %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpksh);
}

static bool test_vpksf() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.s16[outputElement] = saturated(before1.s32[inputElement], s, 16);
      outputElement++;
   }
   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.s16[outputElement] = saturated(before2.s32[inputElement], s, 16);
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpksf %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpksf);
}

static bool test_vpksg() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 2; inputElement++) {
      expected.s32[outputElement] = saturated(before1.s64[inputElement], s, 32);
      outputElement++;
   }
   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.s32[outputElement] = saturated(before2.s64[inputElement], s, 32);
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpksg %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpksg);
}

static bool test_vpklsh() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.u8[outputElement] = saturated(before1.u16[inputElement], u, 8);
      outputElement++;
   }
   for (inputElement = 0; inputElement < 8; inputElement++) {
      expected.u8[outputElement] = saturated(before2.u16[inputElement], u, 8);
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpklsh %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpklsh);
}

static bool test_vpklsf() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.u16[outputElement] = saturated(before1.u32[inputElement], u, 16);
      outputElement++;
   }
   for (inputElement = 0; inputElement < 4; inputElement++) {
      expected.u16[outputElement] = saturated(before2.u32[inputElement], u, 16);
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpklsf %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpklsf);
}

static bool test_vpklsg() {
   size_t outputElement = 0;
   size_t inputElement = 0;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};

   for (inputElement = 0; inputElement < 2; inputElement++) {
      expected.u32[outputElement] = saturated(before1.u64[inputElement], u, 32);
      outputElement++;
   }
   for (inputElement = 0; inputElement < 2; inputElement++) {
      expected.u32[outputElement] = saturated(before2.u64[inputElement], u, 32);
      outputElement++;
   }

   __asm__ volatile(
       "vl %%v1, %[before1]\n"
       "vl %%v2, %[before2]\n"
       "vpklsg %%v3, %%v1, %%v2\n"
       "vst %%v3, %[after]\n"
       : [after] "=m"(after)
       : [before1] "m"(before1), [before2] "m"(before2));

   SMART_RETURN_V128_VV(vpklsg);
}

static bool test_vsel() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before1 = {.u8 = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before2 = {.u8 = {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00,
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};
   V128 pattern = {.u8 = {0x01, 0x04, 0x05, 0x06, 0x02, 0x09, 0x03, 0x07, 0x18,
                          0x1a, 0x19, 0x1b, 0x1d, 0x1c, 0x1f, 0x1e}};

   /* result = (before2 & ~pattern) | (before1 & pattern) */
   for (elem = 0; elem < 2; elem++) {
      expected.u64[elem] = (before2.u64[elem] & ~(pattern.u64[elem])) |
                           (before1.u64[elem] & pattern.u64[elem]);
   }

   __asm__ volatile(
       "vl    %%v1, %[before1]\n"
       "vl    %%v2, %[before2]\n"
       "vl    %%v3, %[pattern]\n"
       "vsel  %%v4, %%v1, %%v2, %%v3\n"
       "vst   %%v4, %[after]\n"
       : [after] "=m"(after)
       :
       [before1] "m"(before1), [before2] "m"(before2), [pattern] "m"(pattern));

   SMART_RETURN_V128_VV(vsel);
}

static bool test_vll() {
   size_t elem;
   V128 expected;
   V128 after;
   V128 before = {.u8 = {0xee, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 someData = {.u8 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

   uint32_t count = 5;
   for (elem = 0; elem < 16; elem++) {
      expected.u8[elem] = (elem <= count) ? before.u8[elem] : 0;
   }

   __asm__ volatile(
       "vl   %%v2, %[someData]\n"
       "vll  %%v2, %[count], %[before]\n"
       "vst  %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [count] "r"(count), [someData] "m"(someData));

   SMART_RETURN_V128(vll);
}

static bool test_vstl() {
   size_t elem;
   V128 expected;
   V128 before = {.u8 = {0xee, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xee}};
   V128 after = {.u8 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

   const uint32_t count = 5;
   for (elem = 0; elem < 16; elem++) {
      expected.u8[elem] = (elem <= count) ? before.u8[elem] : after.u8[elem];
   }

   __asm__ volatile(
       "vl   %%v2, %[before]\n"
       "vstl  %%v2, %[count], %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before), [count] "r"(count));

   SMART_RETURN_V128(vstl);
}

static bool test_vlbb() {
   size_t elem;
   V128 expected;
   V128 after = {.u8 = {0xee, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 before = {.u8 = {0xee, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}};
   V128 someData = {.u8 = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                           0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
   const size_t offset = 10;
   __asm__ volatile(
       "vl   %%v2, %[someData]\n"
       "vlbb  %%v2, %[before], 0\n"
       "vst  %%v2, %[after]\n"
       : [after] "=m"(after)
       : [before] "m"(before.u8[offset]), [someData] "m"(someData));

   for (elem = 0; elem < 16; elem++) {
      expected.u8[elem] =
          (elem < (16 - offset)) ? before.u8[elem + offset] : after.u8[elem];
   }

   SMART_RETURN_V128(vlbb);
}

int main() {
   test(vl_vst);
   test(vlr);

   test(vlrepg);
   test(vlrepf);
   test(vlreph);
   test(vlrepb);

   test(vle);

   test(vlei_pos);
   test(vlei_neg);

   test(vlgvb);
   test(vlgvh);
   test(vlgvf);
   test(vlgvg);

   test(vgbm);

   test(vgmb);
   test(vgmh);
   test(vgmf);
   test(vgmg);

   test(vllezb);
   test(vllezh);
   test(vllezf);
   test(vllezg);

   test(vgef);
   test(vgeg);

   test(vlm);

   test(vlvgp);

   test(vlvgb);
   test(vlvgh);
   test(vlvgf);
   test(vlvgg);

   test(vmrhb);
   test(vmrhh);
   test(vmrhf);
   test(vmrhg);

   test(vmrlb);
   test(vmrlh);
   test(vmrlf);
   test(vmrlg);

   test(vpkh);
   test(vpkf);
   test(vpkg);

   test(vperm);
   test(vpdi);

   test(vscef);
   test(vsceg);

   test(vsegb);
   test(vsegh);
   test(vsegf);

   test(vste);
   test(vstm);

   test(vuphb);
   test(vuphh);
   test(vuphf);

   test(vuplhb);
   test(vuplhh);
   test(vuplhf);

   test(vuplb);
   test(vuplhw);
   test(vuplf);

   test(vupllb);
   test(vupllh);
   test(vupllf);

   test(vrepb);
   test(vreph);
   test(vrepf);
   test(vrepg);

   test(vrepib);
   test(vrepih);
   test(vrepif);
   test(vrepig);

   test(vpksh);
   test(vpksf);
   test(vpksg);

   test(vpklsh);
   test(vpklsf);
   test(vpklsg);

   test(vsel);

   test(vll);
   test(vstl);
   test(vlbb);
   
   return 0;
}
