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
/**
 *  DATE                AUTHOR      INSTRUCTION
 *  2020-06-08 17:28  zhangqianfu  The first version
 *
 */

#include "ota_sign.h"
#include <link_log.h>
#include <string.h>
#include "mbedtls/md.h"
#include "mbedtls/pk.h"


///< attention: consider the security, the user should supply a security random function, and should rewrite the link_random(weak)
extern int mbedtls_hardware_poll(void *data,unsigned char *output, size_t len, size_t *olen);
int ota_sign(sign_para_t *para)
{
    int ret = 1;
    mbedtls_pk_context pk;

    ///check the parameters
    if((NULL == para) || (NULL == para->client_private_key) ||\
       (NULL == para->hash) || (NULL == para->sign))
    {
        return ret;
    }
    ///< do the pk init and setup
    mbedtls_pk_init(&pk);
    ret = mbedtls_pk_parse_key(&pk, para->client_private_key, strlen(para->client_private_key)+1,\
            para->client_private_key_pwd,strlen(para->client_private_key_pwd));
    if (ret != 0)
    {
        LINK_LOG_ERROR("LOAD PRIVATE KEY ERROR");
        goto EXIT_KEYPARSE;
    }

    ret = mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256, para->hash, para->hash_len, para->sign, para->sign_len,mbedtls_hardware_poll,NULL);
    if (ret != 0)
    {
        LINK_LOG_ERROR("SIGN  FAILED");
        goto EXIT_KEYPARSE;
    }
    LINK_LOG_DEBUG("SIGN  SUCCESS");
    ret = 0;

EXIT_KEYPARSE:
    mbedtls_pk_free( &pk );
    return ret;
}
