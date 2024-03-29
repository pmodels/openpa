# -*- Mode: c-basic-offset:4 ; indent-tabs-mode:nil ; -*-
#  
#  (C) 2008 by Argonne National Laboratory.
#      See COPYRIGHT in top-level directory.
# 

AC_PREREQ(2.62)

AC_INIT([OpenPA], [1.0.5], [https://github.com/pmodels/openpa/issues/new])
dnl Set the directory that contains support scripts such as install-sh and
dnl config.guess.  It also contains autoconf macro files.
AC_CONFIG_AUX_DIR(confdb)
AC_CONFIG_MACRO_DIR([confdb])

AM_INIT_AUTOMAKE([-Wall -Werror foreign color-tests 1.12.3])

# automake 1.12 seems to require this, but automake 1.11 doesn't recognize it
# must come before LT_INIT
m4_ifdef([AM_PROG_AR],[AM_PROG_AR])

LT_PREREQ([2.2.6])

# Bug in libtool adds -O2 and -g by default
save_cflags=$CFLAGS
LT_INIT(disable-shared)
CFLAGS=$save_cflags

if test -s "$srcdir/VERSION" ; then
   . $srcdir/VERSION
   AC_SUBST([libopa_so_version])
else
   AC_MSG_ERROR([Version information not found. Configuration aborted.])
fi

dnl force configure to be re-run if $top_srcdir/VERSION changes
AC_SUBST([CONFIG_STATUS_DEPENDENCIES],['$(top_srcdir)/VERSION'])

# FIXME this header needs to end up in the installation include directory in some form,
# so we probably need to change its name to something that won't collide in the
# global namespace. [goodell@ 2009-02-19]
AC_CONFIG_HEADER([src/config.h])
AH_TOP([/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
])
AH_BOTTOM([])

dnl Preps an opa_config.h with prefixed macros from config.h for output at
dnl AC_OUTPUT time.  This way we can safely include opa_config.h in the
dnl installation and include it in installed headers.
AX_PREFIX_CONFIG_H([src/opa_config.h],[OPA])

# Non-verbose make
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

########################################################################

# check if we're cross compiling
if test "$build_alias" = "$host_alias" ; then
  CROSS_COMPILING="no"
else
  CROSS_COMPILING="yes"
fi

AC_PROG_CC

AC_HEADER_ASSERT
# do we need intrin.h in here since it's a windows file?
AC_CHECK_HEADERS([pthread.h atomic.h intrin.h inttypes.h stdint.h stddef.h])

AC_C_RESTRICT
AC_C_INLINE

# Not strictly needed (autoconf docs: This macro is obsolescent, as current C
# compilers support const. New programs need not use this macro. )
AC_C_CONST

dnl Check for presence of the pthreads library.  This is needed by the test
dnl suite.
AC_CHECK_LIB(pthread, pthread_create)

dnl Check for presence of pthread_yield.  If not present, check for sched_yield.
dnl These are used by the test suite.
AC_CHECK_FUNCS(pthread_yield,
    ,
    [AC_CHECK_HEADER(sched.h,
        [AC_CHECK_FUNCS(sched_yield)]
    )]
)
if test "$ac_cv_func_pthread_yield" = "yes" ; then
   PAC_FUNC_NEEDS_DECL([#include <pthread.h>],pthread_yield)
fi

if test "$ac_cv_lib_pthread_pthread_create" = yes; then
AC_MSG_CHECKING([if 100 threads can be run at once])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
    #include <stddef.h>
    #include <pthread.h>
    pthread_mutex_t mutexus_maximus;
    void *thread_func(void *udata) {
        pthread_mutex_lock(&mutexus_maximus);
        pthread_mutex_unlock(&mutexus_maximus);
        return NULL;
    }
]], [[
    int i;
    pthread_t threads[99];
    pthread_mutex_init(&mutexus_maximus, NULL);
    pthread_mutex_lock(&mutexus_maximus);
    for(i=0; i<99; i++)
        if(pthread_create(&threads[i], NULL, thread_func, NULL)) {
            pthread_mutex_unlock(&mutexus_maximus);
            return 1;
        }
    pthread_mutex_unlock(&mutexus_maximus);
    return 0;
]])],
AC_MSG_RESULT([yes])
AC_DEFINE(MAX_NTHREADS, 100, [define to the maximum number of simultaneous threads]),
AC_MSG_RESULT([no])
dnl We cannot run 100 threads, check if we can run 10
    AC_MSG_CHECKING([if 10 threads can be run at once])
    AC_RUN_IFELSE([AC_LANG_PROGRAM([[
        #include <stddef.h>
        #include <pthread.h>
        pthread_mutex_t mutexus_maximus;
        void *thread_func(void *udata) {
            pthread_mutex_lock(&mutexus_maximus);
            pthread_mutex_unlock(&mutexus_maximus);
            return NULL;
        }
    ]], [[[
        int i;
        pthread_t threads[9];
        pthread_mutex_init(&mutexus_maximus, NULL);
        pthread_mutex_lock(&mutexus_maximus);
        for(i=0; i<9; i++)
            if(pthread_create(&threads[i], NULL, thread_func, NULL)) {
                pthread_mutex_unlock(&mutexus_maximus);
                return 1;
            }
        pthread_mutex_unlock(&mutexus_maximus);
        return 0;
    ]]])],
    AC_MSG_RESULT([yes])
    AC_DEFINE(MAX_NTHREADS, 10, [define to the maximum number of simultaneous threads]),
    AC_MSG_RESULT([no])
    AC_DEFINE(MAX_NTHREADS, 4, [define to the maximum number of simultaneous threads]),
    AC_MSG_RESULT([N/A])),
