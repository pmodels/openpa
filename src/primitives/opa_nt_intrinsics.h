/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/* FIXME needs to be converted to the new OPA_int_t-style of functions */

#ifndef OPA_NT_INTRINSICS_H_INCLUDED
#define OPA_NT_INTRINSICS_H_INCLUDED

#include<intrin.h>
#include "mpi.h"

typedef struct { volatile int v;  } OPA_int_t;
typedef struct { int * volatile v; } OPA_ptr_t;

static inline int OPA_load(OPA_int_t *ptr)
{
    return ptr->v;
}

static inline void OPA_store(OPA_int_t *ptr, int val)
{
    ptr->v = val;
}

static inline void *OPA_load_ptr(OPA_ptr_t *ptr)
{
    return ptr->v;
}

static inline void OPA_store_ptr(OPA_ptr_t *ptr, void *val)
{
    ptr->v = val;
}

static inline void OPA_add(volatile int *ptr, int val)
{
    _InterlockedExchangeAdd(ptr, val);
}

static inline void OPA_incr(volatile int *ptr)
{
    _InterlockedIncrement(ptr);
}

static inline void OPA_decr(volatile int *ptr)
{
    _InterlockedDecrement(ptr);
}

static inline int OPA_decr_and_test(volatile int *ptr)
{
    return (_InterlockedDecrement(ptr) == 0);
}

static inline int OPA_fetch_and_add(volatile int *ptr, int val)
{
    return _InterlockedExchangeAdd(ptr, val);
}

static inline int *OPA_cas_int_ptr(int * volatile *ptr, int *oldv, int *newv)
{
    /* FIXME OPA_SIZEOF_VOID_P is defined after configure time, but during
     * configure (where this header is also used) it is only called
     * SIZEOF_VOID_P.  We need a general solution to this problem, since it
     * shows up in other files too.  Punting for the moment since the NT
     * intrinsics doesn't even compile right now, but this is something we need
     * to fix.
     *
     * possibly just something like?:
     *   #if !defined(OPA_SIZEOF_VOID_P) && defined(SIZEOF_VOID_P)
     *   #  define OPA_SIZEOF_VOID_P SIZEOF_VOID_P
     *   #endif
     */
#if (OPA_SIZEOF_VOID_P == 4)
    return _InterlockedCompareExchange(ptr, newv, oldv);
#elif (OPA_SIZEOF_VOID_P == 8)
    return _InterlockedCompareExchange64(ptr, newv, oldv);
#else
#error  "OPA_SIZEOF_VOID_P not valid"
#endif
}

static inline int OPA_cas_int(volatile int *ptr, int oldv, int newv)
{
    return _InterlockedCompareExchange(ptr, newv, oldv);
}

static inline int *OPA_swap_int_ptr(int * volatile *ptr, int *val)
{
#if (OPA_SIZEOF_VOID_P == 4)
    return _InterlockedExchange(ptr, val);
#elif (OPA_SIZEOF_VOID_P == 8)
    return _InterlockedExchange64(ptr, val);
#else
#error  "OPA_SIZEOF_VOID_P not valid"
#endif
}

static inline int OPA_swap_int(volatile int *ptr, int val)
{
    return _InterlockedExchange(ptr, val);
}

/* Implement fetch_and_incr/decr using fetch_and_add (*_faa) */
#define OPA_fetch_and_incr OPA_fetch_and_incr_by_faa
#define OPA_fetch_and_decr OPA_fetch_and_decr_by_faa

#define OPA_write_barrier()      _WriteBarrier()
#define OPA_read_barrier()       _ReadBarrier()
#define OPA_read_write_barrier() _ReadWriteBarrier()

#include "opa_emulated.h"

#endif /* defined(OPA_NT_INTRINSICS_H_INCLUDED) */
