/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-7    maminjie    The first version
 */

#ifndef __TEST_HEAP_H__
#define __TEST_HEAP_H__

#include <cpptest.h>

class TestLosHeap : public Test::Suite
{
public:
    TestLosHeap();
    ~TestLosHeap();

    void test_LOS_MemInit(void);
    void test_LOS_MemAlloc(void);
    void test_LOS_MemAllocAlign(void);
    void test_LOS_MemRealloc(void);
    void test_LOS_MemFree(void);
    void test_LOS_MemStatisticsGet(void);
    void test_heap(void);

protected:
    void setup();
    void tear_down();
};

#endif // __TEST_HEAP_H__
