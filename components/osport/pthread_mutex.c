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
#include "pthread.h"

#include "errno.h"

static const mutex_attr_t def_mutex_attr =
{
    .type = MUTEX_DEFAULT,
};

int mutex_init(mutex_t *mutex, const mutex_attr_t *attr)
{
    int ret;
    if(attr == NULL)
        mutex->attr.type = def_mutex_attr.type;
    else
        mutex->attr.type = attr->type;

    ret = LOS_MuxCreate(&mutex->lock);
    if(ret != LOS_OK)
        return EINVAL;

    return 0;
}

int mutex_destroy(mutex_t *mutex)
{
    int ret;
    
    ret = LOS_MuxDelete(mutex->lock);
    if(ret == LOS_ERRNO_MUX_INVALID)
        return EINVAL;
    else if(ret == LOS_ERRNO_MUX_PENDED)
        return EBUSY;

    return 0;
}

int mutex_trylock(mutex_t *mutex)
{
    int ret;
    u32_t uwIntSave;

    uwIntSave = LOS_IntLock();
    if(g_pstAllMux[mutex->lock].pstOwner == (LOS_TASK_CB *)g_stLosTask.pstRunTask &&
       mutex->attr.type != MUTEX_RECURSIVE)
    {
        LOS_IntRestore(uwIntSave);
        return EDEADLK;
    }
    LOS_IntRestore(uwIntSave);

    ret = LOS_MuxPend(mutex->lock, LOS_NO_WAIT);
    if(ret == LOS_OK)
        return 0;

    return EBUSY;
}

int mutex_lock(mutex_t *mutex)
{
    int ret;
    u32_t uwIntSave;

    uwIntSave = LOS_IntLock();
    if(g_pstAllMux[mutex->lock].pstOwner == (LOS_TASK_CB *)g_stLosTask.pstRunTask &&
       mutex->attr.type != MUTEX_RECURSIVE)
    {
        LOS_IntRestore(uwIntSave);
        return EDEADLK;
    }
    LOS_IntRestore(uwIntSave);

    ret = LOS_MuxPend(mutex->lock, LOS_WAIT_FOREVER);
    if(ret == LOS_OK)
        return 0;

    return EINVAL;
}

int mutex_timedlock(mutex_t *mutex, const struct time_spec *ts)
{
    int ret;
    u32_t uwIntSave;

    uwIntSave = LOS_IntLock();
    if(g_pstAllMux[mutex->lock].pstOwner == (LOS_TASK_CB *)g_stLosTask.pstRunTask &&
       mutex->attr.type != MUTEX_RECURSIVE)
    {
        LOS_IntRestore(uwIntSave);
        return EDEADLK;
    }
    LOS_IntRestore(uwIntSave);

    ret = LOS_MuxPend(mutex->lock, __ts_to_tick(ts));
    if(ret == LOS_OK)
        return 0;

    return EINVAL;
}

int mutex_unlock(mutex_t *mutex)
{
    int ret;

    ret = LOS_MuxPost(mutex->lock);
    if(ret == LOS_OK)
        return 0;

    return EINVAL;
}

int mutex_attr_init(mutex_attr_t *attr)
{
    if(attr)
    {
        *attr = def_mutex_attr;
        return 0;
    }

    return EINVAL;
}

int mutex_attr_destroy(mutex_attr_t *attr)
{
    if(attr)
    {
        attr->type = -1;
        return 0;
    }

    return EINVAL;
}

int mutex_attr_gettype(const mutex_attr_t *attr, int *type)
{
    if(attr && type)
    {
        if(attr->type == MUTEX_NORMAL || attr->type == MUTEX_RECURSIVE)
        {
            *type = attr->type;
            return 0;
        }
    }

    return EINVAL;
}

int mutex_attr_settype(mutex_attr_t *attr, int type)
{
    if(attr)
    {
        if(type == MUTEX_NORMAL || type == MUTEX_RECURSIVE)
        {
            attr->type = type;
            return 0;
        }
    }

    return EINVAL;
}

int mutex_attr_setpshared(mutex_attr_t *attr, int  pshared)
{
    return 0;
}

int mutex_attr_getpshared(mutex_attr_t *attr, int *pshared)
{
    return 0;
}
