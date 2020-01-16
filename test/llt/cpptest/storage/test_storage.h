#ifndef _TEST_STORAGE_
#define _TEST_STORAGE_

#include <cpptest.h>
#include <iostream>

class TEST_storage : public Test::Suite
{
public:
    TEST_storage();
    ~TEST_storage(){};

    void test_storage_dev_install(void);
    void test_storage_dev_uninstall(void);
    void test_storage_device_erase(void);
    void test_storage_device_erase_write(void);
    void test_storage_device_write(void);
    void test_storage_device_read(void);
};

class TEST_partition : public Test::Suite
{
public:
    TEST_partition();
    ~TEST_partition(){};

    void test_storage_partition_init(void);
    void test_storage_partition_deinit(void);
    void test_storage_partition_read(void);
    void test_storage_partition_write(void);
    void test_storage_partition_erase_write(void);
    void test_storage_partition_erase(void);
};

#endif