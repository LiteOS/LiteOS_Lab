/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * LiteOS NOVA is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

#include "tester.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "cmockery.h"

#include "kprintf.h"
#include "heap.h"

extern int heap_destroy (heap_t * heap);

void test_heap_init (void **state)
    {
    heap_t h;
    memset (&h, 0, sizeof (heap_t));

    assert_int_equal (-1, heap_init (NULL));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);

    assert_int_equal (0, heap_init (&h));
    assert_int_equal (-1, heap_init (&h)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_OPERATION);
    assert_int_equal (0, heap_destroy (&h));
    }

void test_heap_destroy (void **state)
    {
    heap_t h;
    memset (&h, 0, sizeof (heap_t));

    assert_int_equal (-1, heap_destroy (NULL));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);
    assert_int_equal (-1, heap_destroy (&h));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_int_equal (0, heap_init (&h));
    assert_int_equal (0, heap_destroy (&h));
    assert_int_equal (-1, heap_destroy (&h)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);

    assert_int_equal (-1, heap_destroy ((heap_id)0x1234)); // invalid address
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_heap_create (void **state)
    {
    heap_id h1_id;
    heap_id h2_id;
    heap_id h3_id;

    assert_true (h1_id = heap_create ());
    assert_true (h2_id = heap_create ());
    assert_true (h3_id = heap_create ());

    // do not destroy in order
    assert_int_equal (0, heap_destroy (h3_id));
    assert_int_equal (0, heap_destroy (h1_id));
    assert_int_equal (0, heap_destroy (h2_id));

    assert_int_equal (-1, heap_destroy (h1_id)); // repeated
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_ID);
    }

