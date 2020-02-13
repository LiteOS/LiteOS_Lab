#ifndef _TEST_STORAGE_
#define _TEST_STORAGE_

#include <cpptest.h>
#include <iostream>

using namespace std;

class TEST_at : public Test::Suite
{
public:
    TEST_at();
    ~TEST_at(){};

    void test_at_init(void);
    void test_at_oobregister(void);
    void test_at_command(void);
    void test_at_streammode_set(void);
    void test_shell_at(void);
    void test_shell_atdebug(void);
};

#endif