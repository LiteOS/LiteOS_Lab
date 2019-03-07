/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2019>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#ifndef __PTHREAD_H
#define __PTHREAD_H

#include "los_def.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u32_t once_t;

int once(once_t *once_control, void (* init_func)(void));

/* Mutex types. */
enum
{
    MUTEX_NORMAL = 0,
    PTHREAD_MUTEX_RECURSIVE,
    MUTEX_ERRORCHECK,
    MUTEX_DEFAULT = MUTEX_NORMAL
};

struct mutex_attr
{
    u32_t type;
};

typedef struct mutex_attr mutex_attr_t;

typedef struct mutex_1
{
    mutex_attr_t attr;
    u32_t lock;
} mutex_t;

typedef struct mutex
{
    mutex_attr_t attr;
    u32_t lock;
} pthread_mutex_t;


/* POSIX pthread mutex interface */
int pthread_mutex_init(pthread_mutex_t *mutex, const mutex_attr_t *attr);
int mutex_destroy(mutex_t *mutex);
int mutex_trylock(mutex_t *mutex);
int mutex_lock(mutex_t *mutex);
int mutex_timedlock(mutex_t *mutex, const struct time_spec *ts);
int mutex_unlock(mutex_t *mutex);

int mutex_attr_init(mutex_attr_t *attr);
int mutex_attr_destroy(mutex_attr_t *attr);
int mutex_attr_gettype(const mutex_attr_t *attr, int *type);
int mutex_attr_settype(mutex_attr_t *attr, int type);
int mutex_attr_setpshared(mutex_attr_t *attr, int  pshared);
int mutex_attr_getpshared(mutex_attr_t *attr, int *pshared);
 
typedef u32_t rwlock_attr_t;

typedef struct rwlock
{
    u32_t lock;
} rwlock_t;

/* POSIX pthread rwlock interface */
int rwlock_init(rwlock_t *rwlock, const rwlock_attr_t *attr);
int rwlock_destroy(rwlock_t *rwlock);
int rwlock_rdlock(rwlock_t *rwlock);
int rwlock_wrlock(rwlock_t *rwlock);
int rwlock_timedrdlock(rwlock_t *rwlock, const struct time_spec *abstime);
int rwlock_timedwrlock(rwlock_t *rwlock, const struct time_spec *abstime);
int rwlock_tryrdlock(rwlock_t *rwlock);
int rwlock_trywrlock(rwlock_t *rwlock);
int rwlock_unlock(rwlock_t *rwlock);

/* POSIX pthread thread interface */
int pthread_create(pthread_t *tid, const pthread_attr_t *attr,
                   void *(*entry) (void *), void *arg);

typedef struct cond
{
    u32_t lock;
} cond_t;

typedef struct cond_attr
{
    int dummy;
} cond_attr_t;

/* POSIX pthread condition interface */
int cond_attr_init(cond_attr_t *attr);
int cond_attr_destroy(cond_attr_t *attr);
int cond_init(cond_t *cond, const cond_attr_t *attr);
int cond_destroy(cond_t *cond);
int cond_broadcast(cond_t *cond);
int cond_signal(cond_t *cond);
int cond_wait(cond_t *cond, mutex_t *mutex);
int cond_timedwait(cond_t *cond, mutex_t *mutex, const struct time_spec *abstime);

#ifdef __cplusplus
}
#endif

#endif