void test_heap_add (void **state)
    {
    heap_t h;
    char mem [64] = {0};
    size_t bytes = sizeof (mem);
    char *ptr;

    assert_int_equal (-1, heap_add (NULL, NULL, 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);
    assert_int_equal (-1, heap_add (&h, mem, 10));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_BLOCK);
    assert_int_equal (-1, heap_add (&h, mem, bytes));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_int_equal (0, heap_init (&h));

    assert_int_equal (-1, heap_add (&h, NULL, 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_BLOCK);
    assert_int_equal (-1, heap_add (&h, mem, -1));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_BLOCK);
    assert_int_equal (-1, heap_add (&h, mem, 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_BLOCK);

    assert_int_equal (0, heap_add (&h, mem, bytes));
    assert_true (ptr = heap_alloc (&h, 10));
    assert_int_equal (-1, heap_destroy (&h));
    assert_true ((heap_free (&h, ptr), 1));
    assert_int_equal (0, heap_destroy (&h));
    }

// heap_alloc like this
void test_heap_alloc_align (void **state)
    {
    heap_t h;
    char *ptr;

    memset (&h, 0, sizeof (heap_t));

    assert_false (ptr = heap_alloc_align (NULL, 0, 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);
    assert_false (ptr = heap_alloc_align (&h, 3, 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ALIGN);
    assert_false (ptr = heap_alloc_align (&h, 0, 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_true (ptr = heap_alloc_align (kernel_heap, 0, 0));
    assert_true ((heap_free (kernel_heap, ptr), 1));
    }

void test_heap_free (void **state)
    {
    heap_t h;
    char *ptr;

    memset (&h, 0, sizeof (heap_t));

    assert_false ((heap_free (NULL, NULL), 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);
    assert_false ((heap_free (&h, NULL), 0)); // mem is NULL

    assert_true (ptr = heap_alloc_align (kernel_heap, 0, 0));

    assert_false ((heap_free (&h, ptr), 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);

    assert_true ((heap_free (kernel_heap, ptr), 1));
    }

// cutdown, expand, alloc, free
void test_heap_realloc (void **state)
    {
    heap_t h;
    char *ptr;

    memset (&h, 0, sizeof (heap_t));

    assert_false (ptr = heap_realloc (NULL, NULL, 0));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);

    assert_true (ptr = heap_realloc (kernel_heap, NULL, 0));    // alloc
    assert_false (heap_realloc (&h, ptr, 0));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);
    assert_false (ptr = heap_realloc (kernel_heap, ptr, 0));    // free

    assert_true (ptr = heap_alloc (kernel_heap, 32));
    assert_true (ptr = heap_realloc (kernel_heap, ptr, 24));    // cutdown
    assert_true (ptr = heap_realloc (kernel_heap, ptr, 32));    // expand
    assert_false (ptr = heap_realloc (kernel_heap, ptr, 0));    // free
    }

#ifdef CONFIG_MEM_STATISTICS
static void __print_heap_statistics (mem_stat_t * stat)
    {
    kprintf ("\nheap statistics:\n");
    kprintf ("free_chunks:          %u\n", stat->free_chunks);
    kprintf ("free_size:            %u\n", stat->free_size);
    kprintf ("busy_chunks:          %u\n", stat->busy_chunks);
    kprintf ("busy_size:            %u\n", stat->busy_size);
    kprintf ("max_busy_size:        %u\n", stat->max_busy_size);
    kprintf ("max_free_size:        %u\n", stat->max_free_size);
    kprintf ("cum_allocated:        %llu\n", stat->cum_allocated);
    kprintf ("cum_size_allocated:   %llu\n", stat->cum_size_allocated);
    kprintf ("cum_freed:            %llu\n", stat->cum_freed);
    kprintf ("cum_size_freed:       %llu\n", stat->cum_size_freed);
    }

void test_heap_stat_get (void **state)
    {
    mem_stat_t stat, statA, statF; // stat_after_freed, stat_after_alloced
    heap_t h;
    char *ptr[32] = {NULL};
    int i, j;

    memset (&stat, 0, sizeof (mem_stat_t));
    memset (&h, 0, sizeof (heap_t));

    assert_int_equal (-1, heap_stat_get (NULL, NULL));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_ID);
    assert_int_equal (-1, heap_stat_get (&h, NULL));
    assert_int_equal (errno, ERRNO_HEAP_ILLEGAL_STAT);
    assert_int_equal (-1, heap_stat_get (&h, &stat));
    assert_int_equal (errno, ERRNO_OBJ_ILLEGAL_MAGIC);


    assert_int_equal (0, heap_stat_get (kernel_heap, &stat));
    __print_heap_statistics (&stat);

    j = 0;
    for (i = 0; i < 32; ++i)
        {
        if ((ptr[i] = heap_alloc (kernel_heap, 100)) != NULL)
            {
            j++;
            }
        }
    assert_int_equal (0, heap_stat_get (kernel_heap, &statA));
    __print_heap_statistics (&statA);
    
    // check
    // assert_int_equal (stat.free_chunks, statA.free_chunks);  // stat.free_chunks >= statA.free_chunks
    assert_int_equal (stat.busy_chunks + j, statA.busy_chunks);
    assert_int_equal (stat.free_size + stat.busy_size, statA.free_size + statA.busy_size);

    assert_int_equal (stat.cum_allocated + j, statA.cum_allocated);
    assert_int_equal (statA.cum_size_allocated - stat.cum_size_allocated, statA.busy_size - stat.busy_size);
    assert_int_equal (stat.cum_freed, statA.cum_freed);
    assert_int_equal (stat.cum_size_freed, statA.cum_size_freed);

    for (i = 0; i < 32; ++i)
        {
        if (ptr[i] != NULL)
            {
            assert_true ((heap_free (kernel_heap, ptr[i]), 1));
            ptr[i] = NULL;
            }
        }
    
    assert_int_equal (0, heap_stat_get (kernel_heap, &statF));
    __print_heap_statistics (&statF);

    // check
    assert_int_equal (stat.free_chunks, statF.free_chunks);
    assert_int_equal (stat.free_size, statF.free_size);
    assert_int_equal (stat.busy_chunks, statF.busy_chunks);
    assert_int_equal (stat.busy_size, statF.busy_size);

    assert_int_equal (statA.max_busy_size, statF.max_busy_size);
    assert_int_equal (stat.max_free_size, statF.max_free_size);

    assert_int_equal (statA.cum_allocated, statF.cum_allocated);
    assert_int_equal (statA.cum_size_allocated, statF.cum_size_allocated);

    assert_int_equal (statA.cum_freed + j, statF.cum_freed);
    assert_int_equal (statA.cum_size_allocated - stat.cum_size_allocated, 
                      statF.cum_size_freed     - statA.cum_size_freed);
    }
#endif

/** test entry */

void test_heap (struct tester_case * caser)
    {
    UnitTest tests [] =
        {
        unit_test (test_heap_init),
        unit_test (test_heap_destroy),
        unit_test (test_heap_create),
        unit_test (test_heap_add),
        unit_test (test_heap_alloc_align),
        unit_test (test_heap_free),
        unit_test (test_heap_realloc),
#ifdef CONFIG_MEM_STATISTICS
        unit_test (test_heap_stat_get),
#endif
        };

    caser->stats->totals = sizeof(tests) / sizeof(tests[0]);
    caser->stats->fails = run_tests (tests);
    }

TESTER_CASE_DEF ("test_heap", test_heap, NULL, NULL, 0);