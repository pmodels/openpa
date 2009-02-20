/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef OPA_MEM_BARRIERS_H_INCLUDED
#define OPA_MEM_BARRIERS_H_INCLUDED

/* TODO fill more of these in, including versions that use the AO lib */

#include <config.h>

#if defined(HAVE_NT_INTRINSICS)
#include <intrin.h>
#endif

#if defined(HAVE_GCC_AND_PENTIUM_ASM) || defined(HAVE_GCC_AND_X86_64_ASM)
#  define OPA_Shm_write_barrier() __asm__ __volatile__  ( "sfence" ::: "memory" )
#  define OPA_Shm_read_barrier() __asm__ __volatile__  ( "lfence" ::: "memory" )
#  define OPA_Shm_read_write_barrier() __asm__ __volatile__  ( "mfence" ::: "memory" )
#elif defined(HAVE_MASM_AND_X86)
#  define OPA_Shm_write_barrier()
#  define OPA_Shm_read_barrier() __asm { __asm _emit 0x0f __asm _emit 0xae __asm _emit 0xe8 }
#  define OPA_Shm_read_write_barrier()
#elif defined(HAVE_GCC_AND_IA64_ASM)
#  define OPA_Shm_write_barrier() __asm__ __volatile__  ("mf" ::: "memory" )
#  define OPA_Shm_read_barrier() __asm__ __volatile__  ("mf" ::: "memory" )
#  define OPA_Shm_read_write_barrier() __asm__ __volatile__  ("mf" ::: "memory" )
#elif defined(HAVE_NT_INTRINSICS)
#   define OPA_Shm_write_barrier() {_WriteBarrier();}
#   define OPA_Shm_read_barrier() {_ReadBarrier();}
#   define OPA_Shm_read_write_barrier() {_ReadWriteBarrier();}
#elif defined(HAVE_GCC_AND_SICORTEX_ASM)
#  define OPA_Shm_write_barrier() __asm__ __volatile__  ("sync" ::: "memory" )
#  define OPA_Shm_read_barrier() __asm__ __volatile__  ("sync" ::: "memory" )
#  define OPA_Shm_read_write_barrier() __asm__ __volatile__  ("sync" ::: "memory" )
#else
/* FIXME this should probably be a configure/winconfigure test instead. */
/* FIXME need to dump MPID_* anything now that we've moved OPA to be a separate project */
#  define OPA_Shm_write_barrier()      MPID_Abort(MPIR_Process.comm_self, MPI_ERR_OTHER, 1, "memory barriers not implemented on this platform")
#  define OPA_Shm_read_barrier()       MPID_Abort(MPIR_Process.comm_self, MPI_ERR_OTHER, 1, "memory barriers not implemented on this platform")
#  define OPA_Shm_read_write_barrier() MPID_Abort(MPIR_Process.comm_self, MPI_ERR_OTHER, 1, "memory barriers not implemented on this platform")
#endif

#endif /* defined(OPA_MEM_BARRIERS_H_INCLUDED) */
