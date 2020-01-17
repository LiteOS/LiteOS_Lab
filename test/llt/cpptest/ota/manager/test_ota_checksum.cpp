/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "test_ota_checksum.h"
#include "cpp_stub.h"

#include <cstring>

#define HASH_LEN_32 (32)

extern "C"
{
    #include "ota_flag.h"
    #include "ota_adaptor.h"
    #include "string.h"
    #include "hal_flash.h"
    #include "osal.h"
    #include "mbedtls/md.h"

    extern char mem_flash[FLASH_SECTOR_ILEGAL];
    extern void hal_init_ota(void);
    extern int osal_init(void);
    extern void ota_storage_flag_init();

    extern int ota_pack_calc_hash(uint8_t *hash_out);
    extern int ota_pack_get_signature_verify_result(int sign_len, int file_len);

    /* stubs */
    static const mbedtls_md_info_t *mbedtls_md_info_from_type_stub( mbedtls_md_type_t md_type )
    {
        return NULL;
    }

    static int mbedtls_md_setup_stub( mbedtls_md_context_t *ctx, const mbedtls_md_info_t *md_info, int hmac )
    {
        return -1;
    }

    static int mbedtls_md_starts_stub( mbedtls_md_context_t *ctx )
    {
        return -1;
    }
}

TestOtaChecksum::TestOtaChecksum()
{
    osal_init();
    hal_init_ota();
    TEST_ADD(TestOtaChecksum::test_ota_pack_calc_hash);
    TEST_ADD(TestOtaChecksum::test_ota_pack_get_signature_verify_result);
    TEST_ADD(TestOtaChecksum::test_ota_checksum_stub);
}

TestOtaChecksum::~TestOtaChecksum()
{

}

void TestOtaChecksum::test_ota_pack_calc_hash(void)
{
    TEST_ASSERT(0 != ota_pack_calc_hash(NULL));

    uint8_t *hash_cache = (uint8_t *)osal_malloc(HASH_LEN_32);
    TEST_ASSERT(NULL != hash_cache);
    memset(hash_cache, 0, HASH_LEN_32);

    TEST_ASSERT(0 == ota_pack_calc_hash(hash_cache));
    osal_free((uint8_t *)hash_cache);
}

void TestOtaChecksum::test_ota_pack_get_signature_verify_result(void)
{
    TEST_ASSERT(0 != ota_pack_get_signature_verify_result(256, 256));
    TEST_ASSERT(0 != ota_pack_get_signature_verify_result(256, 10));
    TEST_ASSERT(0 != ota_pack_get_signature_verify_result(256, 512));
    TEST_ASSERT(0 != ota_pack_get_signature_verify_result(256, 2048));
}

void TestOtaChecksum::test_ota_checksum_stub(void)
{
    uint8_t *hash_cache = (uint8_t *)osal_malloc(HASH_LEN_32);
    TEST_ASSERT(NULL != hash_cache);
    memset(hash_cache, 0, HASH_LEN_32);

    // osal_malloc failed
    Stub sb;
    sb.set(mbedtls_md_info_from_type, mbedtls_md_info_from_type_stub);

    TEST_ASSERT(0 != ota_pack_calc_hash(hash_cache));

    sb.reset(mbedtls_md_info_from_type);

    sb.set(mbedtls_md_setup, mbedtls_md_setup_stub);

    TEST_ASSERT(0 != ota_pack_calc_hash(hash_cache));

    sb.reset(mbedtls_md_setup);

    sb.set(mbedtls_md_starts, mbedtls_md_starts_stub);

    TEST_ASSERT(0 != ota_pack_calc_hash(hash_cache));

    sb.reset(mbedtls_md_starts);

    osal_free((uint8_t *)hash_cache);
}

