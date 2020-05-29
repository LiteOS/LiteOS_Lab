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
 *  2020-05-12 09:53  zhangqianfu  The first version
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <hal_flash.h>
#include <hal_spi_flash.h>
#include <ota_img.h>

///< we make the SPI flash as the FLAG DOWNLOAD  BACKUP
#ifndef CONFIG_OTAIMG_FLAGSIZE
#define CONFIG_OTAIMG_FLAGSIZE        4096
#endif

#ifndef CONFIG_OTAIMG_DOWNLOADSIZE
#define CONFIG_OTAIMG_DOWNLOADSIZE   (512*1024)
#endif

#ifndef CONFIG_OTAIMG_BACKUPSIZE
#define CONFIG_OTAIMG_BACKUPSIZE     (512*1024)
#endif

#define CN_OTAIMG_BASEOFFET  0
#define CN_OTAIMG_FLAG_OFFSET          (CN_OTAIMG_BASEOFFET)
#define CN_OTAIMG_DOWNLOAD_OFFSET      (CN_OTAIMG_FLAG_OFFSET + CONFIG_OTAIMG_FLAGSIZE)
#define CN_OTAIMG_BACKUP_OFFSET        (CN_OTAIMG_DOWNLOAD_OFFSET + CONFIG_OTAIMG_DOWNLOADSIZE)
#define CN_OTAIMG_OFFSETEND            (CN_OTAIMG_BACKUP_OFFSET + CONFIG_OTAIMG_BACKUPSIZE)

/*    The spi flash for OTA we used
 *    |-----flag(4k)--|----DOWNLOAD(512K)-----|----BACKUP(512K)----|
 *
 * */
static int spiflash_read(uintptr_t arg, int offset, void *buf, int len)
{
    int ret;
    uint32_t location;

    location = (uint32_t)arg + (uint32_t)offset;
    ret = hal_spi_flash_read(buf,len, location);

    return ret;
}


static int spiflash_write(uintptr_t arg, int offset,const void *buf, int len)
{
    uint32_t location;
    int ret;

    location = (uint32_t)arg + (uint32_t)offset;
    ret = hal_spi_flash_write(buf,len, &location);

    return ret;
}

static int spiflash_erase(uintptr_t arg, int size)
{
    int ret;
    int32_t location;

    location = (int32_t)arg;
    ret = hal_spi_flash_erase(location, size);

    return ret;
}

static int spiflash_flush(uintptr_t arg, int size)
{
    return 0;
}


static const ota_img_t  g_otaimg_flag = {
    .name = "FLAG",
    .size = CONFIG_OTAIMG_FLAGSIZE,
    .type = EN_OTA_IMG_FLAG,
    .arg = (uintptr_t)CN_OTAIMG_FLAG_OFFSET,
    {
        .write = spiflash_write,
        .read = spiflash_read,
        .erase = spiflash_erase,
        .flush = spiflash_flush,
    },
};


static const ota_img_t  g_otaimg_backup = {
    .name = "BACKUP",
    .size = CONFIG_OTAIMG_BACKUPSIZE,
    .type = EN_OTA_IMG_BACKUP,
    .arg = (uintptr_t)CN_OTAIMG_BACKUP_OFFSET,
    {
        .write = spiflash_write,
        .read = spiflash_read,
        .erase = spiflash_erase,
        .flush = spiflash_flush,
    },
};


static const ota_img_t  g_otaimg_download = {
    .name = "DOWNLOAD",
    .size = CONFIG_OTAIMG_DOWNLOADSIZE,
    .type = EN_OTA_IMG_DOWNLOAD,
    .arg = (uintptr_t)CN_OTAIMG_DOWNLOAD_OFFSET,
    {
        .write = spiflash_write,
        .read = spiflash_read,
        .erase = spiflash_erase,
        .flush = spiflash_flush,
    },
};

/**
 * The INNER flash FOR OTA WE USED
 *
 *     |----LOADER(128KB)---|-------RUNNINGIMG(512KB)----------|
 * */

#ifndef CONFIG_OTAIMG_RUNNINGSIZE
#define CONFIG_OTAIMG_RUNNINGSIZE        (512*1024)
#endif

#ifndef CONFIG_OTAIMG_RUNNINGBASE
#define CONFIG_OTAIMG_RUNNINGBASE        0x08020000
#endif



static int innerflash_read(uintptr_t arg, int offset, void *buf, int len)
{
    int ret;
    uint32_t location;

    location = (uint32_t)arg + (uint32_t)offset;
    ret = hal_flash_read(buf,len, location);

    return ret;
}


static int innerflash_write(uintptr_t arg, int offset,const void *buf, int len)
{
    uint32_t location;
    int ret;

    location = (uint32_t)arg + (uint32_t)offset;
    ret = hal_flash_write(buf,len, &location);

    return ret;
}

static int innerflash_erase(uintptr_t arg, int size)
{
    int ret;
    int32_t location;

    location = (int32_t)arg;
    ret = hal_flash_erase(location, size);

    return ret;
}

static int innerflash_flush(uintptr_t arg, int size)
{
    return 0;
}

static const ota_img_t  g_otaimg_running = {
    .name = "BACKUP",
    .size = CONFIG_OTAIMG_RUNNINGSIZE,
    .type = EN_OTA_IMG_RUNNING,
    .arg = (uintptr_t)CONFIG_OTAIMG_RUNNINGBASE,
    {
        .write = innerflash_write,
        .read = innerflash_read,
        .erase = innerflash_erase,
        .flush = innerflash_flush,
    },
};

int ota_img_init()
{
    hal_spi_flash_config();
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_flag);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_backup);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_download);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_running);

    return 0;
}



