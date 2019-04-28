/*
 * osal_imp.h
 *
 *  Created on: 2019Äê4ÔÂ26ÈÕ
 *      Author: zhangqf
 */

#ifndef __OSAL_IMP_H
#define __OSAL_IMP_H


#include <osal_types.h>

typedef struct
{
    ///< task function needed
    void* (*task_create)(const char *name,int (*task_entry)(void *args),\
                      void *args,int stack_size,void *stack,int prior);
    int   (*task_kill)(void *task);
    void  (*task_exit)();
    void  (*task_sleep)(int ms);

    ///< mutex function needed
    bool_t  (* mutex_create)(osal_mutex_t *mutex);
    bool_t  (* mutex_lock)(osal_mutex_t mutex);
    bool_t  (* mutex_unlock)(osal_mutex_t mutex);
    bool_t  (* mutex_del)(osal_mutex_t mutex);

    ///< semp function needed
    bool_t (*semp_create)(osal_semp_t *semp,int limit,int initvalue);
    bool_t (*semp_pend)(osal_semp_t semp,int timeout);
    bool_t (*semp_post)(osal_semp_t semp);
    bool_t (*semp_del)(osal_semp_t semp);

    ///< memory function needed
    void *(*malloc)(int size);
    void  (*free)(void *addr);

}tag_os_ops;


typedef struct
{
    const char         *name;  ///< operation system name
    const tag_os_ops   *ops;   ///< system function interface
}tag_os;

int osal_install(const tag_os *os); //install the os to the link layer


#endif /* __OSAL_IMP_H */
