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

/*
 * note: this file can also be compiled on a x86/x64 host
 * for msys2, linux:
 *     *) gcc   -O3 -DCONFIG_BESTFIT -D__HOST__ heap_ut.c -I../h -I../../include -I../../core/mem/h -I../../core/kernel/h -I../../utils/h -I../../core/services/h -o heap_ut
 *     *) gcc   -O3 -DCONFIG_TLSF    -D__HOST__ heap_ut.c -I../h -I../../include -I../../core/mem/h -I../../core/kernel/h -I../../utils/h -I../../core/services/h -o heap_ut
 *     *) clang -O3 -DCONFIG_BESTFIT -D__HOST__ heap_ut.c -I../h -I../../include -I../../core/mem/h -I../../core/kernel/h -I../../utils/h -I../../core/services/h -o heap_ut
 *     *) clang -O3 -DCONFIG_TLSF    -D__HOST__ heap_ut.c -I../h -I../../include -I../../core/mem/h -I../../core/kernel/h -I../../utils/h -I../../core/services/h -o heap_ut

 * for msvc:
 *     1) create a project
 *     2) add predefined macro "__HOST__"
 *     3) add the include path mentioned above
 *     4) build and run
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef __HOST__
#include <task.h>
#include <mutex.h>

#include <bug.h>
#include <phys_mem.h>
#include <chunk.h>
#include <heap.h>
#include <cmder.h>
#include <config.h>
#else
#include <stdarg.h>

#if !defined (CONFIG_TLSF) && !defined (CONFIG_BESTFIT)
#define CONFIG_TLSF
#endif

#define CONFIG_HEAP_DEBUG

#include "../../core/mem/src/heap.c"
#include "../../utils/src/rbtree.c"

#include "../../core/kernel/src/obj.c"
#include "../../core/kernel/src/class.c"

typedef int cmder_t;

#define NR_KBS              1024

static unsigned long long buff [NR_KBS * 1024 / sizeof (unsigned long long)];

#ifdef __INIT__
heap_t  kernel_heap [1] = {0,};
#else
#define kernel_heap     kernel_heap_id
heap_id kernel_heap;
#endif

volatile int bug_dummy_check = 0;

void __bug (const char * str)
    {
    printf ("bug: %s\n", str);
    while (!bug_dummy_check) {};
    }

static inline int cmder_printf (cmder_t * cmder, const char * format, ...)
    {
    int ret;
    va_list	valist;

    va_start (valist, format);
    ret = vprintf (format, valist);
    va_end (valist);

    return ret;
    }
#endif

static volatile bool test_exit = false;

static inline int __verify_chunk (cmder_t * cmder, heap_t * heap, chunk_t * chunk)
    {
    if ((chunk->prev != NULL) && (chunk != __get_next_chunk (chunk->prev)))
        {
        __bug ("chunk->prev->next != chunk!");
        }

    if (((uintptr_t) __get_mem_block (chunk)) & ALLOC_ALIGN_MASK)
        {
        __bug ("chunk->memblock not aligned!");
        }

    if (__is_free (chunk))
        {
        if (chunk->size < sizeof (chunk_t))
            {
            __bug ("chunk->size < sizeof (chunk_t)!");
            }

        if (__is_free (__get_prev_chunk (chunk)) ||
            __is_free (__get_next_chunk (chunk)))
            {
            __bug ("two neighbor free!");
            }
        }

#ifdef VERBOSE
    cmder_printf (cmder, "| %c_%p |", __is_free (chunk) ? 'F' : 'A', (void *) chunk->size);
#endif

    return 0;
    }

static inline int __verify_cb (cmder_t * cmder, heap_t * heap, block_t * cb)
    {
    chunk_t * chunk = (chunk_t *) (cb + 1);
    int       verified = 0;
    size_t    size = cb->size - sizeof (block_t);

    do
        {
        __verify_chunk (cmder, heap, chunk);

        size -= (chunk->size & ~1u);

        verified++;

        if (chunk->size == (sizeof (ach_t) | 1) &&
            chunk->prev != NULL)
            {
            if (size != 0)
                {
                __bug ("size not correct!");
                }

            return verified == 0 ? -1 : 0;
            }

        chunk = __get_next_chunk (chunk);
        } while (1);
    }

static int __verify_heap (cmder_t * cmder, heap_t * heap)
    {
    block_t       * cb;
    int             ret;

    mutex_lock (&heap->mux);

#ifdef CONFIG_HEAP_DEBUG
    size_t          i;

#ifdef CONFIG_BESTFIT
    unsigned char * c;

    c = (unsigned char *) &heap->cm.sn_guard0;

    for (i = 0; i < sizeof (size_node_t); i++)
        if (c [i] != 0x5c)
            __bug ("size node guard corrupted!");

    c = (unsigned char *) &heap->cm.sn_guard1;

    for (i = 0; i < sizeof (size_node_t); i++)
        if (c [i] != 0x5c)
            __bug ("size node guard corrupted!");
#endif

#ifdef CONFIG_TLSF
    chunk_mgr_t * cm = &heap->cm;
    uint32_t      fl_bmap = cm->fl_bmap >> FL_IDX_BIAS;

    if (cm->fl_bmap & ((1 << FL_IDX_BIAS) - 1))
        {
        __bug ("first level bitmap has unexpected bit set!");
        }

    for (i = 0; i < FL_IDXES; i++)
        {
        if (fl_bmap & (1 << i))
            {
            int        j;
            uint32_t   sl_bmap = cm->sl_bmap [i];
            chunk_t ** chunks  = cm->chunks [i];

            if (sl_bmap == 0)
                {
                __bug ("2nd bit map is zero while the associated 1st bit is 1");
                }

            if (sl_bmap >= (1 << (1 << SL_BITS)))
                {
                __bug ("second level bitmap has unexpected bit set!");
                }

            for (j = 0; j < 1 << SL_BITS; j++)
                {
                if (sl_bmap & (1 << j))
                    {
                    if (chunks [j] == NULL)
                        {
                        __bug ("2nd bitmap bit set, but list empty!");
                        }
                    }
                else
                    {
                    if (chunks [j] != NULL)
                        {
                        __bug ("2nd bitmap bit not set, but list nit empty!");
                        }
                    }
                }
            }
        else
            {
            int        j;
            uint32_t   sl_bmap = cm->sl_bmap [i];
            chunk_t ** chunks  = cm->chunks [i];

            if (sl_bmap != 0)
                {
                __bug ("1st bitmap bit not set but 2nd bitmap not zero!");
                }

            for (j = 0; j < 1 << SL_BITS; j++)
                {
                if (chunks [j] != NULL)
                    {
                    __bug ("2nd bitmap zero but list not empty!");
                    }
                }
            }
        }
#endif

#endif

    for (cb = heap->blocks; cb != NULL; cb = cb->next)
        {
        ret = __verify_cb (cmder, heap, cb);

#ifdef VERBOSE
        cmder_printf (cmder, "\n");
#endif

        if (ret < 0)
            {
            return ret;
            }
        }

    mutex_unlock (&heap->mux);

    return 0;
    }

static inline char * __alloc_test (cmder_t * cmder, size_t size, size_t align)
    {
    char * m;

    m = heap_alloc_align (kernel_heap, align, size);

#ifdef VERBOSE
    cmder_printf (cmder, "heap after alloc (0x%lx, 0x%lx):\n", size, align);
#endif

    if (__verify_heap (cmder, kernel_heap) == 0)
        {
#ifdef VERBOSE
        cmder_printf (cmder, "heap verify sucess\n");
#endif
        }
    else
        {
        cmder_printf (cmder, "heap verify fail\n");
        }

    return m;
    }

#define TEST_PTRS       (1024)
#define TEST_PTRS_MASK  (TEST_PTRS - 1)

static void __free_all (void ** ptrs)
    {
    int i;

    for (i = 0; i < TEST_PTRS; i++)
        {
        if (ptrs [i] == NULL)
            continue;

        heap_free (kernel_heap, ptrs [i]);
        ptrs [i] = NULL;
        }
    }

#ifndef __HOST__
struct test_info
    {
    volatile uint32_t test_cnt;
    volatile uint64_t test_size;
    };

#define NR_THREADS      4

#if NR_THREADS >= 10
#error "NR_THREADS shoud be less than 10"
#endif

static struct test_info test_info [NR_THREADS] = {0};
#endif

cmder_t * g_cmder = NULL;

static void __stress_test (int id)
    {
    static size_t aligns [32] =
        {
        4, 4, 4, 4, 4, 4, 4,                    /* 7 4s */
        8, 8, 8, 8, 8, 8,                       /* 6 8s */
        16, 16, 16, 16,                         /* 4  16s */
        32, 32, 32,
        64, 64, 64,
        128, 128,
        256, 256,
        512, 512,
        1024, 2048, 4096
        };

    cmder_t * cmder = g_cmder;

