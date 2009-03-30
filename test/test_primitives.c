/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

/*
 * Uncomment this definition to disable the OPA library and instead use naive
 * (non-atomic) operations.  This should cause failures.
 */
/*
#define OPA_TEST_NAIVE
*/

#include "opa_test.h"

/* Definitions for test_threaded_loadstore_int */
#define LOADSTORE_INT_DIFF ((1 << (sizeof(int) * CHAR_BIT / 2)) - 1)
#define LOADSTORE_INT_NITER 2000000
typedef struct {
    OPA_int_t   *shared_val;    /* Shared int being read/written by all threads */
    int         unique_val;     /* This thread's unique value to store in shared_val */
} loadstore_int_t;

/* Definitions for test_threaded_loadstore_ptr */
#define LOADSTORE_PTR_DIFF (((unsigned long) 1 << (sizeof(void *) * CHAR_BIT / 2)) - 1)
#define LOADSTORE_PTR_NITER 2000000
typedef struct {
    OPA_ptr_t   *shared_val;    /* Shared int being read/written by all threads */
    void        *unique_val;    /* This thread's unique value to store in shared_val */
} loadstore_ptr_t;

/* Definitions for test_threaded_add */
#define ADD_EXPECTED 0
#define ADD_NITER 1000000
typedef struct {
    OPA_int_t   *shared_val;    /* Shared int being added to by all threads */
    int         unique_val;    /* This thread's unique value to add to shared_val */
} add_t;


/*-------------------------------------------------------------------------
 * Function: test_simple_loadstore_int
 *
 * Purpose: Tests basic functionality of OPA_load and OPA_store with a
 *          single thread.  Does not test atomicity of operations.
 *
 * Return: Success: 0
 *         Failure: 1
 *
 * Programmer: Neil Fortner
 *             Thursday, March 19, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_simple_loadstore_int(void)
{
    OPA_int_t   a, b;

    TESTING("simple integer load/store functionality", 0);

    /* Store 0 in a, -1 in b.  Verify that these values are returned by
     * OPA_load. */
    OPA_store(&a, 0);
    if(0 != OPA_load(&a)) TEST_ERROR;
    OPA_store(&b, -1);
    if(-1 != OPA_load(&b)) TEST_ERROR;
    if(0 != OPA_load(&a)) TEST_ERROR;
    if(-1 != OPA_load(&b)) TEST_ERROR;

    /* Store INT_MIN in a and INT_MAX in b.  Verify that these values are
     * returned by OPA_LOAD. */
    OPA_store(&a, INT_MIN);
    if(INT_MIN != OPA_load(&a)) TEST_ERROR;
    OPA_store(&b, INT_MAX);
    if(INT_MAX != OPA_load(&b)) TEST_ERROR;
    if(INT_MIN != OPA_load(&a)) TEST_ERROR;
    if(INT_MAX != OPA_load(&b)) TEST_ERROR;

    PASSED();
    return 0;

error:
    return 1;
} /* end test_simple_loadstore_int() */


#if defined(OPA_HAVE_PTHREAD_H)
/*-------------------------------------------------------------------------
 * Function: threaded_loadstore_int_helper
 *
 * Purpose: Helper (thread) routine for test_threaded_loadstore_int
 *
 * Return: Success: NULL
 *         Failure: non-NULL
 *
 * Programmer: Neil Fortner
 *             Thursday, March 19, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static void *threaded_loadstore_int_helper(void *_udata)
{
    loadstore_int_t     *udata = (loadstore_int_t *)_udata;
    OPA_int_t           *shared_val = udata->shared_val;
    int                 unique_val = udata->unique_val;
    int                 loaded_val;
    int                 niter = LOADSTORE_INT_NITER / iter_reduction[curr_test];
    int                 nerrors = 0;
    int                 i;

    /* Main loop */
    for(i=0; i<niter; i++) {
        /* Store the unique value into the shared value */
        OPA_store(udata->shared_val, udata->unique_val);

        /* Load the shared_value, and check if it is valid */
        if((loaded_val = OPA_load(udata->shared_val)) % LOADSTORE_INT_DIFF) {
            printf("    Unexpected load: %d is not a multiple of %d (case 1)\n",
                    loaded_val, LOADSTORE_INT_DIFF);
            nerrors++;
        } /* end if */
    } /* end for */

    /* Any non-NULL exit value indicates an error, we use &i here */
    pthread_exit(nerrors ? &i : NULL);
} /* end threaded_loadstore_int_helper() */
#endif /* OPA_HAVE_PTHREAD_H */


