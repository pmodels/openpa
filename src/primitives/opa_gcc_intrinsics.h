/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* FIXME needs to be converted to new style functions with OPA_t/OPA_ptr_t-types */
#ifndef OPA_GCC_INTRINSICS_H_INCLUDED
#define OPA_GCC_INTRINSICS_H_INCLUDED

/* gcc atomic intrinsics accept an optional list of variables to be
   protected by a memory barrier.  These variables are labeled
   below by "protected variables :". */

static inline int OPA_fetch_and_add(volatile int *ptr, int val)
{
    return __sync_fetch_and_add(ptr, val, /* protected variables: */ ptr);
}

static inline int OPA_decr_and_test(volatile int *ptr)
{
    return __sync_sub_and_fetch(ptr, 1, /* protected variables: */ ptr) == 0;
}

#define OPA_fetch_and_incr_by_faa OPA_fetch_and_incr 
#define OPA_fetch_and_decr_by_faa OPA_fetch_and_decr 
#define OPA_add_by_faa OPA_add 
#define OPA_incr_by_fai OPA_incr 
#define OPA_decr_by_fad OPA_decr 


static inline int *OPA_cas_int_ptr(int * volatile *ptr, int *oldv, int *newv)
{
    return __sync_val_compare_and_swap(ptr, oldv, newv, /* protected variables: */ ptr);
}

static inline int OPA_cas_int(volatile int *ptr, int oldv, int newv)
{
    return __sync_val_compare_and_swap(ptr, oldv, newv, /* protected variables: */ ptr);
}

#ifdef SYNC_LOCK_TEST_AND_SET_IS_SWAP
static inline int *OPA_swap_int_ptr(int * volatile *ptr, int *val)
{
    return __sync_lock_test_and_set(ptr, val, /* protected variables: */ ptr);
}

static inline int OPA_swap_int(volatile int *ptr, int val)
{
    return __sync_lock_test_and_set(ptr, val, /* protected variables: */ ptr);
}

#else
#define OPA_swap_int_ptr_by_cas OPA_swap_int_ptr 
#define OPA_swap_int_by_cas OPA_swap_int 
#endif

#include"opa_emulated.h"

#endif /* OPA_GCC_INTRINSICS_H_INCLUDED */
