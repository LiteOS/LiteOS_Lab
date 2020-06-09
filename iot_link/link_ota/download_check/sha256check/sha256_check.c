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
 *  2020-06-05 09:37  zhangqianfu  The first version
 */

#include "sha256_check.h"

#include <string.h>
#include <link_misc.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <ota_img.h>

///< now we only support the SHA256

#define CN_SHA256_HASHLEN 32
#define CN_SHA256_CACHELEN (CN_SHA256_HASHLEN*2)
static int sha256_compute(en_ota_type_t  ota_type,int offset, int len, uint8_t *buf_out, int buf_out_len )
{
    int ret = -1;
    uint8_t buf_in[CN_SHA256_CACHELEN];
    int cur_len = 0;
    int file_offset;
    int file_left;

    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *md_info;

    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (md_info == NULL)
    {
        LINK_LOG_ERROR("COULD NOT GET THE HASH METHOD");
        return ret;
    }
    mbedtls_md_init( &ctx );

    if( ( ret = mbedtls_md_setup( &ctx, md_info, 0 ) ) != 0 )
    {
        LINK_LOG_ERROR("COULD NOT SETUP THE HASH METHOD");
        goto EXIT_CLEAN;
    }

    if( ( ret = mbedtls_md_starts( &ctx ) ) != 0 )
    {
        LINK_LOG_ERROR("COULD NOT START THE HASH METHOD");
        goto EXIT_CLEAN;
    }

    file_offset = offset;
    file_left = len;
    while(file_left > 0)
    {
        cur_len = file_left > CN_SHA256_CACHELEN? CN_SHA256_CACHELEN:file_left;

        ret = ota_img_read(ota_type,EN_OTA_IMG_DOWNLOAD,file_offset,buf_in, cur_len);
        if(0 != ret)
        {
            LINK_LOG_ERROR("READ DOWNLOAD DATA:OFFSET:%d LEN:%d FAILED",file_offset, cur_len);
            break;
        }
        mbedtls_md_update( &ctx, buf_in, cur_len );

        file_offset += cur_len;
        file_left -= cur_len;
    }
    if(0 == ret)
    {
        mbedtls_md_finish(&ctx, buf_out);
    }

EXIT_CLEAN:
    mbedtls_md_free( &ctx );
    return( ret );
}

/**
 * @brief: use this function to do the verify
 * @param:verify_para: the user supply the parameter to do the check
 *
 * @return: 0 success while others failed
 * */
int sha256_check(sha256_check_t *param)
{
    int ret = -1;
    uint8_t hash[CN_SHA256_HASHLEN];
    uint8_t sha256[CN_SHA256_HASHLEN];

    if(strlen(param->sha256) != (CN_SHA256_HASHLEN*2))
    {
        LINK_LOG_ERROR("SIGNATURE DATA LENGTH ERROR");
        return ret;
    }
    hexstr2byte((const char *)param->sha256, strlen(param->sha256),(char *)sha256);

    ///< compute the hash code
    ret = sha256_compute(param->ota_type,param->data_offset,param->data_len,hash,CN_SHA256_HASHLEN);
    if(ret != 0)
    {
        LINK_LOG_ERROR("HASH COMPUTE ERROR");
        return ret;
    }
    return ret;
}





