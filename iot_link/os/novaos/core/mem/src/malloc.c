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

void * malloc (size_t size)
    {
    return heap_alloc (kernel_heap, size);
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

