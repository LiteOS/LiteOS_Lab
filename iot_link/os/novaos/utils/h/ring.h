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

#ifndef __RING_H__
#define __RING_H__

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ring
    {
    size_t          size;
    size_t          head;
    size_t          tail;
    unsigned char * buff;
    } ring_t;

/* inlines */

/**
 * ring_reset - empty the ring buffer
 * @ring: the ring buff.
 */

static inline void ring_reset (ring_t * ring)
    {
    ring->tail = 0;
    ring->head = 0;
    }

/**
 * ring_len - get the number of bytes in the ring buffer
 * @ring: the ring buff.
 */

static inline size_t ring_len (ring_t * ring)
    {
    return ring->tail - ring->head;
    }

/**
 * ring_empty - check if the ring is empty
 * @ring: the ring buff.
 */

static inline bool ring_empty (ring_t * ring)
    {
    return ring->tail == ring->head;
    }

/**
 * ring_full - check if the ring is full
 * @ring: the ring buff.
 */

static inline bool ring_full (ring_t * ring)
    {
    return ring_len (ring) == ring->size;
    }

/**
 * ring_peek - just return value from buff [head ~ tail] by index, data will not
 *             out of ring
 * @ring: the ring buff.
 * @idx:  the index from head to read.
 */

static inline unsigned char ring_peek (ring_t * ring, size_t idx)
    {
    return ring->buff [(ring->head + idx) & (ring->size - 1)];
    }

/* externs */

extern int      ring_init       (ring_t * ring, unsigned char * buff, size_t size);
extern ring_t * ring_create     (size_t size);
extern void     ring_destroy    (ring_t * ring);
extern size_t   ring_put        (ring_t * ring, unsigned char * buff, size_t len);
extern size_t   ring_put_force  (ring_t * ring, unsigned char * buff, size_t len);
extern size_t   ring_putc       (ring_t * ring, unsigned char byte);
extern size_t   ring_putc_force (ring_t * ring, unsigned char byte);
extern size_t   ring_get        (ring_t * ring, unsigned char * buff, size_t len);
extern size_t   ring_getc       (ring_t * ring, unsigned char * byte);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RING_H__ */