/*-------------------------------------------------------------------------
 * Function: test_threaded_loadstore_int
 *
 * Purpose: Tests atomicity of OPA_load and OPA_store.  Launches nthreads
 *          threads, each of which repeatedly loads and stores a shared
 *          variable.
 *
 * Return: Success: 0
 *         Failure: 1
 *
 * Programmer: Neil Fortner
 *             Thursday, March 19, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_threaded_loadstore_int(void)
{
#if defined(OPA_HAVE_PTHREAD_H)
    pthread_t           *threads = NULL; /* Threads */
    pthread_attr_t      ptattr;         /* Thread attributes */
    loadstore_int_t     *thread_data = NULL; /* User data structs for each thread */
    OPA_int_t           shared_int[2];     /* Integer shared between threads */
    void                *ret;           /* Thread return value */
    unsigned            nthreads = num_threads[curr_test];
    int                 nerrors = 0;    /* number of errors */
    int                 i;

    TESTING("integer load/store", nthreads);

    /* Allocate array of threads */
    if(NULL == (threads = (pthread_t *) malloc(nthreads * sizeof(pthread_t))))
        TEST_ERROR;

    /* Allocate array of thread data */
    if(NULL == (thread_data = (loadstore_int_t *) malloc(nthreads *
            sizeof(loadstore_int_t)))) TEST_ERROR;

    /* Set threads to be joinable */
    pthread_attr_init(&ptattr);
    pthread_attr_setdetachstate(&ptattr, PTHREAD_CREATE_JOINABLE);

    /* Create the threads */
    for(i=0; i<nthreads; i++) {
        thread_data[i].shared_val = &shared_int;
        thread_data[i].unique_val = i * LOADSTORE_INT_DIFF;
        if(pthread_create(&threads[i], NULL, threaded_loadstore_int_helper,
                &thread_data[i])) TEST_ERROR;
    } /* end for */

    /* Free the attribute */
    if(pthread_attr_destroy(&ptattr)) TEST_ERROR;

    /* Join the threads */
    for (i=0; i<nthreads; i++) {
        if(pthread_join(threads[i], &ret)) TEST_ERROR;
        if(ret)
            nerrors++;
    } /* end for */

    /* Check for errors */
    if(nerrors)
        FAIL_OP_ERROR(printf("    Unexpected return from %d thread%s\n", nerrors,
                nerrors == 1 ? "" : "s"));

    /* Free memory */
    free(threads);
    free(thread_data);

    PASSED();

#else /* OPA_HAVE_PTHREAD_H */
    TESTING("integer load/store", 0);
    SKIPPED();
    puts("    pthread.h not available");
#endif /* OPA_HAVE_PTHREAD_H */

    return 0;

#if defined(OPA_HAVE_PTHREAD_H)
error:
    if(threads) free(threads);
    if(thread_data) free(thread_data);
    return 1;
#endif /* OPA_HAVE_PTHREAD_H */
} /* end test_threaded_loadstore_int() */


