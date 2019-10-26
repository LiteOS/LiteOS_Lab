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

#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include <stdint.h>

#include <sem.h>
#include <mutex.h>
#include <list.h>
#include <module.h>
#include <errno.h>
#include <obj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* typedefs */

typedef struct mempool
    {
    obj_t    obj;
    size_t   item_size;
    size_t   nr_items;
    sem_t    sem;
    mutex_t  lock;
    dlist_t  items;
    char   * buff;
    } mempool_t, * mempool_id;

/* macros */

#define ERRNO_MEMPOOL_ILLEGAL_ID        ERRNO_JOIN (MID_MEMPOOL, 1)
#define ERRNO_MEMPOOL_ILLEGAL_PARA      ERRNO_JOIN (MID_MEMPOOL, 2)
#define ERRNO_MEMPOOL_ILLEGAL_OPERATION ERRNO_JOIN (MID_MEMPOOL, 3)
#define ERRNO_MEMPOOL_BUSY              ERRNO_JOIN (MID_MEMPOOL, 4)

/* externs */

extern mempool_id mempool_create   (size_t, size_t, char *);
extern int        mempool_destroy  (mempool_id);
extern char     * mempool_alloc    (mempool_id);
extern int        mempool_free     (mempool_id, char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEMPOOL_H__ */
