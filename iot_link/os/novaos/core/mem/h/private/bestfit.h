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

#ifndef __CM_BESTFIT_H__
#define __CM_BESTFIT_H__

#include <string.h>

#include <arch/config.h>

#include <common.h>
#include <rbtree.h>
#include <list.h>

#include "chunk.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct size_node
    {
    rb_node_t          node;    /* 3 x ptr + 1 x int */
    size_t             size;    /* 1 x long */
    dlist_t            list;    /* 2 x ptr */
    } size_node_t;

typedef struct chunk_mgr
    {
    rb_tree_t          sizes;

    /*
     * only when chunk->size < (sizeof (chunk_t) + sizeof (size_node_t))
     * size_nodes[n] will be used. And:
     *
     * 1) chunk->size is n * ALLOC_ALIGN
     * 2) chunks with chunk->size == sizeof (chunk_t) will not be inserted
     *
     * so size_nodes[0] for chunk->size = sizeof (chunk_t)
     *    size_nodes[1] for chunk->size = sizeof (chunk_t) + ALLOC_ALIGN
     *    size_nodes[2] for chunk->size = sizeof (chunk_t) + ALLOC_ALIGN * 2
     *    ...
     *    size_nodes[n] for chunk->size = sizeof (chunk_t) + sizeof (chunk_t) - ALLOC_ALIGN
     *
     * so
     *
     *    elements = sizeof (size_node_t) / ALLOC_ALIGN;
     *
     *    idx = (chunk->size - sizeof (chunk_t)) / ALLOC_ALIGN
     */

#ifdef CONFIG_HEAP_DEBUG
    size_node_t        sn_guard0;
#endif

    size_node_t        size_nodes [round_up (sizeof (size_node_t), ALLOC_ALIGN) /
                                   ALLOC_ALIGN];

#ifdef CONFIG_HEAP_DEBUG
    size_node_t        sn_guard1;
#endif
    } chunk_mgr_t;

/**
 * __heap_compare_nk - compare size_node with a key
 */

static int __heap_compare_nk (bi_node_t * n, uintptr_t k)
    {
    size_node_t * sn = container_of (n, size_node_t, node.bin);

    if ((uintptr_t) sn->size == k)
        {
        return 0;
        }

    return (uintptr_t) sn->size > k ? 1 : -1;
    }

/**
 * __heap_compare_nn - compare two size_nodes
 */

static int __heap_compare_nn (bi_node_t * a, bi_node_t * b)
    {
    return __heap_compare_nk (a, (uintptr_t) container_of (b, size_node_t, node.bin)->size);
    }

/**
 * __cm_init - initialize chunk manager
 * @cm: the given chunk manager
 *
 * return: 0 on success, negtive value on error
 */

static inline int __cm_init (chunk_mgr_t * cm)
    {
#ifdef CONFIG_HEAP_DEBUG
    memset (&cm->sn_guard0, 0x5c, sizeof (struct size_node));
    memset (&cm->sn_guard1, 0x5c, sizeof (struct size_node));
#endif

    rb_init (&cm->sizes, __heap_compare_nn, __heap_compare_nk);

    return 0;
    }

static inline size_node_t * __get_size_node_from_chunk (chunk_t * chunk)
    {
    return (size_node_t *) (chunk + 1);
    }

/**
 * __get_size_node - get the size node for a given size
 * @heap:  the given heap
 * @chunk: the chunk contains the size
 *
 * return: the size node
 */

static inline size_node_t * __get_size_node (chunk_mgr_t * cm, chunk_t * chunk)
    {
    if (chunk->size >= (sizeof (size_node_t) + sizeof (chunk_t)))
        {
        return __get_size_node_from_chunk (chunk);
        }

    /* please refer to the comments for size_nodes in the struct defination */

    return &cm->size_nodes [(chunk->size - sizeof (chunk_t)) / ALLOC_ALIGN];
    }

/**
 * __put_size_node - put the size node for a given size
 * @heap: the given heap
 * @sn:   the size node to put
 *
 * return: NA
 */

static inline void __put_size_node (chunk_mgr_t * cm, size_node_t * sn)
    {

    /* do nothing, refer to __get_size_node */

    return;
    }

/**
 * __get_bi_node - bi_node create routine for binary search tree
 * @key: the key value
 * @arg: the chunk the node is created for (from)
 *
 * return: the bi_node for the chunk
 */

