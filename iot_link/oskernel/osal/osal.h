

#ifndef __OS_AL_H
#define __OS_AL_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>


#include <osal_types.h>

//we need nothing here except the standard struct here
//kernel port here
//create a task:return the task handle here while -1 means create failed

/**
 * @brief:use this function to create a task here
 *
 * @param[in]:name, task name
 * @param[in]:task_entry, the task entry function
 * @param[in]:stack_size,the task stack size
 * @param[in]:stack, the stack memory, if NULL, which needed malloc by the system
 * @param[in]:args,the parameter pass to the entry function
 * @param[in]:prior,the prior if supported for the task;the bigger value, the lower prior
 *
 * @return:the task handle, while NULL failed
 *
 * */
void* osal_task_create(const char *name,int (*task_entry)(void *args),\
                      void *args,int stack_size,void *stack,int prior);

/**
 * @brief:use this funciton to kill the task
 *
 * @param[in]:task, the object get from task_create
 *
 * @return:0 success while false failed
 * */
int osal_task_kill(void *task);

/**
 * @brief:use this function  to exit from the current task
 *
 **/
void osal_task_exit();

/**
 * @brief:use this function to make the task to sleep
 *
 * @param[in]:ms, how much time to sleep, unit:ms
 *
 * @return:0 success while -1 failed
 **/
void osal_task_sleep(int ms);

/**
 *@brief: the mutex that the os must supplied for the link
 *
 **/

bool_t  osal_mutex_create(osal_mutex_t *mutex);
bool_t  osal_mutex_lock(osal_mutex_t mutex);
bool_t  osal_mutex_unlock(osal_mutex_t mutex);
bool_t  osal_mutex_del(osal_mutex_t mutex);

/**
 *@brief: the semp method that the os must supplied for the link
 *
 **/
bool_t  osal_semp_create(osal_semp_t *semp,int limit,int initvalue);
bool_t  osal_semp_pend(osal_semp_t semp,int timeout);
bool_t  osal_semp_post(osal_semp_t semp);
bool_t  osal_semp_del(osal_semp_t  semp);


/**
 *@brief: the memory method that the os must supplied for the link
 *
 **/
void *osal_malloc(int size);
void  osal_free(void *addr);
void *os_zalloc(int size);


/**
 * @brief:use this function to initialize the os abstract layer
 *
 * @return:0 success while -1 failed
 * */
void osal_init(void);


#endif /* __OS_AL_H */
