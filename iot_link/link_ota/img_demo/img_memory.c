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
 *  2020-05-30 16:44  zhangqianfu  The first version
 *
 */
#include <stdlib.h>
#include <string.h>
#include <link_log.h>
#include <ota_img.h>
#include <ota_flag.h>


#define CN_IMG_TESTSIZE             0X2000
#define CN_IMG_TESTBASE             0

#define CN_IMG_TEST_FLAGBASE        0
#define CN_IMG_TEST_FLAGSIZE        0X200

#define CN_IMG_TEST_RUNNINGBASE    (CN_IMG_TEST_FLAGBASE + CN_IMG_TEST_FLAGSIZE)
#define CN_IMG_TEST_RUNNINGSIZE     0X800

#define CN_IMG_TEST_DOWNLOADBASE    (CN_IMG_TEST_RUNNINGBASE + CN_IMG_TEST_RUNNINGSIZE)
#define CN_IMG_TEST_DOWNLOADSIZE    0X800

#define CN_IMG_TEST_BACKUPBASE      (CN_IMG_TEST_DOWNLOADBASE + CN_IMG_TEST_DOWNLOADSIZE)
#define CN_IMG_TEST_BACKUPSIZE    0X800

static uint8_t g_testimg_mem[CN_IMG_TESTSIZE];

static int imgerase_func(uintptr_t  arg, int imgsize)
{
    int location = (int)arg;

    memset(&g_testimg_mem[location],0,imgsize);

    return 0;
}

static int imgflush_func(uintptr_t  arg, int imgsize)
{
    return 0;
}

static int imgread_func(uintptr_t  arg, int offset, void *buf, int len)
{
    int location = (int)arg + offset;

    memcpy(buf,&g_testimg_mem[location],len);

    return 0;
}


static int imgwrite_func(uintptr_t  arg, int offset,const void *buf, int len)
{
    int location = (int)arg + offset;

    memcpy(&g_testimg_mem[location],buf,len);

    return 0;
}

static const ota_img_t  g_otaimg_flag = {
    .name = "FLAG",
    .type = EN_OTA_IMG_FLAG,
    .size = CN_IMG_TEST_FLAGSIZE,
    .arg = (uintptr_t)CN_IMG_TEST_FLAGBASE,
    {
       .write = imgwrite_func,
       .read = imgread_func,
       .erase = imgerase_func,
       .flush = imgflush_func,
    },
};

static const ota_img_t  g_otaimg_running = {
    .name = "RUNNING",
    .type = EN_OTA_IMG_RUNNING,
    .size = CN_IMG_TEST_RUNNINGSIZE,
    .arg = (uintptr_t)CN_IMG_TEST_RUNNINGBASE,
    {
       .write = imgwrite_func,
       .read = imgread_func,
       .erase = imgerase_func,
       .flush = imgflush_func,
    },
};

static const ota_img_t  g_otaimg_download = {
    .name = "DOWNLOAD",
    .type = EN_OTA_IMG_DOWNLOAD,
    .size = CN_IMG_TEST_DOWNLOADSIZE,
    .arg = (uintptr_t)CN_IMG_TEST_DOWNLOADBASE,
    {
       .write = imgwrite_func,
       .read = imgread_func,
       .erase = imgerase_func,
       .flush = imgflush_func,
    },
};

static const ota_img_t  g_otaimg_backup = {
    .name = "BACKUP",
    .type = EN_OTA_IMG_BACKUP,
    .size = CN_IMG_TEST_BACKUPSIZE,
    .arg = (uintptr_t)CN_IMG_TEST_BACKUPBASE,
    {
       .write = imgwrite_func,
       .read = imgread_func,
       .erase = imgerase_func,
       .flush = imgflush_func,
    },
};

int ota_img_init()
{
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_flag);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_backup);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_running);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_download);
    return 0;
}