static bi_node_t * __get_bi_node (uintptr_t key, uintptr_t arg)
    {
    size_node_t * szn = (size_node_t *) arg;

    szn->size = key;
    dlist_init (&szn->list);

    return &szn->node.bin;
    }

/**
 * __cm_put_chunk - put a free chunk to a heap
 * @heap:  the given heap
 * @chunk: the chunk to insert
 *
 * return: NA
 */

static inline void __cm_put_chunk (chunk_mgr_t * cm, chunk_t * chunk)
    {
    rb_node_t   * rbn;
    size_node_t * szn;

    rbn = rb_node_get (&cm->sizes, (uintptr_t) chunk->size, __get_bi_node,
                       (uintptr_t) __get_size_node (cm, chunk));

    szn = container_of (rbn, size_node_t, node);

    dlist_add (&szn->list, &chunk->node);
    }

/**
 * __cm_del_chunk - delete a chunk from a heap (for allocating or merging)
 * @heap:  the given heap
 * @chunk: the chunk to delete
 *
 * return: NA
 */

static inline void __cm_del_chunk (chunk_mgr_t * cm, chunk_t * chunk)
    {
    dlist_t     * prev = chunk->node.prev;
    size_node_t * sn;
    size_node_t * nsn;

    dlist_del (&chunk->node);

    if (dlist_empty (prev))
        {

        /* 'prev' must be &size_node->list */

        sn = container_of (prev, size_node_t, list);

        rb_delete (&cm->sizes, &sn->node);

        __put_size_node (cm, sn);

        return;
        }

    /*
     * as most of size nodes are allocated in the memory block of chuck, we may
     * deleting a chunk that is just holding the size node, this can only happen
     * when merging free blocks, because chunk are always added from head, so the
     * chunk holds the size node are always the last one, but we are always get
     * new chunk from head
     *
     * here check this condition and carefully handle it
     */

    /* if size node is from heap->size_nodes [], do nothing */

    if (chunk->size < sizeof (chunk_t) + sizeof (size_node_t))
        {
        return;
        }

    sn = __get_size_node_from_chunk (chunk);

    /*
     * chunk are always linked to size node from head, and the chunk holds the
     * size node is just the first one added to the list, so it is always the
     * list tail, and the 'next' of tail is just the 'head', if this condition
     * is not true, we must not deleting chunk holding size node
     */

    if (chunk->node.next != &(sn->list))
        {
        return;
        }

    /*
     * the chunk being deleted is just holding the size_node and the size list
     * is not empty now
     *
     * we should find another size node in the size_node->list, every chunk in
     * the list can be used as all of them have the same size, here we just
     * pick the 'prev' chunk, because it is the new 'tail' after deleting this
     * chunk
     */

    /* delete the list head (which is in the size_node) */

    dlist_del (&sn->list);

    /* get a new size_node */

    nsn = __get_size_node_from_chunk (container_of (prev, chunk_t, node));

    /*
     * made the new list head, 'prev' is now the new 'tail' and 'nsn->list' is
     * just the new 'head', so just add 'nsn->list' after 'prev'
     */

    dlist_add (prev, &nsn->list);

    nsn->size = sn->size;

    rb_replace (&cm->sizes, &sn->node, &nsn->node);
    }

/**
 * __cm_get_chunk - get a proper chunk in the heap
 * @heap:  the heap to allocate from
 * @bytes: size of memory in bytes to allocate
 *
 * return: the proper chunk found, or NULL if not found
 */

static inline chunk_t * __cm_get_chunk (chunk_mgr_t * cm, size_t bytes)
    {
    rb_node_t * rbn = rb_find_ge (&cm->sizes, bytes);
    chunk_t   * chunk;

    if (rbn == NULL)
        {
        return NULL;
        }

    chunk = container_of (container_of (rbn, size_node_t, node)->list.next,
                          chunk_t, node);

    return chunk;
    }

#ifdef CONFIG_MEM_STATISTICS
static inline size_t __get_max_free (chunk_mgr_t * cm)
    {
    rb_node_t * rbn = rb_last (&cm->sizes);
    chunk_t   * chunk;

    if (rbn == NULL)
        {
        return 0;
        }

    chunk = container_of (container_of (rbn, size_node_t, node)->list.next,
                          chunk_t, node);

    return chunk->size;
    }
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CM_BESTFIT_H__ */

