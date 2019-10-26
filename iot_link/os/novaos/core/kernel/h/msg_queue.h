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

#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <stddef.h>

#include <list.h>
#include <sem.h>
#include <mutex.h>
#include <errno.h>
#include <obj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MQ_OP_RD                0
#define MQ_OP_WT                1

#define ERRNO_MQ_ILLEGAL_ID         ERRNO_JOIN (MID_MQ, 1)
#define ERRNO_MQ_ILLEGAL_SIZE       ERRNO_JOIN (MID_MQ, 2)
#define ERRNO_MQ_ILLEGAL_MSGS       ERRNO_JOIN (MID_MQ, 3)
#define ERRNO_MQ_ILLEGAL_BUFF       ERRNO_JOIN (MID_MQ, 4)
#define ERRNO_MQ_ILLEGAL_OPERATION  ERRNO_JOIN (MID_MQ, 5)
#define ERRNO_MQ_UNAVAILABLE        ERRNO_JOIN (MID_MQ, 6)
#define ERRNO_MQ_TIMEOUT            ERRNO_JOIN (MID_MQ, 7)

typedef struct mq
    {
    obj_t       obj;
    sem_t       sem [2];           /* 0: rdsem, 1: wtsem */
    size_t      msg_size;
    size_t      max_msgs;
    atomic_uint idx [2];           /* 0: head,  1: tail  */
    char      * msgs;
    } mq_t, * mq_id;

/* externs */

extern mq_id    mq_create    (size_t, size_t, unsigned int);
extern int      mq_delete    (mq_id);
extern mq_id    mq_open      (const char *, int, ...);
extern int      mq_timedsend (mq_id, void *, size_t, unsigned int);
extern int      mq_send      (mq_id, void *, size_t);
extern int      mq_trysend   (mq_id, void *, size_t);
extern int      mq_timedrecv (mq_id, void *, size_t, unsigned int);
extern int      mq_recv      (mq_id, void *, size_t);
extern int      mq_tryrecv   (mq_id, void *, size_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __MSG_QUEUE_H__ */

