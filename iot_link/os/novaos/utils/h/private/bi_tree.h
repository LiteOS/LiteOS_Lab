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

#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct bi_node
    {
    struct bi_node     * p;     /* parent       */
    struct bi_node     * l;     /* left         */
    struct bi_node     * r;     /* right        */
    } bi_node_t;

typedef struct bi_tree
    {
    bi_node_t          * r;     /* root         */

    /*
     * compare two nodes or a node with a key to determine which is bigger,
     *
     * returning negative values means 'a < b' or 'n->k < k'
     * returning positive values means 'a > b' or 'n->k > k'
     * returning zero means 'a == b' or 'n->k == k'
     */

    int               (* compare_nn) (bi_node_t * a, bi_node_t * b);
    int               (* compare_nk) (bi_node_t * n, uintptr_t k);
    } bi_tree_t;

/* defines */

/**
 * __bit_parent - get the parent for a node, assume n is not NULL
 * @n: the given node
 *
 * return: the parent of the node <n>
 */

static __always_inline bi_node_t * __bit_parent (bi_node_t * n)
    {
    return n->p;
    }

/**
 * __bit_set_parent - set the parent for a node, assume n is not NULL
 * @n: the given node
 *
 * return: NA
 */

static __always_inline void __bit_set_parent (bi_node_t * n, bi_node_t * p)
    {
    n->p = p;
    }

/**
 * __bit_raw_grand - get the grand parent for a node, assume n and p(n) are not NULL
 * @n: the given node
 *
 * return: the grand parent of the node <n>
 */

static __always_inline bi_node_t * __bit_raw_grand (bi_node_t * n)
    {
    return __bit_parent (__bit_parent (n));
    }

/**
 * __bit_grand - get the grand parent for a node
 * @n: the given node
 *
 * return: the grand parent of the node <n>, NULL if n or p(n) is NULL
 */

static __always_inline bi_node_t * __bit_grand (bi_node_t * n)
    {
    bi_node_t * p;

    if ((n == NULL) || ((p = __bit_parent (n)) == NULL))
        {
        return NULL;
        }

    return __bit_parent (p);
    }

/**
 * __bit_raw_uncle - get the uncle for a node, assume n, p(n) and g(n) are not NULL
 * @n: the given node
 *
 * return: the uncle of the node <n>
 */

static __always_inline bi_node_t * __bit_raw_uncle (bi_node_t * n)
    {
    bi_node_t * p = __bit_parent (n);
    bi_node_t * g = __bit_raw_grand (n);

    return p == g->l ? g->r : g->l;
    }

/**
 * __bit_uncle - get the uncle for a node
 * @n: the given node
 *
 * return: the uncle of the node <n>, NULL if n, p(n) or g(n) is NULL
 */

static __always_inline bi_node_t * __bit_uncle (bi_node_t * n)
    {
    bi_node_t * p;
    bi_node_t * g;

    if (n == NULL)
        {
        return NULL;
        }

    if ((p = __bit_parent (n)) == NULL)
        {
        return NULL;
        }

    if ((g = __bit_parent (p)) == NULL)
        {
        return NULL;
        }

    return p == g->l ? g->r : g->l;
    }

/**
 * __bit_successor - the successor node of a node
 * @n: the given node
 *
 * return: the successor of <n>
 */

static __always_inline bi_node_t * __bit_successor (bi_node_t * n)
    {
    bi_node_t * s = n->r;

    if (s != NULL)
        {
        while (s->l)
            {
            s = s->l;
            }

        return s;
        }

    while (n->p && (n == n->p->r))
        {
        n = n->p;
        }

    return n->p;
    }

/**
 * __bit_predecessor - the predecessor node of a node
 * @n: the given node
 *
 * return: the predecessor of <n>
 */

static __always_inline bi_node_t * __bit_predecessor (bi_node_t * n)
    {
    bi_node_t * p = n->l;

    if (p != NULL)
        {
        while (p->r)
            {
            p = p->r;
            }

        return p;
        }

    while (n->p && (n == n->p->l))
        {
        n = n->p;
        }

    return n->p;
    }

/**
 * __bit_find_eq - find a given key in a tree
 * @r: the given tree root
 * @k: the key value
 *
 * return: the node with key = <k>, or NULL if key not found.
 */

static __always_inline bi_node_t * __bit_find_eq (bi_tree_t * t, uintptr_t k)
    {
    bi_node_t * w = t->r;

    while (w != NULL)
        {
        int          c = t->compare_nk (w, k);

        if (c == 0)
            {
            break;
            }

        w = c < 0 ? w->r : w->l;
        }

    return w;
    }

