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

#include "test_ota_flag.h"

#include <cstring>

extern "C"
{
#include "ota_flag.h"
#include "ota_adaptor.h"
#include "string.h"
#include "hal_flash.h"
#include "osal.h"

extern char mem_flash[FLASH_SECTOR_ILEGAL];
extern void hal_init_ota(void);
extern int osal_init(void);
extern void ota_storage_flag_init();

}

TestOta::TestOta()
{
    osal_init();
    hal_init_ota();
    TEST_ADD(TestOta::test_ota_storage_install);
    TEST_ADD(TestOta::test_ota_storage_uninstall);
    TEST_ADD(TestOta::test_ota_storage_bin_read);
    TEST_ADD(TestOta::test_ota_storage_bin_write);
    TEST_ADD(TestOta::test_ota_storage_flag_read);
    TEST_ADD(TestOta::test_ota_storage_flag_write);
}

TestOta::~TestOta()
{

}

void TestOta::test_ota_storage_install(void)
{
    TEST_ASSERT(-1 == ota_storage_install(NULL));
}

void TestOta::test_ota_storage_uninstall(void)
{
    TEST_ASSERT(0 == ota_storage_uninstall(NULL));
}

void TestOta::test_ota_storage_bin_read(void)
{
    int ret;
    char *pbuf = NULL;
    hal_init_ota();

    pbuf = (char *)osal_malloc(FLASH_SECTOR_ILEGAL);

    ret = ota_storage_bin_read(0, (void *)NULL, FLASH_SECTOR_ILEGAL);
    TEST_ASSERT(-1 == ret);

    ret = ota_storage_bin_read(0, (void *)pbuf, FLASH_SECTOR_ILEGAL);
    TEST_ASSERT(-1 != ret);

    osal_free(pbuf);
}

void TestOta::test_ota_storage_bin_write(void)
{
    int ret;
    typedef struct msg {
        char *data;
        int len;
    } msg_t;

    msg_t *msg = (msg_t *)osal_malloc(sizeof(msg_t));
    msg->len = 10;
    msg->data = (char *)osal_malloc(msg->len);

    memset(msg->data, 0, msg->len);
    snprintf(msg->data, msg->len, "hello");

    ret = ota_storage_bin_write(0, (void *)NULL, FLASH_SECTOR_ILEGAL);
    TEST_ASSERT(-1 == ret);

    ret = ota_storage_bin_write(0, (void *)msg->data, msg->len);
    TEST_ASSERT(-1 != ret);

    osal_free(msg->data);
    osal_free(msg);

}

void TestOta::test_ota_storage_flag_read(void)
{
    int ret;
    ota_flag_t flag = {0, };

    ret = ota_storage_flag_read(NULL);
    TEST_ASSERT(-1 == ret);

    ret = ota_storage_flag_read(&flag);
    TEST_ASSERT(-1 != ret);

}

void TestOta::test_ota_storage_flag_write(void)
{
    int ret;
    ota_flag_t flag = {0, };
    ota_storage_flag_init();
    ret = ota_storage_flag_write(NULL);
    TEST_ASSERT(-1 == ret);

    ret = ota_storage_flag_write(&flag);
    TEST_ASSERT(-1 != ret);

}
