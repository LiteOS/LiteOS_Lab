/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"

#include "link_main.h"
#include "osal.h"
#include "queue.h"

/**
 * test task and timer
*/
#define TASK1_PRI 22
#define TASK2_PRI 24

void task1(void)
{
    int sum = 0;

    while (1) {
        printf("This is BearPi-HM_Nano task1----%d\n", sum++);
        osal_task_sleep(1000);
    }
}

void  task2(void)
{
    int sum = 0;

    while (1) {
        printf("This is BearPi-HM_Nano task2----%d\n", sum++);
        osal_task_sleep(1000);
    }
}

/**
 * 
 * test mutex 
 * 
*/
osal_mutex_t g_mutexId;

void HighPrioMutex(void)
{
    // wait 1s until start actual work
    osal_task_sleep(1000);

    while (1) {
        // try to acquire mutex
        osal_mutex_lock(g_mutexId);
        printf("HighPrioThread is running.\n");
        osal_task_sleep(3000);
        osal_mutex_unlock(g_mutexId);
    }
}

void MidPrioMutex(void)
{
    // wait 1s until start actual work
    osal_task_sleep(1000);

    while (1) {
        printf("MidPrioThread is running.\n");
        osal_task_sleep(1000);
    }
}

void LowPrioMutex(void)
{
    while (1) {
        osal_mutex_lock(g_mutexId);
        printf("LowPrioThread is running.\n");

        // block mutex for 3s
        osal_task_sleep(3000);
        osal_mutex_unlock(g_mutexId);
    }
}

/**
 * test semaphere
 * 
*/
osal_semp_t g_semaphoreId;

void Semaphore1Thread(void)
{
    while (1) {
        // release Semaphores twice so that Semaphore2Thread and Semaphore3Thread can execute synchronously
        osal_semp_post(g_semaphoreId);
        // if the Semaphore is released only once, Semaphore2Thread and Semaphore3Thread will run alternately.
        osal_semp_post(g_semaphoreId);
        printf("Semaphore1Thread Release  Semap \n");
        osal_task_sleep(1000);
    }
}

void Semaphore2Thread(void)
{
    while (1) {
        // wait Semaphore
        osal_semp_pend(g_semaphoreId, cn_osal_timeout_forever);
        printf("Semaphore2Thread get Semap \n");
        osal_task_sleep(10);
    }
}

void Semaphore3Thread(void)
{
    while (1) {
        // wait Semaphore
        osal_semp_pend(g_semaphoreId, cn_osal_timeout_forever);
        printf("Semaphore3Thread get Semap \n");
        osal_task_sleep(10);
    }
}


/**
 * test osal_queue
 * 
*/
#define MSGQUEUE_OBJECTS 16

typedef struct {
    // object data type
    char* buf;
    uint8_t idx;
} MSGQUEUE_OBJ_t;

MSGQUEUE_OBJ_t msg;

osal_queue_t g_msgQueueId;

void MsgQueue1Thread(void* argument)
{
    (void)argument;

    // do some work...
    msg.buf = "Hello BearPi-HM_Nano!";
    msg.idx = 0U;
    while (1) {
        osal_queue_send(g_msgQueueId, &msg, sizeof(msg), 0);
        // suspend thread
        osThreadYield();
        osal_task_sleep(1000);
    }
}

void MsgQueue2Thread(void* argument)
{
    osStatus_t status;

    (void)argument;
    while (1) {
        // wait for message
        osal_queue_recv(g_msgQueueId,  &msg, sizeof(msg), 0);
        printf("Message Queue Get msg:%s\n", msg.buf);
        osal_task_sleep(1000);
        
    }
}

/**
 *  test queue
 */
