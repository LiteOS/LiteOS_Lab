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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <hal_flash.h>
#include <ota_img.h>

#define CONFIG_OTAIMG_RUNNINGBASE       (0x08020000)

#ifndef CONFIG_OTAIMG_RUNNINGSIZE
#define CONFIG_OTAIMG_RUNNINGSIZE       (256*1024)
#endif

#define CONFIG_OTAIMG_BACKUPBASE        (CONFIG_OTAIMG_RUNNINGSIZE + CONFIG_OTAIMG_RUNNINGBASE)

#ifndef CONFIG_OTAIMG_BACKUPSIZE
#define CONFIG_OTAIMG_BACKUPSIZE        (256*1024)
#endif

#define CONFIG_OTAIMG_DOWNLOADBASE      (CONFIG_OTAIMG_BACKUPSIZE + CONFIG_OTAIMG_BACKUPBASE)

#ifndef CONFIG_OTAIMG_DOWNLOADSIZE
#define CONFIG_OTAIMG_DOWNLOADSIZE      (256*1024)
#endif

#define CONFIG_OTAIMG_FLAGBASE          (CONFIG_OTAIMG_DOWNLOADSIZE + CONFIG_OTAIMG_DOWNLOADBASE)

#ifndef CONFIG_OTAIMG_FLAGSIZE
#define CONFIG_OTAIMG_FLAGSIZE          (4096)
#endif

#define CONFIG_OTAIMG_FLAGBACKUPBASE    (CONFIG_OTAIMG_FLAGSIZE + CONFIG_OTAIMG_FLAGBASE)

#ifndef CONFIG_OTAIMG_FLAGBACKUPSIZE
#define CONFIG_OTAIMG_FLAGBACKUPSIZE    (4096)
#endif

static int innerflash_read(uintptr_t arg, int offset, void *buf, int len)
{
    int ret;
    uint32_t location;

    location = (uint32_t)arg + (uint32_t)offset;
    ret = hal_flash_read(buf,len, location);

    return ret;
}

static struct {
    uint32_t location;
    bool     caching;
    union {
        uint64_t qword;
        uint8_t  buff [8];
    };
} cached_data = { 0 };

static int innerflash_flush(uintptr_t arg, int size)
{
    if (!cached_data.caching) {
        return 0;
    }

    cached_data.caching = false;

    return hal_flash_write (cached_data.buff, 8, &cached_data.location);
}

static int innerflash_write_byte (uintptr_t arg, uint32_t offset, char ch)
{
    uint32_t location = arg + offset;

    if (cached_data.caching) {
        if ((location < cached_data.location) || location - cached_data.location >= 8) {
            if (innerflash_flush (0, 0) != 0) {
                return -1;
            }
        }
    }

    if (!cached_data.caching) {
        cached_data.caching  = true;
        cached_data.location = location & ~7;
        cached_data.qword    = 0;
    }

    cached_data.buff [location - cached_data.location] = ch;

    if ((location & 7) == 7) {
        return innerflash_flush (0, 0);
    }

    return 0;
}

static int innerflash_write(uintptr_t arg, int offset, const void *buf, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (innerflash_write_byte (arg, (uint32_t) offset + i, ((const char *) buf) [i]) != 0) {
            return -1;
        }
    }

    return 0;
}

static int innerflash_erase(uintptr_t arg, int size)
{
    int ret;
    int32_t location;

    location = (int32_t)arg;
    ret = hal_flash_erase(location, size);

    return ret;
}

static const ota_img_t  g_otaimg_running = {
    .name = "RUNNING",
    .size = CONFIG_OTAIMG_RUNNINGSIZE,
    .type = EN_OTA_IMG_RUNNING,
    .arg  = (uintptr_t)CONFIG_OTAIMG_RUNNINGBASE,
    {
        .write = innerflash_write,
        .read  = innerflash_read,
        .erase = innerflash_erase,
        .flush = innerflash_flush,
    },
};


static const ota_img_t  g_otaimg_backup = {
    .name = "BACKUP",
    .size = CONFIG_OTAIMG_BACKUPSIZE,
    .type = EN_OTA_IMG_BACKUP,
    .arg  = (uintptr_t)CONFIG_OTAIMG_BACKUPBASE,
    {
        .write = innerflash_write,
        .read  = innerflash_read,
        .erase = innerflash_erase,
        .flush = innerflash_flush,
    },
};

static const ota_img_t  g_otaimg_download = {
    .name = "DOWNLOAD",
    .size = CONFIG_OTAIMG_DOWNLOADSIZE,
    .type = EN_OTA_IMG_DOWNLOAD,
    .arg  = (uintptr_t)CONFIG_OTAIMG_DOWNLOADBASE,
    {
        .write = innerflash_write,
        .read  = innerflash_read,
        .erase = innerflash_erase,
        .flush = innerflash_flush,
    },
};

static const ota_img_t  g_otaimg_flag = {
    .name = "FLAG",
    .size = CONFIG_OTAIMG_FLAGSIZE,
    .type = EN_OTA_IMG_FLAG,
    .arg  = (uintptr_t)CONFIG_OTAIMG_FLAGBASE,
    {
        .write = innerflash_write,
        .read  = innerflash_read,
        .erase = innerflash_erase,
        .flush = innerflash_flush,
    },
};

static const ota_img_t  g_otaimg_flagbackup = {
    .name = "FLAGBACKUP",
    .size = CONFIG_OTAIMG_FLAGBACKUPSIZE,
    .type = EN_OTA_IMG_FLAGBACKUP,
    .arg  = (uintptr_t)CONFIG_OTAIMG_FLAGBACKUPBASE,
    {
        .write = innerflash_write,
        .read  = innerflash_read,
        .erase = innerflash_erase,
        .flush = innerflash_flush,
    },
};

int ota_img_init()
{
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_running);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_backup);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_download);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_flag);
    ota_img_bind(EN_OTA_TYPE_FOTA,&g_otaimg_flagbackup);

    return 0;
}

