#include "test_ota_pack.h"
#include <cstring>

extern "C"
{
#include "ota_flag.h"
#include "ota_adaptor.h"
#include "string.h"
#include "hal_flash.h"
#include "osal.h"
#include "ota_manager.h"


extern int osal_init(void);
extern pack_storage_device_api_s *pack_get_device(void);
extern int ota_pack_observe_info_read(uint8_t *buf, uint32_t len);
extern int ota_pack_observe_info_write(uint8_t *buf, uint32_t len);
extern int ota_pack_man_active_software(pack_storage_device_api_s *thi);
extern int ota_pack_man_software_write_end(pack_storage_device_api_s *t, pack_download_result_e result, uint32_t len);
extern int ota_pack_man_software_write(pack_storage_device_api_s *t, uint32_t offset, uint8_t *buf, uint32_t len);
}


TestOtaPack::TestOtaPack()
{
    TEST_ADD(TestOtaPack::test_pack_get_device);
    TEST_ADD(TestOtaPack::test_ota_pack_observe_info_read);
	TEST_ADD(TestOtaPack::test_ota_pack_observe_info_write);
    TEST_ADD(TestOtaPack::test_ota_pack_man_active_software);
	TEST_ADD(TestOtaPack::test_ota_pack_man_software_write_end);
    TEST_ADD(TestOtaPack::test_ota_pack_man_software_write);
}


TestOtaPack:: ~TestOtaPack()
{

}


void TestOtaPack::test_pack_get_device(void)
{
    pack_storage_device_api_s *ret;
    ret = pack_get_device();

	TEST_ASSERT(ret != NULL);
}

void TestOtaPack::test_ota_pack_observe_info_read(void)
{
    int ret;
	uint8_t* buff = NULL;

    ret = ota_pack_observe_info_read(buff, sizeof(buff));
    TEST_ASSERT(ret == -1);
}

void TestOtaPack::test_ota_pack_observe_info_write(void)
{
    int ret;
	uint8_t* buff = NULL;

    ret = ota_pack_observe_info_write(buff, sizeof(buff));
    TEST_ASSERT(ret == -1);
}


void TestOtaPack::test_ota_pack_man_active_software(void)
{
    int ret;
	ret = ota_pack_man_active_software(NULL);

    TEST_ASSERT(ret == 0);
}


void TestOtaPack::test_ota_pack_man_software_write_end(void)
{
    int ret;

    ret = ota_pack_man_software_write_end(NULL,PACK_DOWNLOAD_OK,256);
    TEST_ASSERT(ret == -1);

	ret = ota_pack_man_software_write_end(NULL,PACK_DOWNLOAD_FAIL,256);
    TEST_ASSERT(ret == -1);

}



void  TestOtaPack::test_ota_pack_man_software_write(void)
{
    int ret;
    uint8_t* buff = NULL;

    ret = ota_pack_man_software_write(NULL, 0, buff, 256);
    TEST_ASSERT(ret == 0);
}
