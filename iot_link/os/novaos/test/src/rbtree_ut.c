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
 * note: this file is tested on a x64 pc using "win10+msys2+gnuc" because the
 * test spent huge resource and the code, logic is common
 *
 * the commands:
 *
 *     gcc rbtree_ut.c -I../../include -I../../utils/h/ -I../h/ -O3 -o rbtree_ut
 *
 * this should works fine under Linux machines
 */

#include <stdio.h>
#include <stdlib.h>

#include "../../utils/src/rbtree.c"

volatile int __bug_dummy_check = 0;

static void __bug (const char * str)
    {
    printf ("fatal error, bug: %s\n", str);
    while (__bug_dummy_check == 0) {}
    }

enum color { red = 0, black = 1};

struct rbt_node
    {
    union
        {
        struct
            {
            struct rbt_node * p;
            struct rbt_node * l;
            struct rbt_node * r;
            enum color        c;
            };
        rb_node_t             rbn;
        };
    uintptr_t                 key;
    };

struct rbt_node dummy_no_used;

struct rb_tree  rbt;

static int rbt_compare_nk (bi_node_t * n, uintptr_t k)
    {
    struct rbt_node * na = (struct rbt_node *) n;

    if (na->key < k) return -1;
    if (na->key > k) return 1;

    return 0;
    }

static int rbt_compare_nn (bi_node_t * a, bi_node_t * b)
    {
    return rbt_compare_nk (a, ((struct rbt_node *) b)->key);
    }

static bi_node_t * rbt_create (uintptr_t key, uintptr_t arg)
    {
    struct rbt_node * rbn = (struct rbt_node *) malloc (sizeof (struct rbt_node));

    (void) arg;

    rbn->key = key;

    return &rbn->rbn.bin;
    }

static inline void rbt_insert (uintptr_t key)
    {
#if 0
    struct rbt_node * rbn = (struct rbt_node *) malloc (sizeof (struct rbt_node));

    rbn->key = key;

    rb_insert (&rbt, &rbn->rbn);
#endif

    rb_node_get (&rbt, key, rbt_create, 0);
    }

static inline void rbt_delete(uintptr_t key)
    {
    struct rbt_node * rbn = container_of (rb_find_eq (&rbt, key), struct rbt_node, rbn);

    rb_delete (&rbt, &rbn->rbn);

    free (rbn);
    }

volatile uintptr_t last = 0;
uint32_t nr_nodes;

static int __walk_func (bi_node_t * n)
    {
    struct rbt_node * rbn = container_of (n, struct rbt_node, rbn.bin);

#if 0
    printf ("%d ", rbn->key);
#endif

    nr_nodes++;

    if ((rbn->key == 0) && (last == 0))
        {
        return 0;
        }

    if (last >= rbn->key)
        {
        __bug ("not greater than last!");
        }

    last = rbn->key;

    return 0;
    }

static void __walk_all (void)
    {
    __bit_for_each (&rbt.bit, __walk_func);
    }

static int __rb_verify (rb_node_t * n, int depth)
    {
    if ((__is_red (n)) && (__is_red (__left (n)) || __is_red (__right (n))))
        {
        __bug ("parent & child both red!");
        }

    if (((__left  (n) != NULL) && (__raw_is_black (__left  (n))) && (__right (n) == NULL)) ||
        ((__right (n) != NULL) && (__raw_is_black (__right (n))) && (__left  (n) == NULL)))
        {
        __bug ("shoud not be only one black child!");
        }

    if (__raw_is_black (n))
        {
        depth++;
        }

    if (__left (n) != NULL)
        {
        if (__parent (__left (n)) != n)
            {
            __bug ("child's parent not correct!");
            }
        }

    if (__right (n) != NULL)
        {
        if (__parent (__right (n)) != n)
            {
            __bug ("child's parent not correct!");
            }
        }

    if (__left (n) == NULL)
        {
        if (__right (n) != NULL)
            {
            n = __right (n);

            if ((__left (n) != NULL) || (__right (n) != NULL))
                {
                __bug ("only one red child must be leaf");
                }
            }
        }
    else
        {
        if (__right (n) != NULL)
            {
            int dl = __rb_verify (__left  (n), depth);
            int dr = __rb_verify (__right (n), depth);

            if (dl != dr)
                {
                __bug ("two sub path have different black!");
                }

            depth = dl;
            }
        else
            {
            n = __left (n);

            if ((__left (n) != NULL) || (__right (n) != NULL))
                {
                __bug ("only one red child must be leaf");
                }
            }
        }

    return depth;
    }

int rb_verify (rb_tree_t * t)
    {
    rb_node_t * r;
    int              n;

    if (t == NULL)
        {
        __bug ("tree is NULL");
        }

    r = container_of (t->bit.r, rb_node_t, bin);

    if (!r)
        {
        return 0;
        }

    if (!__raw_is_black (r))
        {
        __bug ("root is not black");
        }

    (void) __rb_verify (r, 0);

    return n;
    }

