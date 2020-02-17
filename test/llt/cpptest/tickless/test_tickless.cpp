/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-8    maminjie    The first version
 */

#include "test_tickless.h"

#include "los_adapter.h"
#include "los_tickless.h"

#include <unistd.h>

extern "C"
{
#include "osal.h"
#include "los_tickless.ph"

    extern BOOL g_bTickIrqFlag;
}

/* functions */

TestTickless::TestTickless()
{
    osal_init();

    LOS_TicklessEnable();

    TEST_ADD(TestTickless::test_demo);
}

TestTickless::~TestTickless()
{
    LOS_TicklessDisable();
}

void TestTickless::setup()
{

}

void TestTickless::tear_down()
{

}

static int quit_flag = 0;

static int idle_task(void *args)
{
    while (!quit_flag)
    {
        osTicklessHandler();
    }
    return 0;
}

static void *__osal_task_create(const char *name, int (*task_entry)(void *args),
        void *args,int stack_size,void *stack,int prior)
{
    void *ret = NULL;
    pthread_t pid;
    pthread_attr_t a;

    pthread_attr_init(&a);
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&pid, &a, (void *(*)(void *))task_entry, args) != 0) {
        return ret;
    }

    ret = (void *)pid;
    return ret;
}

void TestTickless::test_demo(void)
{
    osTicklessStart();

    void *handle = __osal_task_create("idle", idle_task, 0, 0x800, NULL, 1);
    if (NULL == handle)
    {
        return;
    }

    g_bTickIrqFlag = 0;
    sleep(1);

    g_bTickIrqFlag = 1;
    sleep(1);

    quit_flag = 1;

    usleep(100000); // wait task exit
}
