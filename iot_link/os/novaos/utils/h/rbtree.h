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

#ifndef __RBTREE_H__
#define __RBTREE_H__

#include "private/bi_tree.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

#define RBTREE_RED      0
#define RBTREE_BLACK    1

#define RBTREE_DEBUG

/* typedefs */

typedef struct rb_node
    {
    bi_node_t  bin;
    int        c;
    } rb_node_t;

typedef struct rb_tree
    {
    bi_tree_t  bit;

#ifdef RBTREE_DEBUG
    uint32_t   nodes;
#endif
    } rb_tree_t;

/* inlines */

extern int         rb_insert   (rb_tree_t * t, rb_node_t * n);
extern void        rb_delete   (rb_tree_t * t, rb_node_t * n);
extern void        rb_init     (rb_tree_t * t,
                                int (* compare_nn) (bi_node_t *, bi_node_t *),
                                int (* compare_nk) (bi_node_t *, uintptr_t));
extern rb_node_t * rb_find_eq  (rb_tree_t * t, uintptr_t k);
extern rb_node_t * rb_find_ge  (rb_tree_t * t, uintptr_t k);
extern rb_node_t * rb_find_gt  (rb_tree_t * t, uintptr_t k);
extern rb_node_t * rb_find_le  (rb_tree_t * t, uintptr_t k);
extern rb_node_t * rb_find_lt  (rb_tree_t * t, uintptr_t k);
extern rb_node_t * rb_first    (rb_tree_t * t);
extern rb_node_t * rb_last     (rb_tree_t * t);
extern rb_node_t * rb_next     (rb_node_t * n);
extern rb_node_t * rb_prev     (rb_node_t * n);
extern rb_node_t * rb_node_get (rb_tree_t * t, uintptr_t k,
                                bi_node_t * (* create) (uintptr_t, uintptr_t),
                                uintptr_t arg);
extern void        rb_replace  (rb_tree_t * t, rb_node_t * o, rb_node_t * n);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RBTREE_H__ */

