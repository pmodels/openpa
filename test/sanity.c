/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*  
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "opa_primitives.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    OPA_int_t a, b;
    int c;
    OPA_store(&a, 0);
    OPA_store(&b, 1);
    OPA_add(&a, 10);
    assert(10 == OPA_load(&a));
    c = OPA_cas_int(&a, 10, 11);
    assert(10 == c);
    c = OPA_swap_int(&a, OPA_load(&b));
    assert(11 == c);
    assert(1 == OPA_load(&a));

    printf("success!\n");

    return 0;
}

