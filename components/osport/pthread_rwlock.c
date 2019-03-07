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

#include "los_def.h"
#include "pthread.h"
#include "errno.h"

int rwlock_init(rwlock_t *rwlock, const rwlock_attr_t *attr)
{
    u32_t ret;

    ret = LOS_RWLockCreate(&rwlock->lock);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_destroy(rwlock_t *rwlock)
{
    u32_t ret;

    ret = LOS_RWLockDelete(rwlock->lock);
    if(ret == LOS_ERRNO_RWLOCK_INVALID)
        return EINVAL;
    else if(ret == LOS_ERRNO_RWLOCK_PENDED)
        return EBUSY;

    return 0;
}

int rwlock_rdlock(rwlock_t *rwlock)
{
    u32_t ret;

    ret = LOS_RWReadLock(rwlock->lock, LOS_WAIT_FOREVER);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_wrlock(rwlock_t *rwlock)
{
    u32_t ret;

    ret = LOS_RWWriteLock(rwlock->lock, LOS_WAIT_FOREVER);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_timedrdlock(rwlock_t *rwlock, const struct time_spec *abstime)
{
    u32_t ret;

    ret = LOS_RWReadLock(rwlock->lock, __ts_to_tick(abstime));
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_timedwrlock(rwlock_t *rwlock, const struct time_spec *abstime)
{
    u32_t ret;

    ret = LOS_RWWriteLock(rwlock->lock, __ts_to_tick(abstime));
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_tryrdlock(rwlock_t *rwlock)
{
    u32_t ret;

    ret = LOS_RWReadLock(rwlock->lock, LOS_NO_WAIT);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_trywrlock(rwlock_t *rwlock)
{
    u32_t ret;

    ret = LOS_RWWriteLock(rwlock->lock, LOS_NO_WAIT);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int rwlock_unlock(rwlock_t *rwlock)
{
    u32_t ret;
    los_rwlock *pstRWLockPended;

    pstRWLockPended = GET_RWLOCK(rwlock->lock);
    if(pstRWLockPended->pstWOwner == g_stLosTask.pstRunTask)
    {
        ret = LOS_RWWriteUnLock(rwlock->lock);
    }
    else
    {
        ret = LOS_RWReadUnLock(rwlock->lock);
    }

    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}
