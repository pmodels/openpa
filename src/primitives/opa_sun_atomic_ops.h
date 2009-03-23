/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* FIXME needs to be converted to the new OPA_int_t-style of functions */

#ifndef OPA_SUN_ATOMIC_OPS_H_INCLUDED
#define OPA_SUN_ATOMIC_OPS_H_INCLUDED

#include <atomic.h>

static inline void OPA_add(volatile int *ptr, int val)
{
    atomic_add_int((volatile uint_t *)ptr, val);
}

static inline void OPA_incr(volatile int *ptr)
{
    atomic_inc_uint((volatile uint_t *)ptr);
}

static inline void OPA_decr(volatile int *ptr)
{
    atomic_dec_uint((volatile uint_t *)ptr);
}


static inline int OPA_decr_and_test(volatile int *ptr)
{
    return atomic_dec_uint_nv((volatile uint_t *)ptr) == 0;    
}


static inline int OPA_fetch_and_add(volatile int *ptr, int val)
{
    return (int)atomic_add_int_nv((volatile uint_t *)ptr, val) - val;
}

static inline int OPA_fetch_and_decr(volatile int *ptr)
{
    return (int)atomic_dec_uint_nv((volatile uint_t *)ptr) + 1;
}

static inline int OPA_fetch_and_incr(volatile int *ptr)
{
    return (int)atomic_inc_uint_nv((volatile uint_t *)ptr) - 1;
}


static inline int *OPA_cas_int_ptr(int * volatile *ptr, int *oldv, int *newv)
{
    return atomic_cas_ptr(ptr, oldv, newv);
}

static inline int OPA_cas_int(volatile int *ptr, int oldv, int newv)
{
    return (int)atomic_cas_uint((volatile uint_t *)ptr, (uint_t)oldv, (uint_t)newv);
}

static inline int *OPA_swap_int_ptr(int * volatile *ptr, int *val)
{
    return atomic_swap_ptr(ptr, val);
}

static inline int OPA_swap_int(volatile int *ptr, int val)
{
    return (int)atomic_swap_uint((volatile uint_t *)ptr, (uint_t) val);
}

#define OPA_write_barrier()      membar_producer()
#define OPA_read_barrier()       membar_consumer()
#define OPA_read_write_barrier() do { membar_consumer(); membar_producer(); } while (0)


#endif /* OPA_SUN_ATOMIC_OPS_H_INCLUDED */
