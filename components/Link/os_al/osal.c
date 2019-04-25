/*
 * os_al.c
 *
 *  Created on: 2019Äê4ÔÂ26ÈÕ
 *      Author: zhangqf
 */

#include <osal_imp.h>
#include <osal.h>

static const tag_os *s_os_cb = NULL;

int osal_install(const tag_os *os)
{
    int ret = -1;

    if(NULL == s_os_cb)
    {
        s_os_cb = os;
        ret = 0;
    }

    return ret;
}


void* osal_task_create(const char *name,int (*task_entry)(void *args),\
                      void *args,int stack_size,void *stack,,int prior)
{
    void *ret = NULL;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->task_create))
    {
        ret = s_os_cb->ops->task_create(name, task_entry,stack_size,stack,prior);
    }

    return ret;
}


int osal_task_kill(void *task)
{
    int ret = -1;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->task_kill))
    {
        ret = s_os_cb->ops->task_create(task);
    }

    return ret;

}


int osal_task_exit()
{
    int ret = -1;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->task_exit))
    {
        ret = s_os_cb->ops->task_exit();
    }

    return ret;

}


int osal_task_sleep(int ms)
{
    int ret = -1;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->task_sleep))
    {
        ret = s_os_cb->ops->task_sleep(ms);
    }

    return ret;

}


bool_t  osal_mutex_create(osal_mutex_t *mutex)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->mutex_create))
    {
        ret = s_os_cb->ops->mutex_create(mutex);
    }

    return ret;

}


bool_t  osal_mutex_lock(osal_mutex_t mutex)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->mutex_lock))
    {
        ret = s_os_cb->ops->mutex_lock(mutex);
    }

    return ret;

}


bool_t  osal_mutex_unlock(osal_mutex_t mutex)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->mutex_unlock))
    {
        ret = s_os_cb->ops->mutex_unlock(mutex);
    }

    return ret;

}

bool_t  osal_mutex_del(osal_mutex_t mutex)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->mutex_del))
    {
        ret = s_os_cb->ops->mutex_del(mutex);
    }

    return ret;

}

bool_t  osal_semp_create(osal_semp_t *semp,int limit,int initvalue)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->semp_create))
    {
        ret = s_os_cb->ops->semp_create(semp,limit,initvalue);
    }

    return ret;

}

bool_t  osal_semp_pend(osal_semp_t semp,int timeout)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->semp_pend))
    {
        ret = s_os_cb->ops->semp_pend(semp,timeout);
    }

    return ret;

}


bool_t  osal_semp_post(osal_semp_t semp)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->semp_post))
    {
        ret = s_os_cb->ops->semp_post(semp);
    }

    return ret;

}

bool_t  osal_semp_del(osal_semp_t *semp)
{
    bool_t ret = false;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->semp_del))
    {
        ret = s_os_cb->ops->semp_del(semp);
    }

    return ret;

}


void *osal_mem_malloc(int size)
{
    void *ret = NULL;

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->malloc))
    {
        ret = s_os_cb->ops->malloc(size);
    }

    return ret;

}

void  osal_mem_free(void *addr)
{

    if((NULL != s_os_cb) &&(NULL != s_os_cb->ops) &&(NULL != s_os_cb->ops->free))
    {
        s_os_cb->ops->free(addr);
    }

    return;
}