AC_MSG_RESULT([N/A]))
fi

AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([int])

dnl Check for __attribute__ support.  This was originally taken from
dnl the PAC_C_GNU_ATTRIBUTE macro in mpich2.
dnl
dnl We start by requiring Gcc.  Some other compilers accept __attribute__
dnl but generate warning messages, or have different interpretations 
dnl (which seems to make __attribute__ just as bad as #pragma) 
dnl For example, the Intel icc compiler accepts __attribute__ and
dnl __attribute__((pure)) but generates warnings for __attribute__((format...))
if test "$GCC" = "yes" ; then
    AC_CACHE_CHECK([whether __attribute__ allowed],
pac_cv_gnu_attr_pure,[
AC_TRY_COMPILE([int foo(int) __attribute__ ((pure));],[int a;],
pac_cv_gnu_attr_pure=yes,pac_cv_gnu_attr_pure=no)])
AC_CACHE_CHECK([whether __attribute__((format)) allowed],
pac_cv_gnu_attr_format,[
AC_TRY_COMPILE([int foo(char *,...) __attribute__ ((format(printf,1,2)));],[int a;],
pac_cv_gnu_attr_format=yes,pac_cv_gnu_attr_format=no)])
    if test "$pac_cv_gnu_attr_pure" = "yes" -a "$pac_cv_gnu_attr_format" = "yes" ; then
        AC_DEFINE(HAVE_GCC_ATTRIBUTE,1,[Define if GNU __attribute__ is supported])
    fi
fi

dnl Check to make sure that the compiler rejects bogus inline assembly
dnl statements.  If it does not, then we need to be careful below when
dnl we're checking which primitives header file to use.
AC_MSG_CHECKING([if compiler rejects bogus asm statements])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[
    __asm__ __volatile__ ("ILLEGAL_ASM_STMT");
  ]])],
  compiler_rejects_bogus_asm=no
  AC_MSG_RESULT([no])
  ,
  compiler_rejects_bogus_asm=yes
  AC_MSG_RESULT([yes])
)

AC_ARG_WITH([atomic-primitives], [AC_HELP_STRING([--with-atomic-primitives],
    [Force OPA to use a specific atomic primitives implementation file (default
     is auto).  A value of 'no' forces the use of locks to implement atomic
     primitives.  A value of 'auto_allow_emulation' will attempt to detect
     native primitives and will fall back to lock-based emulation if none are
     available.  Note that using locks to implement atomic primitives will have
     a substantial impact on performance and is intended for testing only.  A
     value of 'unsafe' will use a non-atomic version of the primitives for performance
     in single-threaded code and meta-testing.])],
    [AS_IF([test "$with_atomic_primitives" = "yes"], [with_atomic_primitives=auto])],
    [with_atomic_primitives=auto]
)


# is set to yes by the macro below if any test ever succeeds
non_emulated_primitives_available=no
# is set to yes in OPA_TRY_PRIMITIVE_HEADER when a primitive
# matching $with_atomic_primitives is checked
checked_specified_primitive=no

AC_DEFUN([OPA_PRIMITIVE_TEST_PGM], [AC_LANG_PROGRAM([[
#define OPA_SIZEOF_INT SIZEOF_INT
#define OPA_SIZEOF_VOID_P SIZEOF_VOID_P
#ifndef _opa_inline
#define _opa_inline inline
#endif
#ifndef _opa_restrict
#define _opa_restrict restrict
#endif
#ifndef _opa_const
#define _opa_const const
#endif
#ifdef HAVE_GCC_ATTRIBUTE
#define OPA_HAVE_GCC_ATTRIBUTE 1
#endif
#include "opa_util.h" /* for OPA_ATTRIBUTE and friends */
#include "primitives/$1"
    ]],[[
    OPA_int_t a, b;
    int c;

    OPA_store_int(&a, 0);
    OPA_store_int(&b, 1);
    c = OPA_load_int(&a);

    OPA_add_int(&a, 10);
    OPA_incr_int(&a);
    OPA_decr_int(&a);

    c = OPA_decr_and_test_int(&a);
    c = OPA_fetch_and_add_int(&a, 10);
    c = OPA_fetch_and_incr_int(&a);
    c = OPA_fetch_and_decr_int(&a);

    c = OPA_cas_int(&a, 10, 11);
    c = OPA_swap_int(&a, OPA_load_int(&b));

    OPA_write_barrier();
    OPA_read_barrier();
    OPA_read_write_barrier();
     ]])]
)

