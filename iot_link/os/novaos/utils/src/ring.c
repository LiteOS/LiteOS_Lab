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

#include <stdlib.h>
#include <string.h>

#include <common.h>
#include <ring.h>

/**
 * ring_init - initialize a predefined ring using preallocated buffer
 * @ring: the predefined ring to be used.
 * @buff: the preallocated buffer to be used.
 * @size: the size of the preallocated buffer, must be power of 2.
 *
 * return: 0 on success, negtive value on error
 */

int ring_init (ring_t * ring, unsigned char * buff, size_t size)
    {
    if ((ring == NULL) || (buff == NULL))
        {
        return -1;
        }

    if (size & (size - 1))
        {
        return -1;
        }

    ring->buff = buff;
    ring->size = size;
    ring->head = 0;
    ring->tail = 0;

    return 0;
    }

/**
 * ring_create - create ring with a specificed size
 * @size: the size of the buffer to be allocated, must be power of 2.
 *
 * return: a pointer to the ring allocated, NULL on error
 */

ring_t * ring_create (size_t size)
    {
    ring_t * ring;

    if (size & (size - 1))
        {
        return NULL;
        }

    ring = (ring_t *) malloc (sizeof (ring_t) + size);

    if (ring == NULL)
        {
        return NULL;
        }

    ring->buff = (unsigned char *) (&ring [1]);
    ring->size = size;
    ring->head = 0;
    ring->tail = 0;

    return ring;
    }

/**
 * ring_destroy - destroy a ring
 * @size: the ring to be destroied, must be created by ring_create
 *
 * return: NA
 */

void ring_destroy (ring_t * ring)
    {
    if (ring == NULL)
        {
        return;
        }

    free (ring->buff);      /* it is ok to free (NULL) */
    free (ring);
    }

/**
 * __ring_put - put data to a ring, and the free space is enough
 */

static inline size_t __ring_put (ring_t * ring, unsigned char * buff, size_t len)
    {
    size_t tail_space;
    size_t mask = ring->size - 1;

    /*
     * tail_space => the space size from tail to the end of the ring
     *               (size - tail & mask)
     */

    tail_space = ring->size - (ring->tail & mask);

    if (len > tail_space)
        {

        /*
         * must be this condition:
         *
         *        head     tail
         * +------+--------+---------+
         * |      |////////|         |
         * +------+--------+---------+
         *
         * len <= ring_free (already guaranteed in parent function)
         * len >  tail_space
         *
         * so there must be two free partitions, one in the front, one in the end
         */

        memcpy (ring->buff + (ring->tail & mask), buff, tail_space);
        memcpy (ring->buff, buff + tail_space, len - tail_space);
        }
    else
        {

        /*
         *
         * may be the following two conditions:
         *
         * 1)
         *        head     tail
         * +------+--------+---------+
         * |      |////////|         |
         * +------+--------+---------+
         *
         * no problem, it's fine:
         *
         *       ring_free >= tail_space >= len
         *
         * 2)
         *        tail     head
         * +------+--------+---------+
         * |//////|        |/////////|
         * +------+--------+---------+
         *
         * there is only one free region, and:
         *
         *       a) len <= ring_free
         *       b) tail_space > ring_free
         *
         * so it is still safe to copy <len> bytes
         */

        memcpy (ring->buff + (ring->tail & mask), buff, len);
        }

    ring->tail += len;

    return len;
    }

/**
 * ring_put - put data to a ring
 * @ring: the ring which the data will be put in.
 * @buff: the data buffer.
 * @size: the length of the data.
 *
 * return: the size of data actually put
 */

size_t ring_put (ring_t * ring, unsigned char * buff, size_t len)
    {
    return __ring_put (ring, buff, min (len, ring->size - ring_len (ring)));
    }

/**
 * ring_put_force - put data to a ring forcely
 * @ring: the ring which the data will be put in.
 * @buff: the data buffer.
 * @size: the length of the data.
 *
 * return: the size of data actually put
 */

size_t ring_put_force (ring_t * ring, unsigned char * buff, size_t len)
    {
    size_t ring_free;

    len       = min (len, ring->size);
    ring_free = ring->size - ring_len (ring);

    if (len > ring_free)
        {
        ring->head += len - ring_free;
        }

    return __ring_put (ring, buff, len);
    }

/**
 * ring_putc - optimized version for put one byte to a ring
 * @ring: the ring which the data will be put in.
 * @byte: the byte to be put in.
 *
 * return: the size of data actually put
 */

size_t ring_putc (ring_t * ring, unsigned char byte)
    {
    if (ring_full (ring))
        {
        return 0;
        }

    ring->buff [ring->tail & (ring->size - 1)] = byte;

    ring->tail++;

    return 1;
    }

/**
 * ring_putc_force - optimized version for put data to a ring forcely
 * @ring: the ring which the data will be put in.
 * @byte: the byte to be put in.
 *
 * return: 1
 */

size_t ring_putc_force (ring_t * ring, unsigned char byte)
    {
    if (ring_full (ring))
        {
        ring->head++;
        }

    ring->buff [ring->tail & (ring->size - 1)] = byte;

    ring->tail++;

    return 1;
    }

/**
 * ring_get - get data from a ring
 * @ring: the ring which the data will be get from.
 * @buff: the data buffer.
 * @size: the max length of the data to get.
 *
 * return: the size of data actually got
 */

size_t ring_get (ring_t * ring, unsigned char * buff, size_t len)
    {
    size_t ring_data;
    size_t head_space;
    size_t mask = ring->size - 1;

    /*
     * ring_data  => the data size in the ring
     * head_space => the space size from head to the end of the ring
     *               (size - head & mask)
     */

    ring_data  = ring_len (ring);
    head_space = ring->size - (ring->head & mask);

    len = min (len, ring_data);

    if (len > head_space)
        {

        /*
         * must be this condition:
         *
         *        tail     head
         * +------+--------+---------+
         * |//////|        |/////////|
         * +------+--------+---------+
         *
         * len <= ring_data
         * len >  head_space
         *
         * so there must be two data partitions, one in the front, one in the end
         */

        memcpy (buff, ring->buff + (ring->head & mask), head_space);
        memcpy (buff + head_space, ring->buff, len - head_space);
        }
    else
        {

        /*
         * may be the following two conditions:
         *
         * 1)
         *        tail     head
         * +------+--------+---------+
         * |//////|        |/////////|
         * +------+--------+---------+
         *
         * no problem, it's fine:
         *
         *       ring_data >= head_space >= len
         *
         * 2)
         *        head     tail
         * +------+--------+---------+
         * |      |////////|         |
         * +------+--------+---------+
         *
         * there is only one data region, and:
         *
         *       a) len <= ring_data
         *       b) head_space > ring_data
         *
         * so it is still safe to copy <len> bytes
         */

        memcpy (buff, ring->buff + (ring->head & mask), len);
        }

    ring->head += len;

    return len;
    }

/**
 * ring_getc - optimized version for get one byte from a ring
 * @ring: the ring which the data will be put in.
 * @byte: the address where store the byte.
 *
 * return: the size of data actually got
 */

size_t ring_getc (ring_t * ring, unsigned char * byte)
    {
    if (ring_empty (ring))
        {
        return 0;
        }

    *byte = ring->buff [ring->head & (ring->size - 1)];

    ring->head++;

    return 1;
    }

