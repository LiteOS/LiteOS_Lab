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

#include "test_queue.h"
#include "cpp_stub.h"

#include <cstring>

extern "C"
{
    #include "osal_imp.h"
    #include "queue.h"
    #include "osal.h"
    #include "osal_types.h"

    int osal_init(void);
    extern queue_t* queue_create(const char *name,int len,int syncmode);
    extern int queue_push(queue_t *queue,void *data,int timeout);
    extern int queue_pop(queue_t *queue,void **buf,int timeout);
    extern int queue_delete(queue_t *queue);
    extern void* osal_task_create(const char *name,int (*task_entry)(void *args),\
                      void *args,int stack_size,void *stack,int prior);


    /* stubs */

    static void *osal_malloc_stub(size_t size)
    {
        return NULL;
    }

    static bool_t osal_semp_create_stub(osal_semp_t *semp,int limit,int initvalue)
    {
        return false;
    }

    static bool_t osal_mutex_create_stub(osal_mutex_t *mutex)
    {
        return false;
    }
}

TestQueue::TestQueue()
{
    osal_init();

    TEST_ADD(TestQueue::test_queue_create);
    TEST_ADD(TestQueue::test_queue_push);
    TEST_ADD(TestQueue::test_queue_pop);
    TEST_ADD(TestQueue::test_queue_delete);
    TEST_ADD(TestQueue::test_queue_demo);
    TEST_ADD(TestQueue::test_queue_stub);

}

TestQueue::~TestQueue()
{

}


void TestQueue::test_queue_create(void)
{
    queue_t *q1, *q2;

    q1 = queue_create("test", 10, 0);
    TEST_ASSERT(NULL != q1);
    TEST_ASSERT(!strncmp("test", q1->name, 4));

    q2 = queue_create("test", 10, 1);
    TEST_ASSERT(NULL != q2);
    TEST_ASSERT(!strncmp("test", q2->name, 4));

    TEST_ASSERT(0 == queue_delete(q2));
    TEST_ASSERT(0 == queue_delete(q1));

}

void TestQueue::test_queue_push(void)
{
    int data[50] = {123456};
    queue_t *q1, *q2;

    TEST_ASSERT(-1 == queue_push(NULL, NULL, 10));

    q1 = queue_create("test", 10, 0);
    TEST_ASSERT(0 == queue_push(q1, data, 10));

    q2 = queue_create("test", 10, 1);
    TEST_ASSERT(0 == queue_push(q2, data, 10));

    TEST_ASSERT(0 == queue_delete(q2));
    TEST_ASSERT(0 == queue_delete(q1));
}

void TestQueue::test_queue_pop(void)
{
    queue_t *q1, *q2;
    char *buf = NULL;
    int data[50] = {10210};
    TEST_ASSERT(-1 == queue_pop(NULL, NULL, 10));

    q1 = queue_create("test", 10, 0);
    TEST_ASSERT(0 == queue_push(q1, data, 10));
    TEST_ASSERT(0 == queue_pop(q1, (void**)&buf, 10));

    q2 = queue_create("test", 10, 1);
    TEST_ASSERT(0 == queue_push(q2, data, 10));
    TEST_ASSERT(0 == queue_pop(q2, (void**)&buf, 10));

    TEST_ASSERT(0 == queue_delete(q2));
    TEST_ASSERT(0 == queue_delete(q1));
}

void TestQueue::test_queue_delete(void)
{
    queue_t *q1, *q2;

    q1 = queue_create("test", 10, 0);
    TEST_ASSERT(NULL != q1);

    q2 = queue_create("test", 10, 1);
    TEST_ASSERT(NULL != q2);

    TEST_ASSERT(0 == queue_delete(q2));
    TEST_ASSERT(0 == queue_delete(q1));
    TEST_ASSERT(-1 == queue_delete(NULL));

}

void TestQueue::test_queue_demo(void)
{
    int ret;
    int idx = 0;
    queue_t *q = NULL;
    char *pbuf = NULL;
    typedef struct msg {
        char *data;
        int len;
    } msg_t;

    q = queue_create("test", 10, 1);
    TEST_ASSERT(NULL != q);

    ret = queue_pop(q, (void**)&pbuf, 10);
    TEST_ASSERT(-1 == ret);

    for(int i = 0; i < 11; i++) {
        msg_t *msg = (msg_t *)osal_malloc(sizeof(msg_t));
        msg->len = 10;
        msg->data = (char *)osal_malloc(msg->len);
        memset(msg->data, 0, msg->len);
        snprintf(msg->data, msg->len, "hello_%d", idx++);

        ret = queue_push(q, msg, 10);
        if (ret != 0) {
            osal_free(msg->data);
            osal_free(msg);
            break;
        }
    }

    for(int i = 0; i < 11; i++) {
        msg_t *msg = NULL;

        ret = queue_pop(q, (void **)&msg, 10);
        if (ret == 0 && msg != NULL) {
            if (msg->data != NULL) {
                printf("pop: %s\n", msg->data);
                osal_free(msg->data);
            }
            osal_free(msg);
        }
    }

    queue_delete(q);
}

void TestQueue::test_queue_stub(void)
{
    queue_t *q;

    // osal_malloc failed
    Stub sb;
    sb.set(osal_malloc, osal_malloc_stub);

    q = queue_create("test", 10, 1);
    TEST_ASSERT(NULL == q);

    sb.reset(osal_malloc);

    q = queue_create("test", 10, 1); // check stub restore
    TEST_ASSERT(NULL != q);

    queue_delete(q);

    // osal_semp_create failed
    sb.set(osal_semp_create, osal_semp_create_stub);
    q = queue_create("test", 10, 1);
    TEST_ASSERT(NULL == q);
    sb.reset(osal_semp_create);

    // osal_mutex_create failed
    sb.set(osal_mutex_create, osal_mutex_create_stub);
    q = queue_create("test", 10, 1);
    TEST_ASSERT(NULL == q);
    sb.reset(osal_mutex_create);
}