#define CONFIG_QUEUE_TIME_OUT 5000
queue_t *queue_id;
void Queue1Thread(void* argument)
{
    (void)argument;

    // do some work...
    msg.buf = "Hello i am queue!";
    msg.idx = 0U;
    while (1) {
        queue_push(queue_id, &msg, CONFIG_QUEUE_TIME_OUT);
        // suspend thread
        osThreadYield();
        osal_task_sleep(1000);
    }
}

void Queue2Thread(void* argument)
{
    osStatus_t status;

    (void)argument;
    while (1) {
        // wait for message
        queue_pop(queue_id, &msg, (int)cn_osal_timeout_forever);
        printf("Message Queue Get msg:%s\n", msg.buf);
        osal_task_sleep(1000);
        
    }
}

/**
 * Mem Test
*/
static int mem_access_task_entry()
{
    uint32_t i = 0;
    size_t mem_size;
    uint8_t* mem_ptr = NULL;
    while (1)
    {
        mem_size = 1 << i++;
        mem_ptr = osal_malloc(mem_size);
        if(mem_ptr != NULL)
        {
            printf("access %d bytes memory success!\r\n", mem_size);
            osal_free(mem_ptr);
            mem_ptr = NULL;
            printf("free memory success!\r\n");
           
        }
        else
        {
            printf("BearPi-HM_Nano: access %d bytes memory failed!\r\n", mem_size);
            return 0;
        }
    }
}

/**
 * Main entry of XXX_Test
*/
static void TaskTest(void)
{
    printf("this is TaskTest \n");
    link_main_task_entry();
    osal_task_create("user_task1", task1, NULL, 0x400, NULL, TASK1_PRI);
    osal_task_create("user_task2", task2, NULL, 0x400, NULL, TASK2_PRI);
}

void MutexTest(void)
{
    printf("this is MutexTest \n");
    osal_init();
    
    osal_task_create("HighPrioThread", HighPrioMutex, NULL, 0x400, NULL, 26);
    osal_task_create("MidPrioThread", MidPrioMutex, NULL, 0x400, NULL, 25);
    osal_task_create("LowPrioThread", LowPrioMutex, NULL, 0x400, NULL, 24);
    osal_mutex_create(&g_mutexId);
}

void SemaphoreTest(void)
{
    printf("this is SemaphoreTest \n");
    osal_init();
    
    osal_task_create("Semaphore1Thread", Semaphore1Thread, NULL, 0x400, NULL, 24);
    osal_task_create("Semaphore2Thread", Semaphore2Thread, NULL, 0x400, NULL, 24);
    osal_task_create("Semaphore3Thread", Semaphore3Thread, NULL, 0x400, NULL, 24);
   
    osal_semp_create(&g_semaphoreId, 4, 0);
    if (g_semaphoreId == NULL) {
        printf("Failed to create Semaphore!\n");
    }
}

static void OsalQueueTest(void)
{
    printf("this is OsalQueueTest \n");
    osal_init();
    
    osal_task_create("MsgQueue1Thread", MsgQueue1Thread, NULL, 0x1000, NULL, 24);
    osal_task_create("MsgQueue2Thread", MsgQueue2Thread, NULL, 0x1000, NULL, 24);
    osal_queue_create(&g_msgQueueId, MSGQUEUE_OBJECTS, 100);
    if (g_msgQueueId == NULL) {
        printf("Failed to create Message Queue!\n");
    }

  
}

int MemTest()
{
    printf("this is mem test\n");
    osal_init();
    osal_task_create("mem_access_task",mem_access_task_entry,NULL,0x1000,NULL,25);
    return 0;
}

static void QueueTest(void)
{
    printf("this is QueueTest \n");
    osal_init();
    
    osal_task_create("Queue1Thread", Queue1Thread, NULL, 0x1000, NULL, 24);
    osal_task_create("Queue2Thread", Queue2Thread, NULL, 0x1000, NULL, 24);
    
    if (queue_create("queue_Id", 10, 1) == NULL) {
        printf("Failed to create Message Queue!\n");
    }

}

APP_FEATURE_INIT(QueueTest);
