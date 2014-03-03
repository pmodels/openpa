/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* FIXME needs to be converted to new style functions with OPA_int_t/OPA_ptr_t-types */
#ifndef OPA_C11_ATOMICS_H_INCLUDED
#define OPA_C11_ATOMICS_H_INCLUDED

#ifdef __STDC_NO_ATOMICS__
#error stdatomic header is not available.
#else
#include <stdatomic.h>
#endif

#ifndef ATOMIC_INT_LOCK_FREE
#error atomic_int is not lock-free.
#else
typedef atomic_int OPA_int_t;
#endif

#ifndef ATOMIC_POINTER_LOCK_FREE
#error atomic_pointer is not lock-free.
#else
/* TODO Is this really the right type to use here? */
typedef atomic_intptr_t OPA_ptr_t;
#endif

#define OPA_INT_T_INITIALIZER(val_) ATOMIC_VAR_INIT(val_)
#define OPA_PTR_T_INITIALIZER(val_) ATOMIC_VAR_INIT(val_)

static _opa_inline int OPA_load_int(_opa_const OPA_int_t *ptr)
{
    return atomic_load_explicit(ptr, memory_order_relaxed);
}

static _opa_inline void OPA_store_int(OPA_int_t *ptr, int val)
{
    atomic_store_explicit(ptr, val, memory_order_relaxed);
}

static _opa_inline void *OPA_load_ptr(_opa_const OPA_ptr_t *ptr)
{
    return atomic_load_explicit(ptr, memory_order_relaxed);
}

static _opa_inline void OPA_store_ptr(OPA_ptr_t *ptr, void *val)
{
    atomic_store_explicit(ptr, val, memory_order_relaxed);
}

static _opa_inline int OPA_load_acquire_int(_opa_const OPA_int_t *ptr)
{
    return atomic_load_explicit(ptr, memory_order_acquire);
}

static _opa_inline void OPA_store_release_int(OPA_int_t *ptr, int val)
{
    atomic_store_explicit(ptr, val, memory_order_release);
}

static _opa_inline void *OPA_load_acquire_ptr(_opa_const OPA_ptr_t *ptr)
{
    return atomic_load_explicit(ptr, memory_order_acquire);
}

static _opa_inline void OPA_store_release_ptr(OPA_ptr_t *ptr, void *val)
{
    atomic_store_explicit(ptr, val, memory_order_release);
}

static _opa_inline int OPA_fetch_and_add_int(OPA_int_t *ptr, int val)
{
    return atomic_fetch_add_explicit(ptr, val, memory_order_relaxed);
}

static _opa_inline int OPA_decr_and_test_int(OPA_int_t *ptr)
{
    return (1 == atomic_fetch_add_explicit(ptr, -1, memory_order_relaxed));
}

#define OPA_fetch_and_incr_int_by_faa OPA_fetch_and_incr_int
#define OPA_fetch_and_decr_int_by_faa OPA_fetch_and_decr_int
#define OPA_add_int_by_faa OPA_add_int
#define OPA_incr_int_by_fai OPA_incr_int
#define OPA_decr_int_by_fad OPA_decr_int


static _opa_inline void *OPA_cas_ptr(OPA_ptr_t *ptr, void *oldv, void *newv)
{
    return __sync_val_compare_and_swap(&ptr->v, oldv, newv);
}

static _opa_inline int OPA_cas_int(OPA_int_t *ptr, int oldv, int newv)
{
    return __sync_val_compare_and_swap(&ptr->v, oldv, newv);
}

#ifdef SYNC_LOCK_TEST_AND_SET_IS_SWAP
static _opa_inline void *OPA_swap_ptr(OPA_ptr_t *ptr, void *val)
{
    return __sync_lock_test_and_set(&ptr->v, val);
}

static _opa_inline int OPA_swap_int(OPA_int_t *ptr, int val)
{
    return __sync_lock_test_and_set(&ptr->v, val);
}

#else
#define OPA_swap_ptr_by_cas OPA_swap_ptr
#define OPA_swap_int_by_cas OPA_swap_int 
#endif

#define OPA_write_barrier()      __sync_synchronize()
#define OPA_read_barrier()       __sync_synchronize()
#define OPA_read_write_barrier() __sync_synchronize()
#define OPA_compiler_barrier()   __asm__ __volatile__  ( ""  ::: "memory" )



#include"opa_emulated.h"

#endif /* OPA_C11_ATOMICS_H_INCLUDED */
