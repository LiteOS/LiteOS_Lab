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
 *  2019-09-18 15:39  zhangqianfu  The first version
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ota_flag.h>
#include <crc.h>

static const ota_storage_t *s_ota_storage = NULL;

int  ota_storage_install(const ota_storage_t *device)
{
    int ret = -1;
    if((NULL == s_ota_storage) &&\
       (NULL != device->name))
    {
        s_ota_storage = device;
        ret = 0;
    }

    return ret;
}

int  ota_storage_uninstall(const ota_storage_t *device)
{

    s_ota_storage = NULL;

    return 0;
}


int ota_storage_bin_read(int offset, void *buf, int len)
{
    int ret = -1;

    if((NULL != s_ota_storage) && (NULL != s_ota_storage->opt.bin_read))
    {
        ret = s_ota_storage->opt.bin_read(offset,buf,len);
    }

    return ret;
}

int ota_storage_bin_write(int offset, void *msg, int len)
{
    int ret = -1;

    if((NULL != s_ota_storage) && (NULL != s_ota_storage->opt.bin_write))
    {
        ret = s_ota_storage->opt.bin_write(offset,msg,len);
    }

    return ret;
}



int ota_storage_flag_read(ota_flag_t *flag)
{
    int ret = -1;

    if((NULL != s_ota_storage) && (NULL != s_ota_storage->opt.flag_read))
    {
        ret = s_ota_storage->opt.flag_read(flag);
    }

    return ret;
}

int ota_storage_flag_write(ota_flag_t *flag)
{
    int ret = -1;

    if((NULL != s_ota_storage) && (NULL != s_ota_storage->opt.flag_write))
    {
        ret = s_ota_storage->opt.flag_write(flag);
    }

    return ret;
}

void ota_storage_flag_init()
{
    ota_flag_t  flag;
    memset(&flag, 0, sizeof(flag));

    flag.crc = calc_crc32(0,&flag,sizeof(flag) - sizeof(flag.crc));
    ota_storage_flag_write(&flag);
}