dnl OPA_TRY_PRIMITIVE_HEADER([header file from src/ dir], [HAVE_ macro suffix], [feature description])
dnl Does an AC_LINK_IFELSE() to see if the header file works
AC_DEFUN([OPA_TRY_PRIMITIVE_HEADER],[
if test "$with_atomic_primitives" = "auto"                 || \
   test "$with_atomic_primitives" = "auto_allow_emulation" || \
   test "$with_atomic_primitives" = "$1"
then
    checked_specified_primitive=yes
    AC_MSG_CHECKING([for support for $3])
    SAVE_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -I${srcdir}/src"
    AC_LINK_IFELSE([OPA_PRIMITIVE_TEST_PGM([$1])],
        [AC_DEFINE([HAVE_$2], [1], [define to 1 if we have support for $3])
        non_emulated_primitives_available=yes]
        [AC_MSG_RESULT([yes])]
    ,
        [AC_MSG_RESULT([no])]
    )
    CFLAGS="$SAVE_CFLAGS"
fi
])

dnl OPA_TRY_RUN_PRIMITIVE_HEADER([header file from src/ dir], [HAVE_ macro suffix], [feature description])
dnl Does an AC_RUN_IFELSE() to see if the header file works, but falls back to AC_LINK_IFELSE() 
dnl if we're cross-compiling
AC_DEFUN([OPA_TRY_RUN_PRIMITIVE_HEADER],[
if test "$with_atomic_primitives" = "auto"                 || \
   test "$with_atomic_primitives" = "auto_allow_emulation" || \
   test "$with_atomic_primitives" = "$1"
then
    checked_specified_primitive=yes
    AC_MSG_CHECKING([for support for $3])
    SAVE_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -I${srcdir}/src"
    AC_RUN_IFELSE([OPA_PRIMITIVE_TEST_PGM([$1])],[
        AC_DEFINE([HAVE_$2], [1], [define to 1 if we have support for $3])
        non_emulated_primitives_available=yes
        AC_MSG_RESULT([yes])]
    ,
        [AC_MSG_RESULT([no])]
    ,
        [AC_LINK_IFELSE([OPA_PRIMITIVE_TEST_PGM([$1])],[
            AC_DEFINE([HAVE_$2], [1], [define to 1 if we have support for $3])
            non_emulated_primitives_available=yes
            AC_MSG_RESULT([yes])]
        ,
            [AC_MSG_RESULT([no])]
        )]
    )
    CFLAGS="$SAVE_CFLAGS"
fi
])

using_emulated_primitives=no
using_unsafe_primitives=no

if test "$with_atomic_primitives" = "no" ; then
    using_emulated_primitives=yes
    # EXPLICIT_EMULATION becomes OPA_EXPLICIT_EMULATION in the installed opa_config.h
    AC_DEFINE([EXPLICIT_EMULATION],[1],
              [define if lock-based emulation was explicitly requested at
               configure time via --with-atomic-primitives=no])
elif test "$with_atomic_primitives" = "unsafe" ; then
    using_unsafe_primitives=yes
