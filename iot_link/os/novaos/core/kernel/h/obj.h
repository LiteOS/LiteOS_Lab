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

#ifndef __OBJ_H__
#define __OBJ_H__

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include <list.h>
#include <module.h>
#include <errno.h>
#include <atomic.h>

#include <sys/fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* macros */

#define OBJ_FLAGS_STATIC                1   /* object is static, should not be freed */
#define OBJ_FLAGS_EXCL                  2   /* opened as O_EXCL */

#define ERRNO_OBJ_ILLEGAL_ID            ERRNO_JOIN (MID_OBJ, 1)
#define ERRNO_OBJ_ILLEGAL_MAGIC         ERRNO_JOIN (MID_OBJ, 2)
#define ERRNO_OBJ_ILLEGAL_CLASS         ERRNO_JOIN (MID_OBJ, 3)
#define ERRNO_OBJ_ILLEGAL_OPERATION     ERRNO_JOIN (MID_OBJ, 4)
#define ERRNO_OBJ_STILL_USING           ERRNO_JOIN (MID_OBJ, 5)

/* typedefs */

typedef struct class * class_id;

typedef struct obj
    {
    uintptr_t          magic;           /* magic, (uintptr_t) self */
    atomic_uint        safe_cnt;        /* protect for deleting */
    atomic_uint        open_cnt;        /* open (reference) count */
    unsigned int       flags;
    class_id           class;           /* class id this object belongs to */
    dlist_t            node;            /* used to link into class->objs */
    void             * task_del;        /* task wanna delete this object */
    const char       * name;
    } obj_t, * obj_id;

typedef int            (* obj_foreach_pfn) (obj_id, va_list);

/* externs */

extern int    obj_verify         (class_id, obj_id);
extern int    obj_verify_protect (class_id, obj_id);
extern void   obj_unprotect      (obj_id);
extern int    obj_init           (class_id, obj_id, ...);
extern obj_id obj_create         (class_id, ...);
extern obj_id obj_open           (class_id, const char *, int, va_list);
extern int    obj_destroy        (class_id, obj_id);
extern obj_id obj_foreach        (class_id, obj_foreach_pfn, ...);
extern obj_id obj_find           (class_id, const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OBJ_H__ */

