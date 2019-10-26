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

#ifndef __CM_TLSF_H__
#define __CM_TLSF_H__

#include <config.h>

#include <common.h>
#include <list.h>
#include <bug.h>

#include "chunk.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * chunks are inserted to chunk lists in a 2d "chunk_t *" array,
 * "chunks [max_nr_fl] [max_nr_sl]" according to their size:
 *
 * the index in the 1st level is the last (most-significant) bit set:
 *
 *     fl_idx = fls (size);
 *
 * the index in the 2nd level is SL_BITS bits followed the bit:
 *
 *     sl_idx = (size - (1 << fl_idx)) >> (fl_idx - SL_BITS) =
 *            = (size >> (fl_idx - SL_BITS)) - (1 << SL_BITS);
 *
 * given a size with the value of 74, its binary value is:
 *
 *     size = 74 = 0b01001010:
 *
 * fl_idx = 6
 *          |
 *        7 6 5 4 3 2 1 0
 *      +-----------------+
 *      | 0 1 0 0 1 0 1 0 |
 *      +-----------------+
 *        7 6 5 4 3 2 1 0
 * sl_idx =   0 0          0 if SL_BITS = 2
 * sl_idx =   0 0 1        1 if SL_BITS = 3
 * sl_idx =   0 0 1 0      2 if SL_BITS = 4
 *
 * but we only support 2 or 3 as the value of SL_BITS, so sl_bmap can be a
 * array of uint8_t
 */

#define SL_BITS                 2   /* valid value only 2 or 3 */

/*
 * the minimum chunks managed is sizeof (chunk_t) which size is:
 *
 *     4 * sizeof (uintptr_t)
 *
 * so fl_idx is always greater or equal to:
 *
 *     fls (sizeof (chunk_t))
 *
 * so fl_idx from [0 ~ fls (sizeof (chunk_t)) - 1] will never be used for
 * space saving purpose, we use the following index map:
 *
 *     real idx - fl_idx
 *            0 - fls (sizeof (chunk_t))
 *            1 - fls (sizeof (chunk_t)) + 1
 *          ... - ...
 *
 * so the 'real idx' to fl_idx have a fixed gap (called 'bias' here):
 *
 *     bias = fls (sizeof (chunk_t)) == log2 (sizeof (chunk_t))
 */

enum
    {
    MIN_CHUNK_SIZE_LOG2 = sizeof (chunk_t) ==  8 ? 3    /* 16-bit target */
                        : sizeof (chunk_t) == 16 ? 4    /* 32-bit target */
                        : sizeof (chunk_t) == 32 ? 5    /* 64-bit target */
                        : 0,
    };

STATIC_ASSERT (sizeof (chunk_t) == (1 << MIN_CHUNK_SIZE_LOG2));
STATIC_ASSERT (SL_BITS <= MIN_CHUNK_SIZE_LOG2);

enum
    {
    FL_IDX_MAX_008K = 13,
    FL_IDX_MAX_016K,
    FL_IDX_MAX_032K,
    FL_IDX_MAX_064K,
    FL_IDX_MAX_128K,
    FL_IDX_MAX_256K,
    FL_IDX_MAX_512K,
    FL_IDX_MAX_001M,
    FL_IDX_MAX_002M,
    FL_IDX_MAX_004M,
    FL_IDX_MAX_008M,
    FL_IDX_MAX_016M,
    FL_IDX_MAX_032M,
    FL_IDX_MAX_064M,
    FL_IDX_MAX_128M,
    FL_IDX_MAX_256M,
    FL_IDX_MAX_512M,
    FL_IDX_MAX_001G,
    FL_IDX_MAX_002G,
    };

#define FL_IDX_MAX              (CONFIG_FL_IDX_MAX)
#define FL_IDX_BIAS             (MIN_CHUNK_SIZE_LOG2)
#define FL_IDXES                (FL_IDX_MAX - FL_IDX_BIAS)
#define SL_IDXES                (1 << SL_BITS)

#define MAX_CHUNK_SIZE          (1u << FL_IDX_MAX)

#define SL_IDX_MAX              (SL_IDXES - 1)

typedef struct chunk_mgr
    {
    uint32_t  fl_bmap;
    uint8_t   sl_bmap [FL_IDXES];
    chunk_t * chunks  [FL_IDXES] [SL_IDXES];
    } chunk_mgr_t;

