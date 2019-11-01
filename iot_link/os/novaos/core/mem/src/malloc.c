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

#include <heap.h>

#include <stdlib.h>

void * malloc (size_t size)
    {
    return heap_alloc (kernel_heap, size);
    }

void * calloc (size_t size, size_t len)
    {
    size_t s = size * len;
    void * ptr;

    if (s < size || s < len)
        {
        return NULL;
        }

    ptr = malloc (s);

    if (ptr != NULL)
        {
        memset (ptr, 0, s);
        }

    return ptr;
    }

void * realloc (void * ptr, size_t size)
    {
    return (void *) heap_realloc (kernel_heap, (char *) ptr, size);
    }

void free (void * ptr)
    {
    heap_free (kernel_heap, ptr);
    }

void * memalign (size_t alignment,  size_t size)
    {
    return heap_alloc_align (kernel_heap, alignment, size);
    }

#ifdef __NEWLIB__

void * _malloc_r (struct _reent * ptr, size_t size)
    {
    return malloc (size);
    }

void * _realloc_r (struct _reent * ptr, void * old, size_t newlen)
    {
    return realloc (old, newlen);
    }

void * _calloc_r (struct _reent * ptr, size_t size, size_t len)
    {
    return calloc (size, len);
    }

void _free_r (struct _reent * ptr, void * addr)
    {
    free(addr);
    }

#endif

