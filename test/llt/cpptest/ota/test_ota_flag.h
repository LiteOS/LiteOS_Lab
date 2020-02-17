#ifndef __TEST_OTA_H__
#define __TEST_OTA_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestOta : public Test::Suite
{
public:
    TestOta();
    ~TestOta();

    void test_ota_storage_install(void);
    void test_ota_storage_uninstall(void);
    void test_ota_storage_bin_read(void);
    void test_ota_storage_bin_write(void);
    void test_ota_storage_flag_read(void);
    void test_ota_storage_flag_write(void);

};



#endif /* __TEST_OTA_H__ */