/**
 * __bit_find_gt - find a node that is just greater to the given value
 * @t: the given tree
 * @k: the key value
 *
 * return: the nearest node with key > <k>, NULL or if key not found.
 */

static __always_inline bi_node_t * __bit_find_gt (bi_tree_t * t, uintptr_t k)
    {
    bi_node_t * l = NULL;   /* last */
    bi_node_t * w = t->r;

    while (w != NULL)
        {
        int     c = t->compare_nk (w, k);

        if (c > 0)
            {
            l = w;
            w = w->l;
            }
        else
            {
            w = w->r;
            }
        }

    return l;
    }

/**
 * __bit_find_lt - find a node that is just less to the given value
 * @t: the given tree
 * @k: the key value
 *
 * return: the nearest node with key < <k>, NULL or if key not found.
 */

static __always_inline bi_node_t * __bit_find_lt (bi_tree_t * t, uintptr_t k)
    {
    bi_node_t * l = NULL;   /* last */
    bi_node_t * w = t->r;

    while (w != NULL)
        {
        int     c = t->compare_nk (w, k);

        if (c < 0)
            {
            l = w;
            w = w->r;
            }
        else
            {
            w = w->l;
            }
        }

    return l;
    }

/**
 * __bit_first - find the first node of the tree
 * @t: the given tree
 *
 * return: the first node, or NULL if the tree is empty
 */

static __always_inline bi_node_t * __bit_first (bi_tree_t * t)
    {
    bi_node_t * n = t->r;

    if (likely (n != NULL))
        {
        while (likely (n->l != NULL))
            {
            n = n->l;
            }
        }

    return n;
    }

/**
 * __bit_last - find the last node of the tree
 * @t: the given tree
 *
 * return: the last node, or NULL if the tree is empty
 */

static __always_inline bi_node_t * __bit_last (bi_tree_t * t)
    {
    bi_node_t * n = t->r;

    if (likely (n != NULL))
        {
        while (likely (n->r != NULL))
            {
            n = n->r;
            }
        }

    return n;
    }

/**
 * __bit_find_ge - find a node that is just greater or equal to the given value
 * @t: the given tree
 * @k: the key value
 *
 * return: the nearest node with key >= <k>, or NULL if key not found.
 */

static __always_inline bi_node_t * __bit_find_ge (bi_tree_t * t, uintptr_t k)
    {
    bi_node_t * l = NULL;   /* last */
    bi_node_t * w = t->r;

    while (w != NULL)
        {
        int     c = t->compare_nk (w, k);

        if (c == 0)
            {
            return w;
            }

        if (c > 0)
            {
            l = w;
            w = w->l;
            }
        else
            {
            w = w->r;
            }
        }

    return l;
    }

/**
 * __bit_find_le - find a node that is just less or equal to the given value
 * @t: the given tree
 * @k: the key value
 *
 * return: the nearest node with key <= <k>, of NULL if key not found.
 */

static __always_inline bi_node_t * __bit_find_le (bi_tree_t * t, uintptr_t k)
    {
    bi_node_t * l = NULL;   /* last */
    bi_node_t * w = t->r;

    while (w != NULL)
        {
        int     c = t->compare_nk (w, k);

        if (c == 0)
            {
            return w;
            }

        if (c < 0)
            {
            l = w;
            w = w->r;
            }
        else
            {
            w = w->l;
            }
        }

    return l;
    }

/**
 * __bit_node_get - find a existing node or create a new one for it
 * @r:       the given tree root
 * @k:       the key value
 * @created: pointer to a bool value, set to true if node not found and created
 *           a new one
 * @create:  the node creating routine, <k> is the seconde argument
 * @arg:     the first argument of the <create> routine
 *
 * return: the node with key = <k>, or NULL if node not found and allocate fail.
 */

static __always_inline bi_node_t * __bit_node_get (bi_tree_t * t, uintptr_t k,
                                                   bool * created,
                                                   bi_node_t * (* create) (uintptr_t, uintptr_t),
                                                   uintptr_t arg)
    {
    bi_node_t *  w = t->r;      /* walker, iterator */
    bi_node_t *  l;             /* last one */
    bi_node_t ** p;
    int          c;

    if (w == NULL)
        {
        w = create (k, arg);

        w->p = w->l = w->r = NULL;
        t->r = w;

        *created = true;

        return w;
        }

    while (1)
        {
        c = t->compare_nk (w, k);

        if (c == 0)
            {
            break;
            }

        l = w;

        p = c < 0 ? &l->r : &l->l;
        w = *p;

        if (w == NULL)
            {
            w = create (k, arg);

            if (w != NULL)
                {
                w->p = l;
                w->l = w->r = NULL;

                *p   = w;

                *created = true;
                }

            break;
            }
        }

    return w;
    }