/**
 * __cm_init - initialize chunk manager
 * @cm: the given chunk manager
 *
 * return: 0 on success, negtive value on error
 */

static inline int __cm_init (chunk_mgr_t * cm)
    {
    return 0;
    }

static inline int __get_fl_idx (size_t size)
    {
    return __fls_u32 ((uint32_t) size);
    }

static inline int __get_sl_idx (size_t size, int fl_idx)
    {
    return (int) ((size >> (fl_idx - SL_BITS)) - (1 << SL_BITS));
    }

static inline void __cm_put_chunk (chunk_mgr_t * cm, chunk_t * chunk)
    {
    int fl_idx = __get_fl_idx (chunk->size);
    int sl_idx = __get_sl_idx (chunk->size, fl_idx);

    cm->fl_bmap |= 1 << fl_idx;
    cm->sl_bmap [fl_idx - FL_IDX_BIAS] |= 1 << sl_idx;

    fl_idx -= FL_IDX_BIAS;

    if (cm->chunks [fl_idx][sl_idx] == NULL)
        {
        dlist_init (&chunk->node);
        cm->chunks [fl_idx][sl_idx] = chunk;
        }
    else
        {
        dlist_add_tail (&cm->chunks [fl_idx][sl_idx]->node, &chunk->node);
        }
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
    int fl_idx = __get_fl_idx (chunk->size);
    int sl_idx = __get_sl_idx (chunk->size, fl_idx);

    fl_idx -= FL_IDX_BIAS;

    if (!dlist_empty (&chunk->node))
        {
        if (cm->chunks [fl_idx][sl_idx] == chunk)
            {
            cm->chunks [fl_idx][sl_idx] = container_of (chunk->node.next,
                                                        chunk_t, node);
            }

        dlist_del (&chunk->node);

        return;
        }

    cm->chunks [fl_idx][sl_idx] = NULL;

    cm->sl_bmap [fl_idx] &= ~(1 << sl_idx);

    if (cm->sl_bmap [fl_idx] == 0)
        {
        cm->fl_bmap &= ~(1 << (fl_idx + FL_IDX_BIAS));
        }
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
    int       fl_idx;
    int       sl_idx;
    uint32_t  fl_bmap;
    uint32_t  sl_bmap;

    fl_idx = __get_fl_idx (bytes);

    bytes += (1 << (fl_idx - SL_BITS)) - 1;

    fl_idx = __get_fl_idx (bytes);
    sl_idx = __get_sl_idx (bytes, fl_idx);

    if (fl_idx > FL_IDX_MAX)
        {
        return NULL;
        }

    sl_bmap = cm->sl_bmap [fl_idx - FL_IDX_BIAS] & (0xffffffffu << sl_idx);

    if (sl_bmap == 0)
        {
        fl_bmap = cm->fl_bmap & (0xffffffffu << (fl_idx + 1));

        if (fl_bmap == 0)
            {
            return NULL;
            }

        fl_idx  = __ctz_u32 (fl_bmap);
        sl_bmap = cm->sl_bmap [fl_idx - FL_IDX_BIAS];
        }

    sl_idx = __ctz_u32 (sl_bmap);

    return cm->chunks [fl_idx - FL_IDX_BIAS] [sl_idx];
    }

#ifdef CONFIG_MEM_STATISTICS
static inline size_t __get_max_free (chunk_mgr_t * cm)
    {
    int       fl_idx;
    int       sl_idx;
    dlist_t * itr;
    size_t    max;

    if (cm->fl_bmap == 0)
        {
        return 0;
        }

    /* find the highest bit in fl_bmap */

    fl_idx = __get_fl_idx (cm->fl_bmap) - FL_IDX_BIAS;
    sl_idx = __fls_u32 (cm->sl_bmap [fl_idx]);

    if (fl_idx >= FL_IDXES)
        {
        return 0;
        }

    if (sl_idx >= SL_IDXES)
        {
        return 0;
        }

    max = (cm->chunks [fl_idx][sl_idx])->size;

    dlist_foreach (itr, &cm->chunks [fl_idx][sl_idx]->node)
        {
        chunk_t * chunk = container_of (itr, chunk_t, node);

        if (chunk->size > max)
            {
            max = chunk->size;
            }
        }

    return max;
    }
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CM_TLSF_H__ */

