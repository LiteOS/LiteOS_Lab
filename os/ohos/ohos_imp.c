/* ----------------------------------------------------------------------------
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
 * --------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 * --------------------------------------------------------------------------- */
/**
 * DATE                AUTHOR      INSTRUCTION
 * 2021-10-15          chenxingli  The first version
 *
 */

// include the file which implement the function
#include <string.h>
#include <stdlib.h>
#include "osal_imp.h"
#include "cmsis_os2.h"
#include "ohos_imp.h"

// this is implement for the task
static void __task_sleep(int ms)
{
    (void)osDelay(ms / 10); // which tick is ms that hi3861 tick is 10ms
    return;
}

static void *__task_create(const char *name, int (*task_entry)(void *args), void *args, int stack_size, void *stack,
    int prior)
{
    osThreadAttr_t attr;
    attr.name = (char *)name;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = (UINT32)stack_size;
    attr.priority = prior;

    osThreadId_t ret = osThreadNew((osThreadFunc_t)task_entry, args, &attr);
    if (ret == NULL) {
        printf("Falied to create task : %s in ohos_imp!\n", name);
    }
    return ret;
}

static int __task_kill(void *task)
{
    int ret = -1;
    if (NULL != task) {
        if (OHOS_OK == osThreadTerminate(task)) {
            ret = 0;
        }
    }

    return ret;
}

static void __task_exit(void)
{
    osThreadExit();
    return;
}

// this is implement for the mutex
// creat a mutex for the os
static bool_t __mutex_create(osal_mutex_t *mutex)
{
    osMutexId_t ret = osMutexNew(NULL);
    *mutex = ret;
    return (NULL != ret);
}

// lock the mutex
static bool_t __mutex_lock(osal_mutex_t mutex)
{
    return (OHOS_OK == osMutexAcquire((void *)mutex, OHOS_WAIT_FOREVER));
}

// unlock the mutex
static bool_t __mutex_unlock(osal_mutex_t mutex)
{
    return (OHOS_OK == osMutexRelease((void *)mutex));
}
// delete the mutex
static bool_t __mutex_del(osal_mutex_t mutex)
{
    return (OHOS_OK == osMutexDelete((void *)mutex));
}

// this is implement for the semp
// semp of the os
static bool_t __semp_create(osal_semp_t *semp, int limit, int initvalue)
{
    osSemaphoreId_t ret = osSemaphoreNew(limit, initvalue, NULL);
    *semp = ret;
    return (NULL != ret);
}

static bool_t __semp_pend(osal_semp_t semp, unsigned int timeout)
{
    if (timeout == cn_osal_timeout_forever) {
        timeout = OHOS_WAIT_FOREVER;
    }
    return (OHOS_OK == osSemaphoreAcquire(semp, (UINT32)timeout));
}

static bool_t __semp_post(osal_semp_t semp)
{
    return (OHOS_OK == osSemaphoreRelease((void *)semp));
}

static bool_t __semp_del(osal_semp_t semp)
{
    return (OHOS_OK == osSemaphoreDelete((void *)semp));
}

static bool_t __queue_create(osal_queue_t *queue, int len, int msgsize)
{
    osMessageQueueId_t ret = osMessageQueueNew((UINT32)len, (UINT32)msgsize, NULL);
    *queue = ret;
    return (NULL != ret);
}

static bool_t __queue_send(osal_queue_t queue, void *pbuf, unsigned int bufsize, unsigned int timeout)
{
    if (timeout == cn_osal_timeout_forever)
        timeout = OHOS_WAIT_FOREVER;

    return (osMessageQueuePut(queue, pbuf, 0, timeout) == OHOS_OK);
}

static bool_t __queue_recv(osal_queue_t queue, void *pbuf, unsigned int *bufsize, unsigned int timeout)
{
    return (osMessageQueueGet(queue, pbuf, NULL, timeout) == OHOS_OK);
}

static bool_t __queue_del(osal_queue_t queue)
{
    return (osMessageQueueDelete((osMessageQueueId_t)queue) == OHOS_OK);
}

// this implement for the memory management

static void *__mem_malloc(int size)
{
    void *ret = NULL;

    if (size > 0) {
        ret = malloc(size);
    }

    return ret;
}

static void __mem_free(void *addr)
{
    (void)free(addr);
}

// sys time
static unsigned long long __get_sys_time(void)
{
    return osKernelGetTickCount() * (OHOS_SYS_MS_PER_SECOND / OHOSCFG_BASE_CORE_TICK_PER_SECOND);
}

static const tag_os_ops s_ohos_ops = {
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

    .queue_create = __queue_create,
    .queue_send = __queue_send,
    .queue_recv = __queue_recv,
    .queue_del = __queue_del,

    .malloc = __mem_malloc,
    .free = __mem_free,

    .get_sys_time = __get_sys_time,
};

static const tag_os s_link_ohos = {
    .name = "ohos",
    .ops = &s_ohos_ops,
};

int os_imp_init(void)
{
    return osal_install(&s_link_ohos);
}
