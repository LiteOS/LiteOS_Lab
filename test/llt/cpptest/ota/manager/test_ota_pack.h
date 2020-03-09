#ifndef __TEST_OTA_PACK_H__
#define __TEST_OTA_PACK_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestOtaPack : public Test::Suite
{
public:
    TestOtaPack();
    ~TestOtaPack();
    void test_pack_get_device(void);
	void test_ota_pack_observe_info_read();
    void test_ota_pack_observe_info_write();
    void test_ota_pack_man_active_software();
	void test_ota_pack_man_software_write_end();
    void test_ota_pack_man_software_write();
};



#endif /* __TEST_OTA_PACK_H__ */


