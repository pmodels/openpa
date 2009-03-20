/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* FIXME needs to be converted to the new OPA_int_t-style of functions */

#ifndef OPA_GCC_IA64_H_INCLUDED
#define OPA_GCC_IA64_H_INCLUDED

/* XXX DJG FIXME do we need to align these? */
typedef struct { volatile int v;    } OPA_int_t;
typedef struct { void * volatile v; } OPA_ptr_t;

/* Aligned loads and stores are atomic on ia64. */
static inline int OPA_load(OPA_int_t *ptr)
{
    return ptr->v;
}

/* Aligned loads and stores are atomic on ia64. */
static inline void OPA_store(OPA_int_t *ptr, int val)
{
    ptr->v = val;
}

/* Aligned loads and stores are atomic on ia64. */
static inline void *OPA_load_ptr(OPA_ptr_t *ptr)
{
    return ptr->v;
}

/* Aligned loads and stores are atomic on ia64. */
static inline void OPA_store_ptr(OPA_ptr_t *ptr, void *val)
{
    ptr->v = val;
}

#define OPA_add_by_faa OPA_add 
#define OPA_incr_by_faa OPA_incr 
#define OPA_decr_by_faa OPA_decr 
#define OPA_fetch_and_decr_by_faa OPA_fetch_and_decr
#define OPA_fetch_and_incr_by_faa OPA_fetch_and_incr

static inline int OPA_decr_and_test(OPA_int_t *ptr)
{
    int val;
    __asm__ __volatile__ ("fetchadd4.rel %0=[%2],%3"
                          : "=r"(val), "=m"(ptr->v)
                          : "r"(&ptr->v), "i"(-1));
    return val == 1;
}

/* IA64 has a fetch-and-add instruction that only accepts immediate
   values of -16, -8, -4, -1, 1, 4, 8, and 16.  So we check for these
   values before falling back to the CAS implementation. */
#define IA64_FAA_CASE_MACRO(ptr, val) case val: {       \
    int prev;                                           \
    __asm__ __volatile__ ("fetchadd4.rel %0=[%2],%3"    \
                          : "=r"(prev), "=m"(ptr->v)    \
                          : "r"(&ptr->v), "i"(val));    \
    return prev;                                        \
    }                                                   \
    break


static inline int OPA_fetch_and_add(OPA_int_t *ptr, int val)
{
    switch (val)
    {
        IA64_FAA_CASE_MACRO(ptr, -16);
        IA64_FAA_CASE_MACRO(ptr,  -8);
        IA64_FAA_CASE_MACRO(ptr,  -4);
        IA64_FAA_CASE_MACRO(ptr,  -1);
        IA64_FAA_CASE_MACRO(ptr,   1);
        IA64_FAA_CASE_MACRO(ptr,   4);
        IA64_FAA_CASE_MACRO(ptr,   8);
        IA64_FAA_CASE_MACRO(ptr,  16);
    default:
        return OPA_fetch_and_add_by_cas(ptr, val);
    }
}
#undef IA64_FAA_CASE_MACRO


static inline void *OPA_cas_ptr(OPA_ptr_t *ptr, void *oldv, void *newv)
{
    void *prev;
    __asm__ __volatile__ ("mov ar.ccv=%1;;"
                          "cmpxchg8.rel %0=[%3],%4,ar.ccv"
                          : "=r"(prev), "=m"(ptr->v)
                          : "rO"(oldv), "r"(&ptr->v), "r"(newv));
    return prev;   
}

static inline int OPA_cas_int(OPA_int_t *ptr, int oldv, int newv)
{
    int prev;

    switch (sizeof(int)) /* this switch statement should be optimized out */
    {
    case 8:
        __asm__ __volatile__ ("mov ar.ccv=%1;;"
                              "cmpxchg8.rel %0=[%3],%4,ar.ccv"
                              : "=r"(prev), "=m"(ptr->v)
                              : "rO"(oldv), "r"(&ptr->v), "r"(newv)
                              : "memory");
        break;
    case 4:
        __asm__ __volatile__ ("zxt4 %1=%1;;" /* don't want oldv sign-extended to 64 bits */
                              "mov ar.ccv=%1;;"
                              "cmpxchg4.rel %0=[%3],%4,ar.ccv"
                              : "=r"(prev), "=m"(ptr->v)
                              : "r0"(oldv), "r"(&ptr->v), "r"(newv)
                              : "memory");
        break;
    default:
        MPIU_Assertp (0);
    }
    
    return prev;   
}

static inline void *OPA_swap_ptr(OPA_ptr_t *ptr, void *val)
{
    /* is pointer swizzling necessary here? */
    __asm__ __volatile__ ("xchg8 %0=[%2],%3"
                          : "=r" (val), "=m" (*val)
                          : "r" (&ptr->v), "0" (val));
    return val;
}


static inline int OPA_swap_int(OPA_int_t *ptr, int val)
{
    /* is pointer swizzling necessary here? */
    __asm__ __volatile__ ("xchg8 %0=[%2],%3"
                          : "=r" (val), "=m" (*val)
                          : "r" (&ptr->v), "0" (val));
    return val;
}

#include "opa_emulated.h"

#endif /* OPA_GCC_IA64_H_INCLUDED */
