/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* Atomic primitives for SiCortex machines.
 * Originally contributed by Lawrence Stewart at SiCortex.
 */

#ifndef OPA_GCC_SICORTEX_H
#define OPA_GCC_SICORTEX_H

#define OPA_UNIVERSAL_PRIMITIVE OPA_LL_SC

typedef struct { volatile int v;  } OPA_int_t;
typedef struct { int * volatile v; } OPA_ptr_t;

#include <stdint.h>

/* ICE9 rev A1 chips have a low-frequency bug that causes LL to
   fail. The workaround is to do the LL twice to make sure the data
   is in L1
  
   very few systems are affected

   FIXME We should either remove the workaround entirely or make it
   configurable/autodetected somehow. [goodell@ 2008/01/06]
 */
#define ICE9A_LLSC_WAR 0

/* For a description of the inline assembly constraints, see the MIPS section of:
   http://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Machine-Constraints.html#Machine-Constraints

   relevant excerpt:
       I - A signed 16-bit constant (for arithmetic instructions).
       J - Integer zero. 

   Other inline asm knowledge worth remembering:
       r - a general register operand is allowed
       m - a memory address operand
       & - earlyclobber; operand is modified before instruction is finished using the input operands
       = - this operand is write-only

   general format:
       asm volatile ("instructions" : [outputs] : [inputs] : [clobbered_regs]);
       "memory" should be included as a clobbered reg for most of these operations.
 */

/* Atomic increment of a 32 bit value, returning the old value */
static __inline__ int shmemi_fetch_add_4(volatile int * v, int inc)
{
        unsigned long result;
        if (ICE9A_LLSC_WAR) {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     ll      %0, %2          # fetch_add_4           \n"
                "       ll      %0, %2          # fetch_add_4           \n"
                "       addu    %1, %0, %3                              \n"
                "       sc      %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "Ir" (inc)
                : "memory");
        } else {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     ll      %0, %2          # fetch_add_4           \n"
                "       addu    %1, %0, %3                              \n"
                "       sc      %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "Ir" (inc)
                : "memory");
        }

        return result;
}

/* Atomic increment of a 64 bit value, returning the old value */
static __inline__ long int shmemi_fetch_add_8(volatile long int * v, long int inc)
{
        unsigned long result;
        if (ICE9A_LLSC_WAR) {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     lld     %0, %2          # fetch_add_8           \n"
                "       lld     %0, %2          # fetch_add_8           \n"
                "       daddu   %1, %0, %3                              \n"
                "       scd     %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "Ir" (inc)
                : "memory");
        } else {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     lld     %0, %2          # fetch_add_8           \n"
                "       daddu   %1, %0, %3                              \n"
                "       scd     %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "Ir" (inc)
                : "memory");
        }

        return result;
}

/* Atomic swap of a 32 bit value, returning the old contents */
static __inline__ int shmemi_swap_4(volatile int * v, int new)
{
        unsigned long result;
        if (ICE9A_LLSC_WAR) {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     ll      %0, %2          # swap_4                \n"
                "       ll      %0, %2          # swap_4                \n"
                "       move    %1, %3                                  \n"
                "       sc      %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new)
                : "memory");
        } else {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     ll      %0, %2          # swap_4                \n"
                "       move    %1, %3                                  \n"
                "       sc      %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new)
                : "memory");
        }

        return result;
}

/* Atomic swap of a 64 bit value, returning the old contents */
static __inline__ long int shmemi_swap_8(volatile long int * v, long int new)
{
        unsigned long result;
        if (ICE9A_LLSC_WAR) {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     lld     %0, %2          # swap_8                \n"
                "       lld     %0, %2          # swap_8                \n"
                "       move    %1, %3                                  \n"
                "       scd     %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new)
                : "memory");
        } else {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     lld     %0, %2          # swap_8                \n"
                "       move    %1, %3                                  \n"
                "       scd     %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new)
                : "memory");
        }

        return result;
}

/* Atomic compare and swap of a 32 bit value, returns the old value
 * but only does the store of the new value if the old value == expect */
