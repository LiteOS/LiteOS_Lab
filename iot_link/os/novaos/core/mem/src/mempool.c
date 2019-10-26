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

#include <mempool.h>
#include <class.h>
#include <init.h>
#include <warn.h>

#ifdef CONFIG_SYSCALL
#include <syscall.h>
#endif

/* globals */

/*
 * classes are defined as an array with only 1 element, so we can use it as an
 * pointer
 */

class_t mempool_class [1];

/**
 * mempool_create - create a memory pool
 * @size:  the item size
 * @items: the number of items in the pool
 * @buff:  the memory used for the pool
 *
 * return: the created pool, NULL on error
 */

mempool_id mempool_create (size_t size, size_t items, char * buff)
    {
    WARN_ON (size < sizeof (dlist_t) || (size & (sizeof (uintptr_t) - 1)) != 0,
             errno = ERRNO_MEMPOOL_ILLEGAL_PARA; return NULL,
             "Invalid item size!");

    WARN_ON (items == 0 || items >= UINT_MAX,
             errno = ERRNO_MEMPOOL_ILLEGAL_PARA; return NULL,
             "Invalid number of items!");

    WARN_ON (buff == NULL,
             errno = ERRNO_MEMPOOL_ILLEGAL_PARA; return NULL,
             "Invalid buffer!");

    /*
     * obj is just the first member of mempool, so we can return this even when
     * obj_open return NULL
     */

    return container_of (obj_create (mempool_class, size, items, buff),
                         mempool_t, obj);
    }

/**
 * mempool_destroy - destroy a memory pool
 * @pool: the pool will be destroyed
 *
 * return: 0 on success, negtive value on error
 */

int mempool_destroy (mempool_id pool)
    {
    WARN_ON (pool == NULL,
             errno = ERRNO_MEMPOOL_ILLEGAL_ID; return -1,
             "Invalid mempool id!");

    return obj_destroy (mempool_class, &pool->obj);
    }

/**
 * mempool_alloc - allocate an item from a memory pool
 * @pool: the pool id
 *
 * return: a item on success, NULL on error
 */

char * mempool_alloc (mempool_id pool)
    {
    dlist_t * first;

    WARN_ON (pool == NULL,
             errno = ERRNO_MEMPOOL_ILLEGAL_ID; return NULL,
             "Invalid mempool id!");

    if (unlikely (obj_verify_protect (mempool_class, &pool->obj) != 0))
        {
        return NULL;
        }

    if (unlikely (sem_wait (&pool->sem) != 0))
        {
        obj_unprotect (&pool->obj);
        return NULL;
        }

    if ((unlikely (mutex_lock (&pool->lock) != 0)))
        {
        (void) sem_post (&pool->sem);
        obj_unprotect (&pool->obj);
        return NULL;
        }

    first = pool->items.next;

    dlist_del (first);

    mutex_unlock (&pool->lock);

    obj_unprotect (&pool->obj);

    return (char *) first;
    }

/**
 * mempool_free - free an item from a memory pool
 * @pool: the pool id
 * @item: the item to be freed
 *
 * return: 0 on success, negtive value on error
 */

int mempool_free (mempool_id pool, char * item)
    {
    WARN_ON (pool == NULL,
             errno = ERRNO_MEMPOOL_ILLEGAL_ID; return -1,
             "Invalid mempool id!");

    if (unlikely (obj_verify_protect (mempool_class, &pool->obj) != 0))
        {
        return -1;
        }

    if (unlikely (mutex_lock (&pool->lock) != 0))
        {
        obj_unprotect (&pool->obj);
        return -1;
        }

    if (unlikely (((item < pool->buff)) ||
                  ((size_t) (item - pool->buff) / pool->item_size >= pool->nr_items) ||
                  ((size_t) (item - pool->buff) % pool->item_size != 0)))
        {
        mutex_unlock (&pool->lock);
        errno = ERRNO_MEMPOOL_ILLEGAL_OPERATION;
        obj_unprotect (&pool->obj);

        WARN ("Trying to free invalid item!");

        return -1;
        }
    else
        {
        dlist_add (&pool->items, (dlist_t *) item);
        mutex_unlock (&pool->lock);
        (void) sem_post (&pool->sem);
        obj_unprotect (&pool->obj);

        return 0;
        }
    }

static int __mempool_init (obj_id obj, va_list valist)
    {
    mempool_id pool = container_of (obj, mempool_t, obj);
    size_t     size  = va_arg (valist, size_t);
    size_t     items = va_arg (valist, size_t);
    char     * buff  = va_arg (valist, char *);
    size_t     i;

    pool->buff      = buff;
    pool->item_size = size;
    pool->nr_items  = items;

    /* never fail here */

    (void) sem_init   (&pool->sem, (unsigned int) items);
    (void) mutex_init (&pool->lock);

    dlist_init (&pool->items);

    for (i = 0; i < items; i++)
        {
        dlist_add (&pool->items, (dlist_t *) buff);
        buff += size;
        }

    return 0;
    }

static int __mempool_destroy (obj_id obj)
    {
    mempool_id pool = container_of (obj, mempool_t, obj);

    WARN_ON (sem_getvalue (&pool->sem) != pool->nr_items,
             errno = ERRNO_MEMPOOL_BUSY; return -1,
             "Trying to destroy a busy mempool!");

    if (unlikely (mutex_destroy (&pool->lock) != 0))
        {
        return -1;
        }

    (void) sem_destroy (&pool->sem);

    return 0;
    }

/**
 * mempool_lib_init - mempool library initialization routine
 *
 * return: 0 on success, negtive value on error
 */

static int mempool_lib_init (void)
    {
    if (class_init (mempool_class, MID_MEMPOOL, sizeof (mempool_t),
                    __mempool_init, __mempool_destroy, NULL, NULL) != 0)
        {
        WARN ("Fail to initialize mempool_class!");
        return -1;
        }

    return 0;
    }

MODULE_INIT (kernel, mempool_lib_init);

#ifdef CONFIG_SYSCALL
const uintptr_t syscall_entries_mempool [] =
    {
    (uintptr_t) mempool_create,
    (uintptr_t) mempool_destroy,
    (uintptr_t) mempool_alloc,
    (uintptr_t) mempool_free,
    };

const struct syscall_table syscall_table_mempool =
    {
    ARRAY_SIZE (syscall_entries_mempool),
    syscall_entries_mempool
    };
#endif

