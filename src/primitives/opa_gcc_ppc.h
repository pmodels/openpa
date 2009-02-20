/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* FIXME needs to be converted to the new OPA_int_t-style of functions */

#ifndef OPA_GCC_PPC_H_INCLUDED
#define OPA_GCC_PPC_H_INCLUDED

/*
   load-link/store-conditional (LL/SC) primitives.  We LL/SC implement
   these here, which are arch-specific, then use the generic
   implementations from _atomic_emulated.h */

static inline int OPA_LL_int(volatile int *ptr)
{
    int val;
    __asm__ __volatile__ ("lwarx %[val],0,%[ptr]"
                          : [val] "=r" (val)
                          : [ptr] "r" (ptr)
                          : "cc");

    return val;
}

/* Returns non-zero if the store was successful, zero otherwise. */
static inline int OPA_SC_int(volatile int *ptr, int val)
{
    int ret;
    __asm__ __volatile__ ("stwcx. %[val],0,%[ptr];\n"
                          "beq 1f;\n"
                          "li %[ret], 0;\n"
                          "1: ;\n"
                          : [ret] "=r" (ret)
                          : [ptr] "r" (ptr), [val] "r" (val), "0" (ret)
                          : "cc", "memory");
    return ret;
}


/*
   Pointer versions of LL/SC.  For now we implement them in terms of the integer
   versions with some casting.  If/when we encounter a platform with LL/SC and
   differing word and pointer widths we can write separate versions.
*/

static inline int *OPA_LL_int_ptr(volatile int **ptr)
{
    switch (sizeof(int *)) { /* should get optimized away */
        case sizeof(int):
            return OPA_LL_int((volatile int)ptr);
            break;
        default:
            MPIU_Assertp(0); /* need to implement a separate ptr-sized version */
            return NULL; /* placate the compiler */
    }
}

/* Returns non-zero if the store was successful, zero otherwise. */
static inline int OPA_SC_int_ptr(volatile int **ptr, int *val)
{
    switch (sizeof(int *)) { /* should get optimized away */
        case sizeof(int):
            return OPA_SC_int((volatile int *)ptr, (int)val);
            break;
        default:
            MPIU_Assertp(0); /* need to implement a separate ptr-sized version */
            return NULL; /* placate the compiler */
    }
}

/* Implement all function using LL/SC */

#define OPA_add_by_llsc            OPA_add
#define OPA_incr_by_llsc           OPA_incr
#define OPA_decr_by_llsc           OPA_decr
#define OPA_decr_and_test_by_llsc  OPA_decr_and_test
#define OPA_fetch_and_add_by_llsc  OPA_fetch_and_add
#define OPA_fetch_and_decr_by_llsc OPA_fetch_and_decr
#define OPA_fetch_and_incr_by_llsc OPA_fetch_and_incr
#define OPA_cas_int_ptr_by_llsc    OPA_cas_int_ptr
#define OPA_cas_int_by_llsc        OPA_cas_int
#define OPA_swap_int_ptr_by_llsc   OPA_swap_int_ptr
#define OPA_swap_int_by_llsc       OPA_swap_int

#include"mpidu_atomic_emulated.h"

#endif /* OPA_GCC_PPC_H_INCLUDED */
