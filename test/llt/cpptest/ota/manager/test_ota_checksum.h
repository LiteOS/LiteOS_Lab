#ifndef __TEST_OTA_CHECKSUM_H__
#define __TEST_OTA_CHECKSUM_H__

/* Includes -----------------------------------------------------------------*/
#include <cpptest.h>
/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
class TestOtaChecksum : public Test::Suite
{
public:
    TestOtaChecksum();
    ~TestOtaChecksum();

    void test_ota_pack_calc_hash(void);
    void test_ota_pack_get_signature_verify_result(void);
    void test_ota_checksum_stub(void);
};

#endif /* __TEST_OTA_CHECKSUM_H__ */

