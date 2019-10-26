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

#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>

#include <mutex.h>
#include <config.h>
#include <list.h>
#include <module.h>
#include <errno.h>
#include <obj.h>

#include <arch/config.h>        /* for arch specified ALLOC_ALIGN */

#include "chunk.h"

#if   defined (CONFIG_TLSF)
#include "private/tlsf.h"
#elif defined (CONFIG_BESTFIT)
#include "private/bestfit.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define MIN_BLOCK_SIZE                  (sizeof (block_t)   +   \
                                         sizeof (ach_t) * 2 +   \
                                         sizeof (fch_t))

#define ERRNO_HEAP_ILLEGAL_ID           ERRNO_JOIN (MID_HEAP, 1)
#define ERRNO_HEAP_ILLEGAL_BLOCK        ERRNO_JOIN (MID_HEAP, 2)
#define ERRNO_HEAP_ILLEGAL_ALIGN        ERRNO_JOIN (MID_HEAP, 3)
#define ERRNO_HEAP_ILLEGAL_STAT         ERRNO_JOIN (MID_HEAP, 4)
#define ERRNO_HEAP_NOT_ENOUGH_MEMORY    ERRNO_JOIN (MID_HEAP, 5)

/* typedefs */

typedef struct block
    {
    struct block     * next;
    size_t             size;
    } block_t;

#ifdef CONFIG_MEM_STATISTICS
typedef struct mem_stat
    {
    unsigned int       free_chunks;
    size_t             free_size;
    unsigned int       busy_chunks;
    size_t             busy_size;
    size_t             max_busy_size;
    size_t             max_free_size;
    unsigned long long cum_allocated;
    unsigned long long cum_size_allocated;
    unsigned long long cum_freed;
    unsigned long long cum_size_freed;
    } mem_stat_t;
#endif

typedef struct heap
    {
    obj_t              obj;
    chunk_mgr_t        cm;
    block_t          * blocks;
    mutex_t            mux;
#ifdef CONFIG_MEM_STATISTICS
    struct mem_stat    stat;
#endif
    } heap_t, * heap_id;

/* inlines */

#ifdef CONFIG_MEM_STATISTICS
static inline void __stat_chunk_add (mem_stat_t * stat, chunk_t * chunk)
    {
    stat->free_chunks++;
    stat->free_size += (chunk->size - sizeof (ach_t));
    }

static inline void __stat_chunk_del (mem_stat_t * stat, chunk_t * chunk)
    {
    stat->free_chunks--;
    stat->free_size -= (chunk->size - sizeof (ach_t));
    }

static inline void __stat_chunk_alloc (mem_stat_t * stat, chunk_t * chunk)
    {
    stat->busy_chunks++;
    stat->busy_size += chunk->size;
    stat->cum_allocated++;
    stat->cum_size_allocated += chunk->size;

    if (stat->busy_size > stat->max_busy_size)
        {
        stat->max_busy_size = stat->busy_size;
        }
    }

static inline void __stat_chunk_free (mem_stat_t * stat, chunk_t * chunk)
    {
    stat->busy_chunks--;
    stat->busy_size -= chunk->size;
    stat->cum_freed++;
    stat->cum_size_freed += chunk->size;
    }
#endif

/* externs */

extern heap_t                   kernel_heap [1];

extern char  * heap_alloc_align (heap_t *, size_t, size_t);
extern char  * heap_alloc       (heap_t *, size_t);
extern void    heap_free        (heap_t *, char *);
extern char  * heap_realloc     (heap_t *, char *, size_t);
extern int     heap_init        (heap_t *);
extern heap_id heap_create      (void);
extern int     heap_add         (heap_t *, char *, size_t);
#ifdef CONFIG_MEM_STATISTICS
extern int     heap_stat_get    (heap_t *, mem_stat_t *);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HEAP_H__ */