#ifndef __HOST__
    struct test_info * ti = test_info + id;
#endif

    int      idx;
    size_t   align;
    size_t   size;
    void  ** ptrs = malloc (sizeof (void *) * TEST_PTRS);

#ifdef __HOST__
    uint64_t counter = 0;
#else
    int      counter = 0;
#endif

    if (ptrs == NULL)
        {
        return;
        }

    memset (ptrs, 0, sizeof (void *) * TEST_PTRS);

#ifndef __HOST__
    task_delay (2);
#endif

     while (!test_exit)
        {
        counter++;

        if (__verify_heap (cmder, kernel_heap) == 0)
            {
#ifdef VERBOSE
            cmder_printf (cmder, "heap verify sucess\n");
#endif
            }
        else
            {
            __bug ("heap verify fail!");
            }

#ifdef __HOST__
#define PRINT_COUNT_SHIFT   17
#define PRINT_COUNT         (1 << PRINT_COUNT_SHIFT)
#define PRINT_COUNT_MASK    (PRINT_COUNT - 1)

        if (!(counter & PRINT_COUNT_MASK))
            {
            if (__verify_heap (cmder, kernel_heap) != 0)
                {
                __bug ("heap verify fail!");
                }

            cmder_printf (cmder, "%08lld x %08llx times\n", counter >> PRINT_COUNT_SHIFT, PRINT_COUNT);
            fflush (stdout);
            }
#endif

        if (!(counter & 0xfffff))
            {
            __free_all (ptrs);

            continue;
            }

        idx = rand () & TEST_PTRS_MASK;

        if (ptrs [idx])
            {
            if ((idx & 3) == 0)
                {
                char * new = heap_realloc (kernel_heap, ptrs [idx], aligns [(idx >> 5) & 31]);
                if (new != NULL)
                    {
                    ptrs [idx] = new;
                    }
                continue;
                }

            heap_free (kernel_heap, ptrs [idx]);
            ptrs [idx] = NULL;
            continue;
            }

        align = aligns [rand () & 31];

        size  = aligns [idx & 31];

        ptrs [idx] = heap_alloc_align (kernel_heap, align, size);

#ifndef __HOST__
        ti->test_cnt++;
        ti->test_size += size;
#endif
        }

    __free_all (ptrs);
    free (ptrs);
    }