#define SHOW_MASK       0xffff

void __stress_test (void)
    {
    int  i;

    volatile char a [1024] = {0};

    volatile int elements = 0;
    volatile int add = 0, del = 0;

    while (1)
        {
        i = rand () & 1023;

        if (a [i] == 0)
            {
            rbt_insert (i);
            a [i] = 1;

            elements++;

            add++;
            }
        else
            {
            rbt_delete (i);
            a [i] = 0;

            elements--;

            ++del;
            }

        if (rb_verify (&rbt) < 0)
            {
            __bug ("verify fail!");
            }

        if (((add + del) & SHOW_MASK) == 0)
            {
            static uint32_t shape_idx = 0;

            last = 0;
            nr_nodes = 0;

            printf ("%c\r", "-\\|/" [shape_idx++ & 3]);
            fflush (stdout);

            __walk_all ();

            if (nr_nodes != rbt.nodes)
                {
                printf ("%d - %d:", nr_nodes, rbt.nodes);
                __bug ("number of foreach not correct!");
                }
            }
        }
    }

static void check_result (int i)
    {
    if (i)
        {
        printf ("PASS\n");
        }
    else
        {
        printf ("FAIL\n");
        }
    }

int main (void)
    {
    int               i;

    rb_init (&rbt, rbt_compare_nn, rbt_compare_nk);

    rbt_insert (4);
    rbt_insert (2);
    rbt_insert (7);
    rbt_insert (1);
    rbt_insert (3);
    rbt_insert (6);
    rbt_insert (8);
    rbt_insert (5);

    rb_verify (&rbt);
    rbt_delete (8);
    rb_verify (&rbt);
    rbt_delete (1);
    rb_verify (&rbt);
    rbt_delete (3);
    rb_verify (&rbt);
    rbt_delete (7);
    rb_verify (&rbt);
    rbt_delete (6);
    rb_verify (&rbt);
    rbt_delete (5);
    rb_verify (&rbt);
    rbt_delete (2);
    rb_verify (&rbt);
    rbt_delete (0);
    rb_verify (&rbt);
    rbt_delete (4);
    rb_verify (&rbt);

    for (i = 0; i < 100; i += 2)
        {
        rbt_insert (i);
        }

    check_result (container_of (rb_find_ge (&rbt, 51), struct rbt_node, rbn)->key == 52);
    check_result (container_of (rb_find_ge (&rbt, 54), struct rbt_node, rbn)->key == 54);
    check_result (container_of (rb_find_ge (&rbt, 0),  struct rbt_node, rbn)->key == 0);
    check_result (container_of (rb_find_ge (&rbt, 98), struct rbt_node, rbn)->key == 98);

    check_result (container_of (rb_find_gt (&rbt, 41), struct rbt_node, rbn)->key == 42);
    check_result (container_of (rb_find_gt (&rbt, 44), struct rbt_node, rbn)->key == 46);
    check_result (container_of (rb_find_gt (&rbt, 0),  struct rbt_node, rbn)->key == 2);
    check_result (container_of (rb_find_gt (&rbt, 98), struct rbt_node, rbn)      == NULL);

    check_result (container_of (rb_find_lt (&rbt, 61), struct rbt_node, rbn)->key == 60);
    check_result (container_of (rb_find_lt (&rbt, 64), struct rbt_node, rbn)->key == 62);
    check_result (container_of (rb_find_lt (&rbt, 98), struct rbt_node, rbn)->key == 96);
    check_result (container_of (rb_find_lt (&rbt, 0),  struct rbt_node, rbn)      == NULL);

    check_result (container_of (rb_find_le (&rbt, 21), struct rbt_node, rbn)->key == 20);
    check_result (container_of (rb_find_le (&rbt, 24), struct rbt_node, rbn)->key == 24);
    check_result (container_of (rb_find_le (&rbt, 98), struct rbt_node, rbn)->key == 98);
    check_result (container_of (rb_find_le (&rbt, 0),  struct rbt_node, rbn)->key == 0);

    check_result (container_of (rb_first (&rbt),  struct rbt_node, rbn)->key == 0);
    check_result (container_of (rb_last (&rbt),  struct rbt_node, rbn)->key == 98);


    check_result (container_of (rb_next (rb_find_le (&rbt, 24)), struct rbt_node, rbn)->key == 26);
    check_result (container_of (rb_prev (rb_find_le (&rbt, 24)), struct rbt_node, rbn)->key == 22);

    for (i = 0; i < 100; i += 2)
        {
        rbt_delete (i);
        }

    __stress_test ();

    return 0;
    }

