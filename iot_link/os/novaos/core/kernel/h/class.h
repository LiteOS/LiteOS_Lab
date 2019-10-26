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

/* ================= THIS MODULE IS FOR INTERRNAL USE ONLY! ================= */

#ifndef __CLASS_H__
#define __CLASS_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <list.h>
#include <mutex.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

/* typedefs */

typedef struct obj   * obj_id;

typedef int         (* obj_init_pfn)    (obj_id, va_list);
typedef obj_id      (* obj_alloc_pfn)   (void);
typedef obj_id      (* obj_free_pfn)    (obj_id);
typedef int         (* obj_destroy_pfn) (obj_id);

typedef struct class
    {
    uint8_t            mid;             /* module id */
    size_t             obj_size;        /* object size */
    mutex_t            lock;            /* lock used to protect objs list, mutex
                                           lock is used here instead of task_lock
                                           because in obj_foreach, the callback
                                           may invoke mutex_lock */
    dlist_t            objs;            /* all objects belongs to this */
    obj_init_pfn       init_rtn;
    obj_destroy_pfn    destroy_rtn;
    obj_alloc_pfn      alloc_rtn;
    obj_free_pfn       free_rtn;
    bool               inited;
    } class_t, * class_id;

/* externs */

extern int class_init (class_id, uint8_t, size_t, obj_init_pfn, obj_destroy_pfn,
                       obj_alloc_pfn, obj_free_pfn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CLASS_H__ */