/*-------------------------------------------------------------------------
 * Function: test_simple_loadstore_ptr
 *
 * Purpose: Tests basic functionality of OPA_load and OPA_store with a
 *          single thread.  Does not test atomicity of operations.
 *
 * Return: Success: 0
 *         Failure: 1
 *
 * Programmer: Neil Fortner
 *             Thursday, March 20, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_simple_loadstore_ptr(void)
{
    OPA_ptr_t   a, b;

    TESTING("simple pointer load/store functionality", 0);

    /* Store 0 in a, 1 in b.  Verify that these values are returned by
     * OPA_load. */
    OPA_store_ptr(&a, (void *) 0);
    if((void *) 0 != OPA_load_ptr(&a)) TEST_ERROR;
    OPA_store_ptr(&b, (void *) 1);
    if((void *) 1 != OPA_load_ptr(&b)) TEST_ERROR;
    if((void *) 0 != OPA_load_ptr(&a)) TEST_ERROR;
    if((void *) 1 != OPA_load_ptr(&b)) TEST_ERROR;

    /* Store -1 in a and -2 in b.  Verify that these values are returned by
     * OPA_LOAD. */
    OPA_store_ptr(&a, (void *) -2);
    if((void *) -2 != OPA_load_ptr(&a)) TEST_ERROR;
    OPA_store_ptr(&b, (void *) -1);
    if((void *) -1 != OPA_load_ptr(&b)) TEST_ERROR;
    if((void *) -2 != OPA_load_ptr(&a)) TEST_ERROR;
    if((void *) -1 != OPA_load_ptr(&b)) TEST_ERROR;

    PASSED();
    return 0;

error:
    return 1;
} /* end test_simple_loadstore_ptr() */


#if defined(OPA_HAVE_PTHREAD_H)
/*-------------------------------------------------------------------------
 * Function: threaded_loadstore_ptr_helper
 *
 * Purpose: Helper (thread) routine for test_threaded_loadstore_ptr
 *
 * Return: Success: NULL
 *         Failure: non-NULL
 *
 * Programmer: Neil Fortner
 *             Thursday, March 20, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static void *threaded_loadstore_ptr_helper(void *_udata)
{
    loadstore_ptr_t     *udata = (loadstore_ptr_t *)_udata;
    OPA_ptr_t           *shared_val = udata->shared_val;
    void                *unique_val = udata->unique_val;
    unsigned long       loaded_val;
    int                 niter = LOADSTORE_PTR_NITER / iter_reduction[curr_test];
    int                 nerrors = 0;
    int                 i;

    /* Main loop */
    for(i=0; i<niter; i++) {
        /* Store the unique value into the shared value */
        OPA_store_ptr(udata->shared_val, udata->unique_val);

        /* Load the shared_value, and check if it is valid */
        if((loaded_val = (unsigned long) OPA_load_ptr(udata->shared_val)) % LOADSTORE_PTR_DIFF) {
            printf("    Unexpected load: %ld is not a multiple of %ld (case 1)\n",
                    loaded_val, LOADSTORE_PTR_DIFF);
            nerrors++;
        } /* end if */
    } /* end for */

    /* Any non-NULL exit value indicates an error, we use &i here */
    pthread_exit(nerrors ? &i : NULL);
} /* end threaded_loadstore_ptr_helper() */
#endif /* OPA_HAVE_PTHREAD_H */


/*-------------------------------------------------------------------------
 * Function: test_threaded_loadstore_ptr
 *
 * Purpose: Tests atomicity of OPA_load and OPA_store.  Launches nthreads
 *          threads, each of which repeatedly loads and stores a shared
 *          variable.
 *
 * Return: Success: 0
 *         Failure: 1
 *
 * Programmer: Neil Fortner
 *             Thursday, March 20, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_threaded_loadstore_ptr(void)
{
#if defined(OPA_HAVE_PTHREAD_H)
    pthread_t           *threads = NULL; /* Threads */
    pthread_attr_t      ptattr;         /* Thread attributes */
    loadstore_ptr_t     *thread_data = NULL; /* User data structs for each thread */
    OPA_ptr_t           shared_ptr;     /* Integer shared between threads */
    void                *ret;           /* Thread return value */
    unsigned            nthreads = num_threads[curr_test];
    int                 nerrors = 0;    /* number of errors */
    int                 i;

    TESTING("pointer load/store", nthreads);

    /* Allocate array of threads */
    if(NULL == (threads = (pthread_t *) malloc(nthreads * sizeof(pthread_t))))
        TEST_ERROR;

    /* Allocate array of thread data */
    if(NULL == (thread_data = (loadstore_ptr_t *) malloc(nthreads *
            sizeof(loadstore_ptr_t)))) TEST_ERROR;

    /* Set threads to be joinable */
    pthread_attr_init(&ptattr);
    pthread_attr_setdetachstate(&ptattr, PTHREAD_CREATE_JOINABLE);

    /* Create the threads */
    for(i=0; i<nthreads; i++) {
        thread_data[i].shared_val = &shared_ptr;
        thread_data[i].unique_val = (void *) ((unsigned long) i * LOADSTORE_PTR_DIFF);
        if(pthread_create(&threads[i], NULL, threaded_loadstore_ptr_helper,
                &thread_data[i])) TEST_ERROR;
    } /* end for */

    /* Free the attribute */
    if(pthread_attr_destroy(&ptattr)) TEST_ERROR;

    /* Join the threads */
    for (i=0; i<nthreads; i++) {
        if(pthread_join(threads[i], &ret)) TEST_ERROR;
        if(ret)
            nerrors++;
    } /* end for */

    /* Check for errors */
    if(nerrors)
        FAIL_OP_ERROR(printf("    Unexpected return from %d thread%s\n", nerrors,
                nerrors == 1 ? "" : "s"));

    /* Free memory */
    free(threads);
    free(thread_data);

    PASSED();

