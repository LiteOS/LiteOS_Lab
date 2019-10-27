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

#ifndef __SEM_H__
#define __SEM_H__

#include <stdint.h>

#include <list.h>
#include <module.h>
#include <errno.h>
#include <tick.h>
#include <atomic.h>
#include <obj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sem
    {
    obj_t        obj;
    atomic_uint  count;
    dlist_t      pend_q;
    } sem_t, * sem_id;

/* defines */

#define ERRNO_SEM_ILLEGAL_ID        ERRNO_JOIN (MID_SEM, 1)
#define ERRNO_SEM_TIMEOUT           ERRNO_JOIN (MID_SEM, 2)
#define ERRNO_SEM_DELETED           ERRNO_JOIN (MID_SEM, 3)
#define ERRNO_SEM_UNAVAILABLE       ERRNO_JOIN (MID_SEM, 4)
#define ERRNO_SEM_ILLEGAL_OPERATION ERRNO_JOIN (MID_SEM, 5)
#define ERRNO_SEM_OVERFLOW          ERRNO_JOIN (MID_SEM, 6)

/* externs */

extern int          sem_init      (sem_t *, unsigned int);
extern sem_id       sem_create    (unsigned int);
extern sem_id       sem_open      (const char *, int, ...);
extern int          sem_destroy   (sem_t *);
extern int          sem_timedwait (sem_t *, unsigned int);
extern int          sem_wait      (sem_t *);
extern int          sem_trywait   (sem_t *);
extern int          sem_post      (sem_t *);
extern unsigned int sem_getvalue  (sem_t *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SEM_H__ */

