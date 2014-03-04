/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *  (C) 2014 by Argonne National Laboratory, Leadership Computing Facility.
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
#include <stdint.h>
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
    return (void*)atomic_load_explicit(ptr, memory_order_relaxed);
}

static _opa_inline void OPA_store_ptr(OPA_ptr_t *ptr, void *val)
{
    atomic_store_explicit(ptr, (intptr_t)val, memory_order_relaxed);
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
    return (void*)atomic_load_explicit(ptr, memory_order_acquire);
}

static _opa_inline void OPA_store_release_ptr(OPA_ptr_t *ptr, void *val)
{
    atomic_store_explicit(ptr, (intptr_t)val, memory_order_release);
}

static _opa_inline void OPA_add_int(OPA_int_t *ptr, int val)
{
    atomic_fetch_add_explicit(ptr, val, memory_order_relaxed);
}

static _opa_inline int OPA_fetch_and_add_int(OPA_int_t *ptr, int val)
{
    return atomic_fetch_add_explicit(ptr, val, memory_order_relaxed);
}

static _opa_inline int OPA_fetch_and_decr_int(OPA_int_t *ptr)
{
    return atomic_fetch_add_explicit(ptr, -1, memory_order_relaxed);
}

static _opa_inline int OPA_fetch_and_incr_int(OPA_int_t *ptr)
{
    return atomic_fetch_add_explicit(ptr, 1, memory_order_relaxed);
}

static _opa_inline void OPA_incr_int(OPA_int_t *ptr)
{
    atomic_fetch_add_explicit(ptr, 1, memory_order_relaxed);
}

static _opa_inline void OPA_decr_int(OPA_int_t *ptr)
{
    atomic_fetch_add_explicit(ptr, -1, memory_order_relaxed);
}

static _opa_inline int OPA_decr_and_test_int(OPA_int_t *ptr)
{
    return (1 == atomic_fetch_add_explicit(ptr, -1, memory_order_relaxed));
}

/* Dave Goodell says weak is fine.  OpenPA does not expect strong. */

static _opa_inline void *OPA_cas_ptr(OPA_ptr_t *ptr, void *oldv, void *newv)
{
    /* FIXME return value */
    return atomic_compare_exchange_weak_explicit(ptr, (intptr_t*)&oldv, (intptr_t)newv, memory_order_relaxed, memory_order_relaxed);
}

static _opa_inline int OPA_cas_int(OPA_int_t *ptr, int oldv, int newv)
{
    /* FIXME return value */
    return atomic_compare_exchange_weak_explicit(ptr, &oldv, newv, memory_order_relaxed, memory_order_relaxed);
}

static _opa_inline void *OPA_swap_ptr(OPA_ptr_t *ptr, void *val)
{
    return (void*)atomic_exchange_explicit(ptr, (intptr_t)val, memory_order_relaxed);
}

static _opa_inline int OPA_swap_int(OPA_int_t *ptr, int val)
{
    return atomic_exchange_explicit(ptr, val, memory_order_relaxed);
}

/* Dave says that read/write don't match acq/rel perfectly so we use heavy hammer. */
#define OPA_write_barrier()      atomic_thread_fence(memory_order_acq_rel);
#define OPA_read_barrier()       atomic_thread_fence(memory_order_acq_rel);
#define OPA_read_write_barrier() atomic_thread_fence(memory_order_acq_rel);
/* FYI: According to C11 7.17.4.2 NOTE 2, atomic_signal_fence acts as
 *      a compiler barrier.  Note sure how memory_order affects this. */
#define OPA_compiler_barrier()   atomic_signal_fence(memory_order_acq_rel);

#endif /* OPA_C11_ATOMICS_H_INCLUDED */