#else /* OPA_HAVE_PTHREAD_H */
    TESTING("pointer load/store", 0);
    SKIPPED();
    puts("    pthread.h not available");
#endif /* OPA_HAVE_PTHREAD_H */

    return 0;

#if defined(OPA_HAVE_PTHREAD_H)
error:
    if(threads) free(threads);
    if(thread_data) free(thread_data);
    return 1;
#endif /* OPA_HAVE_PTHREAD_H */
} /* end test_threaded_loadstore_ptr() */


/*-------------------------------------------------------------------------
 * Function: test_simple_add_incr_decr
 *
 * Purpose: Tests basic functionality of OPA_add, OPA_incr and OPA_decr
 *          with a single thread.  Does not test atomicity of operations.
 *
 * Return: Success: 0
 *         Failure: 1
 *
 * Programmer: Neil Fortner
 *             Thursday, March 20, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_simple_add_incr_decr(void)
{
    OPA_int_t   a;


    TESTING("simple add/incr/decr functionality", 0);

    /* Store 0 in a */
    OPA_store(&a, 0);

    /* Add INT_MIN */
    OPA_add(&a, INT_MIN);

    /* Increment */
    OPA_incr(&a);

    /* Add INT_MAX */
    OPA_add(&a, INT_MAX);

    /* Decrement */
    OPA_decr(&a);

    /* Load the result, verify it is correct */
    if(OPA_load(&a) != INT_MIN + 1 + INT_MAX - 1) TEST_ERROR;

    /* Store 0 in a */
    OPA_store(&a, 0);

    /* Add INT_MAX */
    OPA_add(&a, INT_MAX);

    /* Decrement */
    OPA_decr(&a);

    /* Add INT_MIN */
    OPA_add(&a, INT_MIN);

    /* Increment */
    OPA_incr(&a);

    /* Load the result, verify it is correct */
    if(OPA_load(&a) != INT_MAX - 1 + INT_MIN + 1) TEST_ERROR;

    PASSED();
    return 0;

error:
    return 1;
} /* end test_simple_add_incr_decr() */


#if defined(OPA_HAVE_PTHREAD_H)
/*-------------------------------------------------------------------------
 * Function: threaded_add_helper
 *
 * Purpose: Helper (thread) routine for test_threaded_add
 *
 * Return: Success: NULL
 *         Failure: non-NULL
 *
 * Programmer: Neil Fortner
 *             Thursday, March 20, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static void *threaded_add_helper(void *_udata)
{
    add_t               *udata = (add_t *)_udata;
    OPA_int_t           *shared_val = udata->shared_val;
    int                 unique_val = udata->unique_val;
    int                 niter = ADD_NITER / iter_reduction[curr_test];
    int                 i;

    /* Main loop */
    for(i=0; i<niter; i++)
        /* Add the unique value to the shared value */
        OPA_add(udata->shared_val, udata->unique_val);

    /* Any non-NULL exit value indicates an error, we use &i here */
    pthread_exit(NULL);
} /* end threaded_add_helper() */
#endif /* OPA_HAVE_PTHREAD_H */