static __inline__ int shmemi_cswap_4(volatile int * v, int expect, int new)
{
        unsigned long result;
        if (ICE9A_LLSC_WAR) {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     ll      %0, %2          # cswap_4               \n"
                "       ll      %0, %2          # cswap_4               \n"
                "       bne     %0, %4, 1f                              \n"
                "       move    %1, %3                                  \n"
                "       sc      %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "1:                                                     \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new), "Jr" (expect)
                : "memory");
        } else {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     ll      %0, %2          # cswap_4               \n"
                "       bne     %0, %4, 1f                              \n"
                "       move    %1, %3                                  \n"
                "       sc      %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "1:                                                     \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new), "Jr" (expect)
                : "memory");
        }

        return result;
}

/* Atomic compare and swap of a 64 bit value, returns the old value
 * but only does the store of the new value if the old value == expect */
static __inline__ long int shmemi_cswap_8(volatile long int * v, long int expect, long int new)
{
        unsigned long result;
        if (ICE9A_LLSC_WAR) {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     lld     %0, %2          # cswap_8               \n"
                "       lld     %0, %2          # cswap_8               \n"
                "       bne     %0, %4, 1f                              \n"
                "       move    %1, %3                                  \n"
                "       scd     %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "1:                                                     \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new), "Jr" (expect)
                : "memory");
        } else {
                unsigned long temp;

                __asm__ __volatile__(
                "       .set    mips3                                   \n"
                "       .set    noreorder                               \n"
                "1:     lld     %0, %2          # cswap_8               \n"
                "       bne     %0, %4, 1f                              \n"
                "       move    %1, %3                                  \n"
                "       scd     %1, %2                                  \n"
                "       beqz    %1, 1b                                  \n"
                "        nop                                            \n"
                "       .set    reorder                                 \n"
                "1:                                                     \n"
                "       .set    mips0                                   \n"
                : "=&r" (result), "=&r" (temp), "=m" (*v)
                : "r" (new), "Jr" (expect)
                : "memory");
        }

        return result;
}

#if !defined(_MIPS_SZPTR)
#  error "_MIPS_SZPTR must be defined!"
#endif


static __inline__ void OPA_add(OPA_int_t *ptr, int val)
{
    shmemi_fetch_add_4(ptr, val);
}

static __inline__ void *OPA_cas_ptr(OPA_ptr_t *ptr, void *oldv, void *newv)
{
#if (_MIPS_SZPTR == 64)
    return((int *) shmemi_cswap_8((volatile long int *) &ptr->v, (uintptr_t) oldv, (uintptr_t) newv));
#else
    return((int *) shmemi_cswap_4((volatile int *) &ptr->v, (uintptr_t) oldv, (uintptr_t) newv));
#endif
}

static __inline__ int OPA_cas_int(OPA_int_t *ptr, int oldv, int newv)
{
    return(shmemi_cswap_4(&ptr->v, oldv, newv));
}

static __inline__ void OPA_decr(OPA_int_t *ptr)
{
    shmemi_fetch_add_4(&ptr->v, -1);
}


static __inline__ int OPA_decr_and_test(OPA_int_t *ptr)
{
    int old = shmemi_fetch_add_4(&ptr->v, -1);
    return (old == 1);
}

static __inline__ int OPA_fetch_and_add(OPA_int_t *ptr, int val)
{
    return(shmemi_fetch_add(&ptr->v, val));
}

static __inline__ int OPA_fetch_and_decr(OPA_int_t *ptr)
{
    return(shmemi_fetch_add_4(&ptr->v, -1));
}

static __inline__ int OPA_fetch_and_incr(OPA_int_t *ptr)
{
    return(shmemi_fetch_add_4(&ptr->v, 1));
}

static __inline__ void OPA_incr(OPA_int_t *ptr)
{
    shmemi_fetch_add_4(&ptr->v, 1);
}

static __inline__ int *OPA_swap_ptr(OPA_ptr_t *ptr, int *val)
{
#if (_MIPS_SZPTR == 64)
    return((int *) shmemi_swap_8((long int *) &ptr->v, (uintptr_t) val));
#else
    return((int *) shmemi_swap_4((int *) &ptr->v, (uintptr_t) val));
#endif
}

static __inline__ int OPA_swap_int(OPA_int_t *ptr, int val)
{
    return(shmemi_swap_4(&ptr->v, val));
}

#endif /* OPA_GCC_SICORTEX_H */
