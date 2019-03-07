/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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
#include "errno.h"
#include "pthread.h"

int cond_attr_init(cond_attr_t *attr)
{
    return 0;
}

int cond_attr_destroy(cond_attr_t *attr)
{
    return 0;
}

int cond_init(cond_t *cond, const cond_attr_t *attr)
{
    int ret;

    ret = LOS_SemCreate(0, &cond->lock);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int cond_destroy(cond_t *cond)
{
    int ret;

    ret = LOS_SemDelete(cond->lock);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int cond_broadcast(cond_t *cond)
{
    int ret;

    LOS_TaskLock();

    for(;;)
    {
        ret = LOS_SemPend(cond->lock, 0);
        if(ret == LOS_ERRNO_SEM_TIMEOUT)
        {
            LOS_SemPost(cond->lock);
        }
        else if(ret == LOS_OK)
        {
            LOS_SemPost(cond->lock);
            break;
        }
        else
        {
            LOS_TaskUnlock();
            return EINVAL;
        }
    }

    LOS_TaskUnlock();

    return 0;
}

int cond_signal(cond_t *cond)
{
    int ret;

    ret = LOS_SemPost(cond->lock);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int cond_wait(cond_t *cond, mutex_t *mutex)
{
    int ret;

    if(g_pstAllMux[mutex->lock].pstOwner == (LOS_TASK_CB *)g_stLosTask.pstRunTask) 
        return EINVAL;

    mutex_unlock(mutex);
    ret = LOS_SemPend(cond->lock, LOS_WAIT_FOREVER);
    mutex_lock(mutex);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int cond_timedwait(cond_t *cond, mutex_t *mutex, const struct time_spec *ts)
{
    int ret;

    if(g_pstAllMux[mutex->lock].pstOwner == (LOS_TASK_CB *)g_stLosTask.pstRunTask) 
        return EINVAL;

    mutex_unlock(mutex);
    ret = LOS_SemPend(cond->lock, __ts_to_tick(ts));
    mutex_lock(mutex);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;

}
