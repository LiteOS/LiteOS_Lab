/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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
#include "ota_adaptor.h"
#include "partition.h"
#include "flash_adaptor.h"
#include "common.h"

static int ota_flag_read(ota_flag_t *flag)
{
  if (flag != NULL) {
    printf("SPI FLAG R\n");
    return storage_partition_read(PART_OTA_FLAG1, (void *)flag, sizeof(ota_flag_t), 0);
  }
  return ERR;
}

static int ota_flag_write(ota_flag_t *flag)
{
  if (flag != NULL) {
    printf("SPI FLAG W:state %d\n", flag->cur_state);
    return storage_partition_write(PART_OTA_FLAG1, (void *)flag, sizeof(ota_flag_t), 0);
  }
  return ERR;
}

static int ota_bin_read(uint32_t offset, void *buf, int len)
{
  if (buf != NULL) {
    printf("SPI BIN R: %08d %08d\n", offset, len);
    return storage_partition_read(PART_OTA_IMG_DOWNLOAD, buf, len, offset);
  }
  return ERR;
}

static int ota_bin_write(uint32_t offset, void *buf, int len)
{
  if (buf != NULL) {
    printf("SPI BIN W: %08d %08d\n", offset, len);
    return storage_partition_write(PART_OTA_IMG_DOWNLOAD, buf, len, offset);
  }
  return ERR;
}

static ota_storage_t  prv_ota_flag_s =
{
   .name = "fire_ota_spi_storage",
   {
    .flag_read = ota_flag_read,
    .flag_write = ota_flag_write,
    .bin_read = ota_bin_read,
    .bin_write = ota_bin_write,
   }
};

/* 0: full upgrade, 1: diff upgrade */
EN_PACKAGE_TYPE get_package_type(const uint8_t *data, uint32_t len)
{
  ota_binary_info *info;
  if (data == NULL ||len <= sizeof(ota_binary_info))
    return ERR;

  info = (ota_binary_info *)data;
  return info->pack_type;
}

void ota_update_upgrade_result(ota_flag_t *flag, uint32_t result)
{
  if (flag != NULL) {
    flag->cur_state = EN_OTA_STATUS_UPGRADED;
    flag->ret_upgrade = result;
    flag->crc = calc_crc32(0, flag, sizeof(*flag) - sizeof(flag->crc));
    ota_storage_flag_write(flag);
  }
}

void hal_init_ota(void)
{
  printf("HAL OTA init\n");
  flash_adaptor_init();
  ota_storage_install(&prv_ota_flag_s);
}

