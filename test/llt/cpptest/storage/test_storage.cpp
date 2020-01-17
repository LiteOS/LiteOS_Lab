#include "test_storage.h"

extern "C" {
#include "storage.h"
}

using namespace std;

TEST_storage::TEST_storage()
{
    TEST_ADD(TEST_storage::test_storage_dev_install);
    TEST_ADD(TEST_storage::test_storage_dev_uninstall);
    TEST_ADD(TEST_storage::test_storage_device_erase);
    TEST_ADD(TEST_storage::test_storage_device_erase_write);
    TEST_ADD(TEST_storage::test_storage_device_write);
    TEST_ADD(TEST_storage::test_storage_device_read);
}

static void test_storage_init()
{
    cout << "I`m init" << endl;
}

static int test_storage_read(void *buf, int32_t len, uint32_t offset)
{
    cout << "I`m read" << endl;
    return 0;
}

static int test_storage_write(const uint8_t *buf, int32_t len, uint32_t offset)
{
    cout << "I`m write" << endl;
    return 0;
}

static int test_storage_erase(uint32_t offset, int32_t len)
{
    cout << "I`m erase" << endl;
    return 0;
}

static int test_storage_erase_write(const void *buf, int32_t len, uint32_t offset)
{
    cout << "I`m erase_write" << endl;
    return 0;
}

typedef void (*init)(void);
static storage_device test_storage_device[] = {
    {
        0,
        (char *)"storage_0",
        1024 * 1024,
        test_storage_init,
        test_storage_read,
        test_storage_write,
        test_storage_erase,
        test_storage_erase_write,
    },
    {
        1,
        (char *)"storage_1",
        1024 * 1024,
        test_storage_init,
        test_storage_read,
        test_storage_write,
        test_storage_erase,
        test_storage_erase_write,
    }
};

void TEST_storage::test_storage_dev_install()
{
    int ret = storage_dev_install(NULL, 0);
    TEST_ASSERT (ret == -1);

    ret = storage_dev_install(test_storage_device, sizeof(test_storage_device) / sizeof(storage_device));
    TEST_ASSERT (ret == 0);

    (void)storage_dev_uninstall();
}

void TEST_storage::test_storage_dev_uninstall()
{
    (void)storage_dev_install(test_storage_device, sizeof(test_storage_device) / sizeof(storage_device));

    int ret = storage_dev_uninstall();
    TEST_ASSERT (ret == 0);

    ret = storage_dev_uninstall();
    TEST_ASSERT (ret == -1);
}

void TEST_storage::test_storage_device_erase()
{
    int ret;
    ret = storage_device_erase(0, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    storage_device test_storage_device_tmp= test_storage_device[0];
    test_storage_device_tmp.erase = NULL;
    (void)storage_dev_install(&test_storage_device_tmp, sizeof(test_storage_device_tmp) / sizeof(storage_device));
    ret = storage_device_erase(0, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    (void)storage_dev_uninstall();
    (void)storage_dev_install(test_storage_device, sizeof(test_storage_device) / sizeof(storage_device));
    ret = storage_device_erase(999, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    ret = storage_device_erase(0, 0x1000, 0x400);
    TEST_ASSERT (ret == 0);

    (void)storage_dev_uninstall();
}

void TEST_storage::test_storage_device_erase_write()
{
    int ret;
    ret = storage_device_erase_write(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    storage_device test_storage_device_tmp= test_storage_device[0];
    test_storage_device_tmp.erase_write = NULL;
    (void)storage_dev_install(&test_storage_device_tmp, sizeof(test_storage_device_tmp) / sizeof(storage_device));
    ret = storage_device_erase_write(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    (void)storage_dev_uninstall();
    (void)storage_dev_install(test_storage_device, sizeof(test_storage_device) / sizeof(storage_device));
    ret = storage_device_erase_write(888, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    ret = storage_device_erase_write(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == 0);

    (void)storage_dev_uninstall();
}

void TEST_storage::test_storage_device_write()
{
    int ret;
    ret = storage_device_write(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    storage_device test_storage_device_tmp= test_storage_device[0];
    test_storage_device_tmp.write = NULL;
    (void)storage_dev_install(&test_storage_device_tmp, sizeof(test_storage_device_tmp) / sizeof(storage_device));
    ret = storage_device_write(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    (void)storage_dev_uninstall();
    (void)storage_dev_install(test_storage_device, sizeof(test_storage_device) / sizeof(storage_device));
    ret = storage_device_write(888, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    ret = storage_device_write(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == 0);

    (void)storage_dev_uninstall();
}

void TEST_storage::test_storage_device_read()
{
    int ret;
    ret = storage_device_read(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    storage_device test_storage_device_tmp= test_storage_device[0];
    test_storage_device_tmp.read = NULL;
    (void)storage_dev_install(&test_storage_device_tmp, sizeof(test_storage_device_tmp) / sizeof(storage_device));
    ret = storage_device_read(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    (void)storage_dev_uninstall();
    (void)storage_dev_install(test_storage_device, sizeof(test_storage_device) / sizeof(storage_device));
    ret = storage_device_read(888, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == -1);

    ret = storage_device_read(0, NULL, 0x1000, 0x400);
    TEST_ASSERT (ret == 0);

    (void)storage_dev_uninstall();
}
