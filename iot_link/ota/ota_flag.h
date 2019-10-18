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
 *  2019-09-18 12:09  zhangqianfu  The first version
 *
 */
#ifndef LITEOS_LAB_IOT_LINK_OTA_OTA_FLAG_H_
#define LITEOS_LAB_IOT_LINK_OTA_OTA_FLAG_H_

#include <stdint.h>
#include <stddef.h>

#define CN_OTA_VERSION_LEN   32

typedef enum
{
    EN_OTA_STATUS_IDLE = 0,
    EN_OTA_STATUS_DOWNLOADING,
    EN_OTA_STATUS_DOWNLOADED,
    EN_OTA_STATUS_UPGRADING,
    EN_OTA_STATUS_UPGRADED,
}en_ota_status_t;


#pragma pack(1)
typedef struct
{
    uint8_t  ver[CN_OTA_VERSION_LEN];
    uint32_t ver_code;
    uint32_t file_size;   ///< the new bin file size
    uint32_t blk_size;    ///< the new bin block size
    uint32_t blk_num;     ///< the new bin block num
    uint32_t blk_cur;
    uint32_t file_off;    ///< the current offet to write
    uint32_t cur_state;   ///< defined by en_ota_status_t
    uint32_t ret_upgrade; ///< the upgrade,filled by the loader
    uint32_t crc;         ///< all the ota information computed
}ota_flag_t;
#pragma pack()


typedef struct
{
    int (*flag_read) (ota_flag_t *flag);                ///< call this function will
    int (*flag_write) (ota_flag_t *flag);
    int (*bin_write) (int offset, void *msg, int len);  ///< call this function will write the new bin to the storage
    int (*bin_read) (int offset, void *buf, int len);   ///< call this function will read the new bin from the storage
    ///< call this function will read the new bin from the storage
}ota_storage_opt_t;

typedef struct
{
    const char        *name;
    ota_storage_opt_t  opt;
}ota_storage_t;


int  ota_storage_install(const ota_storage_t *device);
int  ota_storage_uninstall(const ota_storage_t *device);
int ota_storage_bin_read(int offset, void *buf, int len);
int ota_storage_bin_write(int offset, void *msg, int len);
int ota_storage_flag_read(ota_flag_t *flag);
int ota_storage_flag_write(ota_flag_t *flag);








#endif /* LITEOS_LAB_IOT_LINK_OTA_OTA_FLAG_H_ */