/**
 * __bit_rotate_right - rotate a sub-tree right
 * @T: the given tree
 * @n: the sub-tree
 *
 *      n              a
 *     / \            / \
 *    a   x    ->    y   n
 *   / \                / \
 *  y   z              z   x
 *
 * return: NA
 */

static __always_inline void __bit_rotate_right (bi_tree_t * t, bi_node_t * n)
    {
    bi_node_t * a = n->l;
    bi_node_t * p = __bit_parent (n);

    if (a->r)       /* check if 'z' exist  */
        {
        __bit_set_parent (a->r, n);
        }

    __bit_set_parent (n, a);
    n->l = a->r;
    a->r = n;

    __bit_set_parent (a, p);

    if (unlikely (p == NULL))
        {
        t->r = a;
        }
    else if (n == p->l)
        {
        p->l = a;
        }
    else
        {
        p->r = a;
        }
    }

/**
 * __bit_rotate_left - rotate a sub-tree left
 * @T: the given tree
 * @n: the sub-tree
 *
 *    n                  a
 *   / \                / \
 *  x   a      ->      n   y
 *     / \            / \
 *    z   y          x   z
 */

static __always_inline void __bit_rotate_left (bi_tree_t * t, bi_node_t * n)
    {
    bi_node_t * a = n->r;
    bi_node_t * p = __bit_parent (n);

    if (a->l)       /* check if 'z' exist  */
        {
        __bit_set_parent (a->l, n);
        }

    __bit_set_parent (n, a);
    n->r = a->l;
    a->l = n;

    __bit_set_parent (a, p);

    if (unlikely (p == NULL))
        {
        t->r = a;
        }
    else if (n == p->l)
        {
        p->l = a;
        }
    else
        {
        p->r = a;
        }
    }

/**
 * __bit_insert - insert a new node into a tree
 * @t: the given tree
 * @n: the new inserted node
 *
 * return: 0 insert done, -1 insert fail
 */

static __always_inline int __bit_insert (bi_tree_t * t, bi_node_t * n)
    {
    bi_node_t * p = t->r;

    n->r = n->l = NULL;

    if (unlikely (p == NULL))
        {
        n->p = (bi_node_t *) NULL;
        t->r = n;

        return 0;
        }

    while (1)
        {
        int c = t->compare_nn (n, p);

        if (c == 0)
            {
            return -1;
            }

        if (c > 0)
            {
            if (p->r == NULL)
                {
                p->r = n;
                break;
                }

            p = p->r;
            }
        else
            {
            if (p->l == NULL)
                {
                p->l = n;
                break;
                }

            p = p->l;
            }
        }

    n->p = p;

    return 0;
    }

/**
 * __bit_for_each - iterate over a binary search tree from most left to most right
 * @t:   the given tree
 * @pfn: the callback function for each found node, when pfn return non-zero value
 *       the loop end
 *
 * return: NA
 */

static __always_inline void __bit_for_each (bi_tree_t * t, int (* pfn) (bi_node_t *))
    {
    bi_node_t * n = t->r;

    if (n == NULL)
        {
        return;
        }

    while (1)
        {
        while (n->l != NULL)
            {
            n = n->l;
            }

        while (1)
            {
            if (pfn (n))
                {
                return;
                }

            if (n->r != NULL)
                {
                n = n->r;
                break;
                }

            while (1)
                {
                if (n->p == NULL)
                    {
                    return;
                    }

                if (n == n->p->l)
                    {
                    n = n->p;
                    break;
                    }

                n = n->p;
                }
            }
        }
    }

/**
 * __bit_init - init a bit-tree
 * @t: the given tree
 * @c: the comparison method
 * @k: the method to get the key of a node
 *
 * return: 0 on success, -1 on error
 */

static __always_inline int __bit_init (bi_tree_t * t,
                                       int (* compare_nn) (bi_node_t *, bi_node_t *),
                                       int (* compare_nk) (bi_node_t *, uintptr_t))
    {
    t->r = NULL;

    if (!compare_nn || !compare_nk)
        {
        return -1;
        }

    t->compare_nn = compare_nn;
    t->compare_nk = compare_nk;

    return 0;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BINARY_TREE_H__ */

