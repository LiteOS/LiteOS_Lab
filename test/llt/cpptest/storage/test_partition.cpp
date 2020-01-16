#include "test_storage.h"

extern "C" {
#include "partition.h"
extern int storage_partition_init(storage_partition *part, int32_t max_num);
extern int storage_partition_deinit();
}

using namespace std;

TEST_partition::TEST_partition()
{
    TEST_ADD(TEST_partition::test_storage_partition_init);
    TEST_ADD(TEST_partition::test_storage_partition_deinit);
    TEST_ADD(TEST_partition::test_storage_partition_read);
    TEST_ADD(TEST_partition::test_storage_partition_write);
    TEST_ADD(TEST_partition::test_storage_partition_erase_write);
    TEST_ADD(TEST_partition::test_storage_partition_erase)
;}

static const storage_partition test_storage_partition[] = {
    {0, (char *)"part_0", 0x0000, 0x0100},
    {0, (char *)"part_1", 0x0100, 0x0200},
    {1, (char *)"part_0", 0x0000, 0x0100},
    {1, (char *)"part_1", 0x0100, 0x0200},
};

void TEST_partition::test_storage_partition_init()
{
    int ret = storage_partition_init(NULL, 0);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    TEST_ASSERT (ret == 0);

    ret = storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    TEST_ASSERT (ret == -1);

    (void)storage_partition_deinit();
}

void TEST_partition::test_storage_partition_deinit()
{
    int ret = storage_partition_deinit();
    TEST_ASSERT (ret == -1);

    (void)storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    ret = storage_partition_deinit();
    TEST_ASSERT (ret == 0);
}

void TEST_partition::test_storage_partition_read()
{
    char buff[100] = {0};
    int ret = storage_partition_read(777, (uint8_t *)buff, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_read(0, NULL, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    ret = storage_partition_read(1, (uint8_t *)buff, 0x1000, 0x1000);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_read(1, (uint8_t *)buff, 0x50, 0x50);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_deinit();
}

void TEST_partition::test_storage_partition_write()
{
    char buff[100] = {0};
    int ret = storage_partition_write(777, (uint8_t *)buff, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_write(0, NULL, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    ret = storage_partition_write(1, (uint8_t *)buff, 0x1000, 0x1000);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_write(1, (uint8_t *)buff, 0x50, 0x50);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_deinit();
}

void TEST_partition::test_storage_partition_erase_write()
{
    char buff[100] = {0};
    int ret = storage_partition_erase_write(777, (uint8_t *)buff, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_erase_write(0, NULL, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    ret = storage_partition_erase_write(1, (uint8_t *)buff, 0x1000, 0x1000);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_erase_write(1, (uint8_t *)buff, 0x50, 0x50);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_deinit();
}

void TEST_partition::test_storage_partition_erase()
{
    int ret = storage_partition_erase(777, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_erase(0, 0x400, 0x100);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_init((storage_partition *)test_storage_partition, sizeof(test_storage_partition) / sizeof(storage_partition));
    ret = storage_partition_erase(1, 0x1000, 0x1000);
    TEST_ASSERT (ret == -1);

    ret = storage_partition_erase(1, 0x50, 0x50);
    TEST_ASSERT (ret == -1);

    (void)storage_partition_deinit();
}
