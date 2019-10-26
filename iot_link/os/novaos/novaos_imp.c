/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
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
/**
 *  DATE                AUTHOR      INSTRUCTION
 *  2019-10-25 09:55  zhangqianfu  The first version
 *
 */
//include the file which implement the function
#include  <string.h>
#include  <osal_imp.h>

///< this is implement for the task
#include <task.h>
#include <kconfig.h>

///< we need to translate the time mini-second to ticks: use the CONFIG_SYS_TICK_HZ
#define FN_MS_2_TICKS(ms)     (ms*CONFIG_SYS_TICK_HZ/1000)
#define FN_TICKS_2_MS(ticks)  (ticks*1000/CONFIG_SYS_TICK_HZ)

static void __task_sleep(int ms)
{
    task_delay(FN_MS_2_TICKS(ms));

    return;
}

static void *__task_create(const char *name,int (*task_entry)(void *args),\
        void *args,int stack_size,void *stack,int prior)
{
    task_id  ret = NULL;

    if((stack_size > 0) && (NULL == stack))
    {
        ret = task_spawn(name,prior, 0,stack_size + 0x1000,task_entry,(uintptr_t)args);
    }

    return ret;
}


static int __task_kill(void *task)
{
    int ret = -1;
    if(NULL != task)
    {
        ret = task_delete ((task_id) task);
    }

    return ret;
}

static void __task_exit()
{
    task_exit();
    return;
}

///< this is implement for the mutex
#include <mutex.h>
//creat a mutex for the os
static bool_t  __mutex_create(osal_mutex_t *mutex)
{
    bool_t ret = false;
    mutex_id id = NULL;

    if (NULL == mutex)
    {
        return ret;
    }

    id = mutex_create();
    if(NULL != id)
    {
        *mutex = id;
        ret = true;
    }

    return ret;

}
//lock the mutex
static bool_t  __mutex_lock(osal_mutex_t mutex)
{
    if(0 == mutex_lock(mutex))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//unlock the mutex
static bool_t  __mutex_unlock(osal_mutex_t mutex)
{
    if(0 == mutex_unlock(mutex))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//delete the mutex
static bool_t  __mutex_del(osal_mutex_t mutex)
{
    if( 0 == mutex_destroy(mutex))
    {
        return true;
    }
    else
    {
        return false;
    }
}

///< this is implement for the semp
#include <sem.h>
//semp of the os
static bool_t  __semp_create(osal_semp_t *semp,int limit,int initvalue)
{
    bool_t ret = false;

    sem_id id;

    if(NULL == semp)
    {
        return ret;
    }

    id = sem_create(initvalue);

    if(NULL != id)
    {
        *semp = id;
        ret = true;
    }

    return ret;
}

static bool_t  __semp_pend(osal_semp_t semp,int timeout)
{
    if(0 == sem_timedwait(semp,FN_MS_2_TICKS(timeout)))
    {
        return true;
    }
    else
    {
        return false;
    }
}


static bool_t  __semp_post(osal_semp_t semp)
{
    if(0 == sem_post(semp))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool_t  __semp_del(osal_semp_t semp)
{
    if(0 == sem_destroy(semp))
    {
        return true;
    }
    else
    {
        return false;
    }
}


///< this implement for the memory management
#include <stdlib.h>
static void *__mem_malloc(int size)
{
    void *ret = NULL;

    ret = malloc(size);

    return ret;
}

static void __mem_free(void *addr)
{
    free(addr);
    return;
}

///< sys time
#include <tick.h>

static unsigned long long __get_sys_time()
{
    uint64_t ticks;

    ticks = tick_count_get64();

    return FN_TICKS_2_MS(ticks);
}

__attribute__((weak)) int os_reboot()
{
    while(1);   ///< waiting for the dog if not impelment. you could implement it your self
    return 0;
}



static const tag_os_ops s_novaos_ops =
{
    .task_sleep = __task_sleep,
    .task_create = __task_create,
    .task_kill = __task_kill,
    .task_exit = __task_exit,

    .mutex_create = __mutex_create,
    .mutex_lock = __mutex_lock,
    .mutex_unlock = __mutex_unlock,
    .mutex_del = __mutex_del,

    .semp_create = __semp_create,
    .semp_pend = __semp_pend,
    .semp_post = __semp_post,
    .semp_del = __semp_del,

    .malloc = __mem_malloc,
    .free = __mem_free,

    .get_sys_time = __get_sys_time,
    .reboot = os_reboot,

};



static const tag_os s_link_novaos =
{
    .name = "NovaOS",
    .ops = &s_novaos_ops,
};

int osal_install_novaos(void)
{
    int ret = -1;

    ret = osal_install(&s_link_novaos);

    return ret;
}



