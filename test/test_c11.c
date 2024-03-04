#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if __STDC_VERSION__ >= 201112L
# include <stdbool.h>
# include <stdatomic.h>
#endif

int main(int argc, char **argv)
{
#if __STDC_VERSION__ >= 201112L
    atomic_int a, b;
    int c;

    //OPA_store_int(&a, 0);
    atomic_store_explicit(&a, 0, memory_order_relaxed);

    //OPA_store_int(&b, 1);
    atomic_store_explicit(&b, 1, memory_order_relaxed);

    //OPA_add_int(&a, 10);
    atomic_fetch_add_explicit(&a, 10, memory_order_relaxed);

    //assert(10 == OPA_load_int(&a));
    assert(10 == atomic_load_explicit(&a, memory_order_relaxed));

    //c = OPA_cas_int(&a, 10, 11);
    {
      int old = 10;
      int new = 11;
      atomic_compare_exchange_weak_explicit(&a, &old, new,
                                            memory_order_relaxed,
                                            memory_order_relaxed);
      c = old;
    }
    assert(10 == c);

    //c = OPA_swap_int(&a, OPA_load_int(&b));
    c = atomic_exchange_explicit(&a,
                                 atomic_load_explicit(&b, memory_order_relaxed),
                                 memory_order_relaxed);

    assert(11 == c);

    //assert(1 == OPA_load_int(&a));
    assert(1 == atomic_load_explicit(&a, memory_order_relaxed));

    printf("success!\n");
#else
    printf("C11 not supported!\n");
#endif
    return 0;
}