else
dnl We currently test for support of each platform or implementation by
dnl attempting to compile the associated primitives header file.  This doesn't
dnl feel right, but it's actually pretty effective while being fairly easy to
dnl implement as well.  The biggest problem with this strategy is that if we are
dnl missing some little bit of compatibility code (a missing type or header for
dnl which we have a workaround) we could end up selecting the wrong
dnl implementation.
dnl
dnl If the compiler can't tell that it's getting bad assembly, we have
dnl no hope of being able to check what asm statements are supported
dnl without AC_TRY_RUN().
    if test "$compiler_rejects_bogus_asm" = "yes"  ; then 
        # if we're cross compiling, don't try the gcc_intel_32_64
        # test, since it uses fence operations which are not supported
        # on pre Pentium 4 machines, but it may still compile and link
        if test "$CROSS_COMPILING" = "no" ; then
            OPA_TRY_RUN_PRIMITIVE_HEADER([opa_gcc_intel_32_64.h], [GCC_X86_32_64], [gcc x86/x86_64 primitives])
        fi
        OPA_TRY_RUN_PRIMITIVE_HEADER([opa_gcc_intel_32_64_p3.h], [GCC_X86_32_64_P3], [gcc x86 primitives for pre-Pentium 4])
        OPA_TRY_PRIMITIVE_HEADER([opa_gcc_ia64.h], [GCC_AND_IA64_ASM], [gcc ia64 primitives])
        OPA_TRY_PRIMITIVE_HEADER([opa_gcc_ppc.h], [GCC_AND_POWERPC_ASM], [gcc PowerPC atomics])
        OPA_TRY_PRIMITIVE_HEADER([opa_gcc_arm.h], [GCC_AND_ARM_ASM], [gcc ARM atomics])
        OPA_TRY_PRIMITIVE_HEADER([opa_gcc_sicortex.h], [GCC_AND_SICORTEX_ASM], [gcc SiCortex atomics])
    fi

    OPA_TRY_PRIMITIVE_HEADER([opa_gcc_intrinsics.h], [GCC_INTRINSIC_ATOMICS], [gcc atomic intrinsics])
    OPA_TRY_PRIMITIVE_HEADER([opa_nt_intrinsics.h], [NT_INTRINSICS], [Windows NT atomic intrinsics])
    OPA_TRY_PRIMITIVE_HEADER([opa_sun_atomic_ops.h], [SUN_ATOMIC_OPS], [Sun atomic operations library])

    if test "$checked_specified_primitive" = "no" ; then
        AC_MSG_ERROR([did not find specified atomic primitives file "$with_atomic_primitives"], 1)
    fi

    if test "$non_emulated_primitives_available" = "no" ; then
        if test "$with_atomic_primitives" = "auto_allow_emulation" ; then
            using_emulated_primitives=yes
        else
            AC_MSG_ERROR([
=======================================================
No native supported atomic primitives were detected.
You can use "--with-atomic-primitives=no" to emulate
the atomic primitives using locks, but note that doing
this will result in a substantial performance
penalty.
=======================================================], 1)
        fi
    fi
fi

internal_pkg_config_libs=
external_pkg_config_libs=
if test "$using_emulated_primitives" = "yes" ; then
    AC_MSG_WARN([
===================================================
Using locks to implement atomic primitives.  This
will result in a substantial impact on performance.
Use this only for testing.
===================================================])
    AC_DEFINE(USE_LOCK_BASED_PRIMITIVES, 1, [define to 1 to force using lock-based atomic primitives])
    internal_pkg_config_libs="-lpthread $internal_pkg_config_libs"
    external_pkg_config_libs="-lopa     $external_pkg_config_libs"
fi

AC_SUBST(internal_pkg_config_libs)
AC_SUBST(external_pkg_config_libs)

if test "$using_unsafe_primitives" = "yes" ; then
    AC_MSG_WARN([
===================================================
Using *UNSAFE*, *NON-ATOMIC* primitive operations.
Use this for only for testing or for performance
reasons in non-concurrent code.

Consider yourself warned!
===================================================])
    AC_DEFINE([USE_UNSAFE_PRIMITIVES],[1],[define to 1 if unsafe (non-atomic) primitives should be used])
fi

dnl Check to see if we should enable strict fairness checks
AC_MSG_CHECKING([whether to enable strict fairness checks])
AC_ARG_ENABLE(strict-fairness-checks,
    [AC_HELP_STRING([--enable-strict-fairness-checks],
        [Enable stricter checks of the "fairness" of atomic operations in the
         test suite (default is no).  A value of 'yes' will remove the calls to
         pthread_yield() from these test routines.])],
    [strict_fairness_checks=$enableval],
    [strict_fairness_checks=no])
if test "${strict_fairness_checks}" = "yes"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_STRICT_FAIRNESS_CHECKS], [1],
        [Define if strict checking of atomic operation fairness is desired])
else
    AC_MSG_RESULT([no])
fi

## Enable creation of libtool-style versioning or no versioning
AC_ARG_ENABLE(versioning,
        [AC_HELP_STRING([--enable-versioning],[Enable library versioning])],,
        [enable_versioning=yes])

if test "$enable_versioning" = "yes" ; then
   libopa_so_versionflags="-version-info \$(libopa_so_version)"
else
   libopa_so_versionflags="-avoid-version"
fi
export libopa_so_versionflags
AC_SUBST(libopa_so_versionflags)

AC_ARG_ENABLE(embedded,
        [AC_HELP_STRING([--enable-embedded],[Enable embedded mode])])
AM_CONDITIONAL([EMBEDDED], [test "x${enable_embedded}" = "xyes"])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile openpa.pc])
AC_OUTPUT

