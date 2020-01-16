/**
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>. All rights reserved.
 *
 *  DATE        AUTHOR      INSTRUCTION
 *  2020-1-8    maminjie    The first version
 */

#ifndef __TEST_TICKLESS_H__
#define __TEST_TICKLESS_H__

#include <cpptest.h>

class TestTickless : public Test::Suite
{
public:
    TestTickless();
    ~TestTickless();

    void test_demo(void);

protected:
    void setup();
    void tear_down();
};

#endif // __TEST_TICKLESS_H__
