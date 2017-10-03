#include "test-s390x-vector.h"

#define s390_sum(a, b) ((a) + (b))
#define s390_substract(a, b) ((a) - (b))
#define s390_multiply_s(a, b) ((int64_t)(a) * (b))
#define s390_multiply_u(a, b) ((uint64_t)(a) * (b))
#define s390_multiply_low(a, b) ((a) * (b))
#define s390_multiply_high_u(a, b) \
   (((uint64_t)(a) * (b)) >> (8 * s390_currentSize))
#define s390_multiply_high_s(a, b) \
   (((int64_t)(a) * (b)) >> (8 * s390_currentSize))
#define s390_and(a, b) ((a) & (b))
#define s390_and_with_completement(a, b) ((a) & (~(b)))
#define s390_xor(a, b) ((a) ^ (b))
#define s390_nor(a, b) (~((a) | (b)))
#define s390_or(a, b) ((a) | (b))
#define s390_avg(a, b) ((a >> 1) + (b >> 1) + (((a & 1) + (b & 1)) >> 1))
#define s390_max(a, b) (((a) >= (b)) ? (a) : (b))
#define s390_min(a, b) (((a) <= (b)) ? (a) : (b))
#define s390_completement(a) (~(a) + 1)
#define s390_abs(a) ((a < 0) ? -a : a)
#define s390_compare_high(a, b) (((a) > (b)) ? 0xffffffffffffffffULL : 0)

/* Contains size of argument for functions in bytes */
static size_t s390_currentSize = 0;

uint64_t s390_trailing_zeros(uint64_t n) {
   if (n == 0) {
      return s390_currentSize * 8;
   }
   unsigned count = 0;
   while ((n % 2) == 0) {
      count++;
      n >>= 1;
   }
   return count;
}

uint64_t s390_leading_zeros(uint64_t n) {
   if (n == 0) {
      return s390_currentSize * 8;
   }
   unsigned count = 0;
   while ((n & (1ULL << (8 * s390_currentSize - 1))) == 0) {
      count++;
      n <<= 1;
   }
   return count;
}

uint8_t s390_count_ones(const uint8_t elem) {
   uint8_t result = 0;
   uint8_t bit;

   for (bit = 0; bit < 8; bit++) {
      if ((elem & (1 << bit)) != 0) {
         result++;
      }
   }

   return result;
}

/* clang-format off */
declare_vv_binary_all_signed_with_q(va, s390_sum)
declare_vv_binary_all_signed_with_q(vs, s390_substract)
declare_vv_binary(vn, , u, s390_and, full, same)
declare_vv_binary(vnc, , u, s390_and_with_completement, full, same)
declare_vv_binary(vx, , u, s390_xor, full, same)
declare_vv_binary(vno, , u, s390_nor, full, same)
declare_vv_binary(vo, , u, s390_or, full, same)
declare_vv_binary_all_signed_without_q(vavg, s390_avg)
declare_vv_binary_all_unsigned_without_q(vavgl, s390_avg)
declare_vv_binary_all_signed_without_q(vmx, s390_max)
declare_vv_binary_all_unsigned_without_q(vmxl, s390_max)
declare_vv_binary_all_signed_without_q(vmn, s390_min)
declare_vv_binary_all_unsigned_without_q(vmnl, s390_min)
declare_vv_binary_all_signed_without_q(vch, s390_compare_high)
declare_vv_binary_all_unsigned_without_q(vchl, s390_compare_high)
declare_vv_unary_all_signed_without_q(vlc, s390_completement)
declare_vv_unary_all_signed_without_q(vlp, s390_abs)
declare_vv_unary_all_unsigned_without_q(vctz, s390_trailing_zeros)
declare_vv_unary_all_unsigned_without_q(vclz, s390_leading_zeros)
declare_vv_unary(vpopct,b , u, s390_count_ones)

declare_vv_binary(vml,b , s, s390_multiply_low, full, same)
declare_vv_binary(vml,hw , s, s390_multiply_low, full, same)
declare_vv_binary(vml,f , s, s390_multiply_low, full, same)

declare_vv_binary_all_signed_without_qg(vmh, s390_multiply_high_s)
declare_vv_binary_all_unsigned_without_qg(vmlh, s390_multiply_high_u)

declare_vv_binary(vmle,b , u, s390_multiply_u, even, wider)
declare_vv_binary(vmle,h , u, s390_multiply_u, even, wider)
declare_vv_binary(vmle,f , u, s390_multiply_u, even, wider)
declare_vv_binary(vme,b , s, s390_multiply_s, even, wider)
declare_vv_binary(vme,h , s, s390_multiply_s, even , wider)
declare_vv_binary(vme,f , s, s390_multiply_s, even, wider)

declare_vv_binary(vmlo,b , u, s390_multiply_u, odd, wider)
declare_vv_binary(vmlo,h , u, s390_multiply_u, odd, wider)
declare_vv_binary(vmlo,f , u, s390_multiply_u, odd, wider)
declare_vv_binary(vmo,b , s, s390_multiply_s, odd, wider)
declare_vv_binary(vmo,h , s, s390_multiply_s, odd, wider)
declare_vv_binary(vmo,f , s, s390_multiply_s, odd, wider)
/* clang-format on */

int main() {
   test(vab);
   test(vah);
   test(vaf);
   test(vag);
   test(vaq);

   test(vsb);
   test(vsh);
   test(vsf);
   test(vsg);
   test(vsq);

   test(vn);
   test(vnc);
   test(vx);
   test(vno);
   test(vo);

   test(vavgb);
   test(vavgh);
   test(vavgf);
   test(vavgg);
   test(vavglb);
   test(vavglh);
   test(vavglf);
   test(vavglg);

   test(vmxb);
   test(vmxh);
   test(vmxf);
   test(vmxg);
   test(vmnb);
   test(vmnh);
   test(vmnf);
   test(vmng);

   test(vlcb);
   test(vlch);
   test(vlcf);
   test(vlcg);

   test(vlpb);
   test(vlph);
   test(vlpf);
   test(vlpg);

   test(vchb);
   test(vchh);
   test(vchf);
   test(vchg);

   test(vchlb);
   test(vchlh);
   test(vchlf);
   test(vchlg);

   s390_currentSize = 1;
   test(vctzb);
   test(vclzb);
   test(vmhb);
   test(vmlhb);
   s390_currentSize = 2;
   test(vctzh);
   test(vclzh);
   test(vmhh);
   test(vmlhh);
   s390_currentSize = 4;
   test(vctzf);
   test(vclzf);
   test(vmhf);
   test(vmlhf);
   s390_currentSize = 8;
   test(vctzg);
   test(vclzg);

   test(vpopctb);

   test(vmlb);
   test(vmlhw);
   test(vmlf);

   test(vmleb);
   test(vmleh);
   test(vmlef);
   test(vmeb);
   test(vmeh);
   test(vmef);

   test(vmlob);
   test(vmloh);
   test(vmlof);
   test(vmob);
   test(vmoh);
   test(vmof);

   return 0;
}
