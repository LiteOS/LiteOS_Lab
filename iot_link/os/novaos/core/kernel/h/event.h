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

#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

#include <list.h>
#include <module.h>
#include <errno.h>
#include <tick.h>
#include <obj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct event
    {
    obj_t    obj;
    uint32_t event_set;
    dlist_t  pend_q;
    } event_t, * event_id;

/* macros */

#define EVENT_WAIT_ALL                  1
#define EVENT_WAIT_ANY                  2
#define EVENT_WAIT_CLR                  4

#define ERRNO_EVENT_ILLEGAL_ID          ERRNO_JOIN (MID_EVENT, 1)
#define ERRNO_EVENT_TIMEOUT             ERRNO_JOIN (MID_EVENT, 2)
#define ERRNO_EVENT_DELETED             ERRNO_JOIN (MID_EVENT, 3)
#define ERRNO_EVENT_UNAVAILABLE         ERRNO_JOIN (MID_EVENT, 4)
#define ERRNO_EVENT_ILLEGAL_OPERATION   ERRNO_JOIN (MID_EVENT, 5)

/* externs */

extern int      event_init      (event_id);
extern event_id event_create    (void);
extern event_id event_open      (const char *, int, ...);
extern int      event_destroy   (event_id);
extern int      event_timedrecv (event_id, uint32_t, uint32_t, unsigned int, uint32_t *);
extern int      event_recv      (event_id, uint32_t, uint32_t, uint32_t *);
extern int      event_tryrecv   (event_id, uint32_t, uint32_t, uint32_t *);
extern int      event_send      (event_id, uint32_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __EVENT_H__ */

