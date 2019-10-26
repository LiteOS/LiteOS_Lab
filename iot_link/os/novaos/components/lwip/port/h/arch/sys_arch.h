/*
 * Copyright (c) 2017 Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmdit <goldsimon@gmx.de>
 *
 */
#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "lwip/opt.h"
#include "lwip/arch.h"

#include <config.h>

/* typedefs */

#if SYS_LIGHTWEIGHT_PROT
typedef u32_t sys_prot_t;
#endif /* SYS_LIGHTWEIGHT_PROT */

typedef struct 
    {
    void * mut;
    } sys_mutex_t;

typedef struct
    {
    void * sem;
    } sys_sem_t;

typedef struct 
    {
    void * mbox;
    } sys_mbox_t;

typedef struct 
    {
    void * thread;
    } sys_thread_t;

/* macros */

#define sys_msleep  sys_arch_msleep

#define sys_mutex_valid         sys_arch_mutex_valid
#define sys_mutex_set_invalid   sys_arch_mutex_set_invalid

#define sys_sem_valid           sys_arch_sem_valid
#define sys_sem_set_invalid     sys_arch_sem_set_invalid

#define sys_mbox_valid          sys_arch_mbox_valid
#define sys_mbox_set_invalid    sys_arch_mbox_set_invalid

/* inlines */

static __always_inline bool sys_arch_mutex_valid (sys_mutex_t * mutex)
    {
    return ((mutex != NULL) && (mutex->mut != NULL));
    }

static __always_inline void sys_arch_mutex_set_invalid (sys_mutex_t * mutex)
    {
    mutex->mut = NULL;
    }

static __always_inline bool sys_arch_sem_valid (sys_sem_t * sem)
    {
    return ((sem != NULL) && (sem->sem != NULL));
    }

static __always_inline void sys_arch_sem_set_invalid (sys_sem_t * sem)
    {
    sem->sem = NULL;
    }

static __always_inline bool sys_arch_mbox_valid (sys_mbox_t * mbox)
    {
    return ((mbox != NULL) && (mbox->mbox != NULL));
    }

static __always_inline void sys_arch_mbox_set_invalid (sys_mbox_t * mbox)
    {
    mbox->mbox = NULL;
    }

/* externs */

void sys_arch_msleep (u32_t delay_ms);

#endif /* __SYS_ARCH_H__ */
