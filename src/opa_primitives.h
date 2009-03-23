/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef OPA_PRIMITIVES_H_INCLUDED
#define OPA_PRIMITIVES_H_INCLUDED

#include "opa_config.h"
#include "opa_util.h"

/*
   Primitive atomic functions
   --------------------------

   The included file is responsible for defining the types of OPA_int_t and
   OPA_ptr_t as well as a set of functions for operating on these
   types.  If you have the following declaration:

       OPA_int_t atomic_var;

   Then in order for the emulation functions compile the underlying value of
   atomic_var should be accessible via:

       atomic_var.v;

   The same goes for OPA_ptr_t.
   
   The atomic functions that must be ported for each architecture: 

   static inline int   OPA_load(OPA_int_t *ptr);
   static inline void  OPA_store(OPA_int_t *ptr, int val);
   static inline void *OPA_load_ptr(OPA_ptr_t *ptr);
   static inline void  OPA_store_ptr(OPA_ptr_t *ptr, void *val);

   static inline void OPA_add(OPA_int_t *ptr, int val);
   static inline void OPA_incr(OPA_int_t *ptr);
   static inline void OPA_decr(OPA_int_t *ptr);
   
   static inline int OPA_decr_and_test(OPA_int_t *ptr);
   static inline int OPA_fetch_and_add(OPA_int_t *ptr, int val);
   static inline int OPA_fetch_and_decr(OPA_int_t *ptr);
   static inline int OPA_fetch_and_incr(OPA_int_t *ptr);
   
   static inline int *OPA_cas_ptr(OPA_ptr_t *ptr, void *oldv, void *newv);
   static inline int  OPA_cas_int(OPA_int_t *ptr, int oldv, int newv);
   
   static inline int *OPA_swap_ptr(OPA_ptr_t *ptr, int *val);
   static inline int  OPA_swap_int(OPA_int_t *ptr, int val);

   static inline void OPA_write_barrier();
   static inline void OPA_read_barrier();
   static inline void OPA_read_write_barrier();
   
   
   The following need to be ported only for architectures supporting LL/SC:
   
   static inline int OPA_LL_ptr(OPA_ptr_t *ptr);
   static inline int OPA_SC_ptr(OPA_ptr_t *ptr, int *val);
   static inline int OPA_LL_int(volatile int *ptr);
   static inline int OPA_SC_int(volatile int *ptr, int val);

   OPA_UNIVERSAL_PRIMITIVE should be defined to be one of:
     OPA_CAS
     OPA_LL_SC
     OPA_NO_UNIVERSAL
*/
enum OPA_Universal_primitive_type {
    OPA_NO_UNIVERSAL = 0,
    OPA_CAS          = 1,
    OPA_LL_SC        = 2
};

/* Include the appropriate header for the architecture */
#if   defined(HAVE_GCC_AND_POWERPC_ASM)
#include "primitives/opa_gcc_ppc.h"
#elif defined(HAVE_GCC_X86_32_64)
#include "primitives/opa_gcc_intel_32_64.h"
#elif defined(HAVE_GCC_AND_IA64_ASM)
#include "primitives/opa_gcc_ia64.h"
#elif defined(HAVE_GCC_AND_SICORTEX_ASM)
#include "primitives/opa_gcc_sicortex.h"
#elif defined(HAVE_GCC_INTRINSIC_ATOMICS)
#include "primitives/opa_gcc_intrinsics.h"
#elif defined(HAVE_SUN_ATOMIC_OPS)
#include "primitives/opa_sun_atomic_ops.h"
#elif defined(HAVE_NT_INTRINSICS)
#include "primitives/opa_nt_intrinsics.h"
#else
/* No architecture specific atomics, using lock-based emulated
   implementations */

/* This is defined to ensure that interprocess locks are properly
   initialized. */
#define USE_ATOMIC_EMULATION
#include "primitives/opa_by_lock.h"
#endif

/*
    This routine is needed because the MPIU_THREAD_XXX_CS_{ENTER,EXIT} macros do
    not provide synchronization across multiple processes, only across multiple
    threads within a process.  In order to safely emulate atomic operations on a
    shared memory region, we need a shared memory backed lock mechanism.

    This routine must be called by any subsystem that intends to use the atomic
    abstractions if the cpp directive USE_ATOMIC_EMULATION is defined.  It must
    be called exactly once by _all_ processes, not just a single leader.  This
    function will initialize the contents of the lock variable if the caller
    specifies (isLeader==true).  Note that multiple initialization is forbidden
    by several lock implementations, especially pthreads.

    Inputs:
      shm_lock - A pointer to an allocated piece of shared memory that can hold
                 an MPIDU_Process_lock_t.  This 
      isLeader - This boolean value should be set to true 
*/
/* FIXME We need to extricate ourselves from MPIDU_Process_locks because they
   are hopelessly broken. */
#if defined(HAVE_PTHREAD_H)
#  include <pthread.h>
int MPIDU_Interprocess_lock_init(pthread_mutex_t *shm_lock, int isLeader);
#endif


/* FIXME This should probably be pushed down into the platform-specific headers. */
#if defined(HAVE_SCHED_YIELD)
#  include <sched.h>
#  define OPA_busy_wait() sched_yield()
#else
#  define OPA_busy_wait() do { } while (0)
#endif

#endif /* defined(OPA_PRIMITIVES_H_INCLUDED) */