static int __heap_test_task (int id)
    {
    char * m1, * m2, * m3, * m4;

    cmder_t * cmder = g_cmder;

    m1 = __alloc_test (cmder, 100, 4096);
    m4 = heap_realloc (kernel_heap, m1, 1);

    m1 = m4 == NULL ? m1 : m4;

#ifdef VERBOSE
    cmder_printf (cmder, "after realloc:\n");
#endif
    __verify_heap (cmder, kernel_heap);
    heap_free (kernel_heap, m1);

#ifdef VERBOSE
    cmder_printf (cmder, "heap before test:\n");
#endif

    __verify_heap (cmder, kernel_heap);
    m1 = __alloc_test (cmder, 100, 4096);
    m2 = __alloc_test (cmder, 4096 - 256 + 8, 4096);
    m3 = __alloc_test (cmder, 60, 128);

    m4 = heap_realloc (kernel_heap, m1, 104);

    m1 = m4 == NULL ? m1 : m4;
    m4 = heap_realloc (kernel_heap, m1, 105);

    m1 = m4 == NULL ? m1 : m4;
    m4 = heap_realloc (kernel_heap, m1, 1);

    m1 = m4 == NULL ? m1 : m4;

    heap_free (kernel_heap, m1);
    __verify_heap (cmder, kernel_heap);
    heap_free (kernel_heap, m2);
    __verify_heap (cmder, kernel_heap);
    heap_free (kernel_heap, m3);
    __verify_heap (cmder, kernel_heap);

    m1 = __alloc_test (cmder, 1, 2);
    m2 = __alloc_test (cmder, 1, 4);
    __verify_heap (cmder, kernel_heap);
    heap_free (kernel_heap, m1);
    heap_free (kernel_heap, m2);
    __verify_heap (cmder, kernel_heap);

    __stress_test (id);

    return 0;
    }

#ifdef __HOST__
int main (int argc, char * argv [])
    {
    heap_lib_init ();

#ifdef __INIT__
    heap_init (kernel_heap);
#else
    kernel_heap = heap_create ();
#endif

    if (heap_add (kernel_heap, (char *) buff, sizeof (buff)) != 0)
        {
        printf ("heap init fail!\n");
        return -1;
        }

    __heap_test_task (0);
    }
#endif

#ifdef CONFIG_CMDER

static int __cmd_heap_test (cmder_t * cmder, int argc, char * argv [])
    {
    static bool running = false;

    int         i;
    char        name [8];

#ifdef __NEWLIB__

    /* rand will invoke malloc, invoke it before test start ensure no racing */

    (void) rand ();
#endif

    g_cmder = cmder;

    if (!running)
        {
        for (i = 0; i < NR_THREADS; i++)
            {
            sprintf (name, "heap_t%d", i);

            task_spawn (name, 20, 0, 0x400,
                        (int (*) (uintptr_t)) __heap_test_task,
                        (uintptr_t) i);
            }

        running = true;

        test_exit = false;
        }
    else
        {
        for (i = 0; i < NR_THREADS; i++)
            {
            cmder_printf (cmder, "task-%d heap test info:\n");
            cmder_printf (cmder, "  test times: 0x%08x\n",    test_info [i].test_cnt);
            cmder_printf (cmder, "  test size:  0x%016llx\n", test_info [i].test_size);
            }
        }

    return 0;
    }

CMDER_CMD_DEF ("heap_test", "heap test routine", __cmd_heap_test);

#endif