/*-------------------------------------------------------------------------
 * Function: test_threaded_add
 *
 * Purpose: Tests atomicity of OPA_add and OPA_store.  Launches nthreads
 *          threads, each of which repeatedly adds a unique number to a
 *          shared variable.
 *
 * Return: Success: 0
 *         Failure: 1
 *
 * Programmer: Neil Fortner
 *             Thursday, March 20, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_threaded_add(void)
{
#if defined(OPA_HAVE_PTHREAD_H)
    pthread_t           *threads = NULL; /* Threads */
    pthread_attr_t      ptattr;         /* Thread attributes */
    add_t               *thread_data = NULL; /* User data structs for each thread */
    OPA_int_t           shared_val;     /* Integer shared between threads */
    unsigned            nthreads = num_threads[curr_test];
    int                 i;

    TESTING("add", nthreads);

    /* Allocate array of threads */
    if(NULL == (threads = (pthread_t *) malloc(nthreads * sizeof(pthread_t))))
        TEST_ERROR;

    /* Allocate array of thread data */
    if(NULL == (thread_data = (add_t *) malloc(nthreads * sizeof(add_t)))) TEST_ERROR;

    /* Set threads to be joinable */
    pthread_attr_init(&ptattr);
    pthread_attr_setdetachstate(&ptattr, PTHREAD_CREATE_JOINABLE);

    /* Set the initial state of the shared value (ADD_EXPECTED) */
    OPA_store(&shared_val, ADD_EXPECTED);

    /* Create the threads.  All the unique values must add up to 0. */
    for(i=0; i<nthreads; i++) {
        thread_data[i].shared_val = &shared_val;
        thread_data[i].unique_val = i - (nthreads - 1) / 2
                - (!(nthreads % 2) && (i >= nthreads / 2));
        if(pthread_create(&threads[i], NULL, threaded_add_helper,
                &thread_data[i])) TEST_ERROR;
    } /* end for */

    /* Free the attribute */
    if(pthread_attr_destroy(&ptattr)) TEST_ERROR;

    /* Join the threads */
    for (i=0; i<nthreads; i++)
        if(pthread_join(threads[i], NULL)) TEST_ERROR;

    /* Verify that the shared value contains the expected result (0) */
    if(OPA_load(&shared_val) != ADD_EXPECTED)
        FAIL_OP_ERROR(printf("    Unexpected result: %d expected: %d\n",
                OPA_load(&shared_val), ADD_EXPECTED));

    /* Free memory */
    free(threads);
    free(thread_data);

    PASSED();

#else /* OPA_HAVE_PTHREAD_H */
    TESTING("add", 0);
    SKIPPED();
    puts("    pthread.h not available");
#endif /* OPA_HAVE_PTHREAD_H */

    return 0;

#if defined(OPA_HAVE_PTHREAD_H)
error:
    if(threads) free(threads);
    if(thread_data) free(thread_data);
    return 1;
#endif /* OPA_HAVE_PTHREAD_H */
} /* end test_threaded_add() */


/*-------------------------------------------------------------------------
 * Function:    main
 *
 * Purpose:     Tests the opa primitives
 *
 * Return:      Success:        exit(0)
 *
 *              Failure:        exit(1)
 *
 * Programmer:  Neil Fortner
 *              Thursday, March 19, 2009
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int main(int argc, char **argv)
{
    unsigned nerrors = 0;

    nerrors += test_simple_loadstore_int();
    nerrors += test_simple_loadstore_ptr();
    nerrors += test_simple_add_incr_decr();

    for(curr_test=0; curr_test<num_thread_tests; curr_test++) {
        nerrors += test_threaded_loadstore_int();
        nerrors += test_threaded_loadstore_ptr();
        nerrors += test_threaded_add();
    }

    if(nerrors)
        goto error;
    printf("All primitives tests passed.\n");

    return 0;

error:
    if(!nerrors)
        nerrors = 1;
    printf("***** %d PRIMITIVES TEST%s FAILED! *****\n",
            nerrors, 1 == nerrors ? "" : "S");
    return 1;
} /* end main() */
