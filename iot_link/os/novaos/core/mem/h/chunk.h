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

#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <stddef.h>
#include <stdint.h>

#include <list.h>
#include <compiler.h>

#include <arch/config.h>        /* for ARCH specified ALLOC_ALIGN */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef ALLOC_ALIGN
#define ALLOC_ALIGN             (8)
#else
STATIC_ASSERT ((ALLOC_ALIGN & (ALLOC_ALIGN - 1)) == 0);
#endif

#define ALLOC_ALIGN_MASK        (ALLOC_ALIGN - 1)

/*
 * ALLOC_MIN_SIZE is defined as sizeof (dlist_t) ensure the memory block can be
 * inserted to the free list when it is free
 */

#define ALLOC_MIN_SIZE          (sizeof (dlist_t))

/*
 * it is just fine that use uintptr_t as size, this feild just describes the size
 * of a chunk, and a chunk is just a block of memory that CPU can access directlly
 * which is virtual memory or physical memory without mmu, its address and range
 * is just in [0, UINTPTR_MAX], so use a uintptr_t as the size is perfect, this
 * make sure 'prev' and 'size' have the same size and alignment
 */

typedef struct chunk
    {
    struct chunk * prev;
    uintptr_t      size;        /* bit[0] means is_allocated */
    union
        {
        dlist_t    node;        /* used for free chunks only */
                                /* none for allocated chunks */
        };
    } chunk_t;

typedef struct fch
    {
    struct chunk * prev;
    uintptr_t      size;        /* bit[0] means is_allocated */
    dlist_t        node;        /* used for free chunks only */
    } fch_t;

typedef struct ach
    {
    struct chunk * prev;
    uintptr_t      size;        /* bit[0] means is_allocated */
    } ach_t;

/* inlines */

/**
 * __is_free - check if a chunk is free
 * @chunk: the given chunk
 *
 * return: true if the chunk is free, false if not
 */

static __always_inline int __is_free (chunk_t * chunk)
    {
    return (chunk->size & 1) == 0;
    }

/**
 * __set_chunk_allocated - set a chunk as allocated
 * @chunk: the given chunk
 */

static __always_inline void __set_chunk_allocated (chunk_t * chunk)
    {
    chunk->size |= 1;
    }

/**
 * __set_chunk_free - set a chunk as free
 * @chunk: the given chunk
 */

static __always_inline void __set_chunk_free (chunk_t * chunk)
    {
    chunk->size &= ~1u;
    }

/**
 * __get_next_chunk - get the next chunk
 * @chunk: the current chunk
 *
 * return: next chunk
 */

static __always_inline chunk_t * __get_next_chunk (chunk_t * chunk)
    {
    return (chunk_t *) (((char *) chunk) + (chunk->size & ~1u));
    }

/**
 * __get_prev_chunk - get the previous chunk
 * @chunk: the current chunk
 *
 * return: previous chunk
 */

static __always_inline chunk_t * __get_prev_chunk (chunk_t * chunk)
    {
    return chunk->prev;
    }

/**
 * __get_mem_block - get the memory block in a chunk
 * @chunk: the chunk
 *
 * return: the memory block in the chunk
 */

static __always_inline char * __get_mem_block (chunk_t * chunk)
    {
    return (char *) chunk + sizeof (ach_t);
    }

/**
 * __get_fch_from_mem - create a new chunk for a memory block
 * @mem: the memory block
 *
 * return: the new chunk containing the memory block
 */

static __always_inline chunk_t * __get_fch_from_mem (char * mem)
    {
    return &((chunk_t *) mem) [-1];
    }

/**
 * __get_ach_from_mem - get the chunk of a allocated memory block
 * @mem: the memory block
 *
 * return: the chunk containing the memory block
 */

static __always_inline ach_t * __get_ach_from_mem (char * mem)
    {
    uintptr_t szorptr;

    /*
     * there are two possible cases here:
     *
     * 1) chunk->head is just behind the 'mem':
     *
     *    +-------------+--------------+
     *    | prev | size | memory block |
     *    +-------------+--------------+
     *
     * 2) after an aligned alloc, there may extra memory in the front but the
     *    size is less than sizeof (chunk_t), there will be a pointer in the
     *    'extra mem':
     *
     *    +-------------+-----------+----------------------+
     *    | prev | size | extra mem | aligned memory block |
     *    +-------------+-----------+----------------------+
     *
     *    please refer to the routine of <__carve_head> for this case
     *
     * for both cases, this routine just works fine
     */

    szorptr = ((uintptr_t *) mem) [-1];

    if (szorptr & 1)
        {

        /* case 1, chunk->size */

        return &((ach_t *) mem) [-1];
        }

    /* case 2, a pointer to the chunk */

    return (ach_t *) szorptr;
    }

/**
 * __set_ach_to_mem - set chunk in the front of memory block
 * @mem: the memory block
 *
 * return: NA
 */

static __always_inline void __set_ach_to_mem (char * mem, chunk_t * chunk)
    {
    ((chunk_t **) mem) [-1] = chunk;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CHUNK_H__ */

