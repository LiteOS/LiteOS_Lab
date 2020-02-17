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

#include <string.h>
#include <stdlib.h>
#include <osal.h>
#include <board.h>
#include "common.h"
#include "hal_flash.h"
#include "hal_spi_flash.h"
#include "partition.h"
#include "flash_adaptor.h"

#define FLASH_BLOCK_SIZE 0x1000
#define FLASH_BLOCK_MASK 0xfff

static storage_partition s_storage_part[] = {
  {INNER_FLASH, "loader", 0x08000000, 0x00020000},
  {INNER_FLASH, "app", OTA_DEFAULT_IMAGE_ADDR, 0x000E0000},
  {SPI_FLASH, "ota_flag1", OTA_FLAG_ADDR1, 0x00004000},
  {SPI_FLASH, "ota_flag2", OTA_FLAG_ADDR2, 0x00004000},
  {SPI_FLASH, "reserved_info", RESERVED_INFO_ADDR, RESERVED_INFO_SIZE},
  {SPI_FLASH, "ota_download", OTA_IMAGE_DOWNLOAD_ADDR, OTA_IMAGE_DOWNLOAD_SIZE},
  {SPI_FLASH, "ota_backup", OTA_IMAGE_BCK_ADDR, OTA_IMAGE_BCK_SIZE},
  {SPI_FLASH, "ota_diff_uprade", OTA_IMAGE_DIFF_UPGRADE_ADDR, OTA_IMAGE_DIFF_UPGRADE_SIZE}
};

int flash_adaptor_write(uint32_t offset, const uint8_t *buffer, uint32_t len)
{
    int ret = ERR;
    uint8_t *block_buff;
    uint32_t blk_off = offset % FLASH_BLOCK_SIZE;

    if((NULL == buffer) || (0 == len) || (len > FLASH_BLOCK_SIZE))
    {
        HAL_OTA_LOG("invalid param len %ld, offset %ld", len, offset);
        return ERR;
    }

    if (len == FLASH_BLOCK_SIZE && ((offset & FLASH_BLOCK_MASK) == 0))
    {
        ret = hal_spi_flash_erase_write(buffer, FLASH_BLOCK_SIZE, offset);
        if(ret != OK)
        {
           HAL_OTA_LOG("hal_ota_write_flash fail offset %lu, len %u", offset, FLASH_BLOCK_SIZE);
        }
        return ret;
    }

    uint32_t blk_size;
    if (len + blk_off > FLASH_BLOCK_SIZE) {
      blk_size = FLASH_BLOCK_SIZE * 2;
    } else {
      blk_size = FLASH_BLOCK_SIZE;
    }

    block_buff = osal_malloc(blk_size);
    if (NULL == block_buff) {
      HAL_OTA_LOG("atiny_malloc fail");
      return ERR;
    }

    ret = hal_spi_flash_read(block_buff, blk_size, offset - blk_off);
    if(ret != OK)
    {
	HAL_OTA_LOG("hal_spi_flash_read fail offset %lu, len %lu", offset + len, FLASH_BLOCK_SIZE - len);
	goto EXIT;
    }
    (void)memcpy(block_buff + blk_off, buffer, len);
    ret = hal_spi_flash_erase_write(block_buff, blk_size, offset - blk_off);
    if(ret != OK)
    {
      HAL_OTA_LOG("hal_ota_write_flash fail offset %lu, len %u", offset, FLASH_BLOCK_SIZE);
    }
EXIT:
    osal_free(block_buff);
    return ret;
}

int app_image_restore(uint32_t src, uint32_t dst, uint32_t data_len, uint32_t head_len, uint8_t *cache, uint32_t cache_len)
{
  int rest_len = data_len - head_len;
  int blk_size = 0;
  int off = 0;

  if (cache == NULL || cache_len == 0)
    return -1;

  storage_partition_erase(dst, 0, rest_len);

  while(rest_len > 0) {
    blk_size = rest_len > cache_len ? cache_len : rest_len;

    if (storage_partition_read(src, cache, blk_size, off + head_len) ||
      storage_partition_write(dst, cache, blk_size, off))
      return -1;
    
    off += blk_size;
    rest_len -= blk_size;
  }
  return 0;
}

int app_image_backup(uint32_t src, uint32_t dst, uint32_t len, uint8_t * cache, uint32_t cache_len)
{
  int rest_len = len;
  int blk_size = 0;
  int off = 0;

  if (cache == NULL || cache_len == 0)
    return -1;

  storage_partition_erase(dst, 0, rest_len);

  while(rest_len > 0) {
    blk_size = rest_len > cache_len ? cache_len : rest_len;
    if (storage_partition_read(src, cache, blk_size, off) ||
	       storage_partition_write(dst, cache, blk_size, off)) {
      return -1;
    }
    off += blk_size;
    rest_len -= blk_size;
  }
  return 0;
}

static int hal_spi_flash_write_wrapper(const uint8_t *buf, int32_t len, uint32_t offset)
{
    return flash_adaptor_write(offset, buf, len);
}

static int hal_flash_write_wrapper(const uint8_t *buf, int32_t len, uint32_t offset)
{
    return hal_flash_write(buf, len, &offset);
}

static storage_device s_storage_dev[] = {
    {
     INNER_FLASH,
     "inner_flash",
     1024 * 1024,
     NULL,
     hal_flash_read,
     hal_flash_write_wrapper,
     hal_flash_erase,
     hal_flash_erase_write
    },
    {
     SPI_FLASH,
     "spi_flash",
     256 * 1024 * 1024,
     hal_spi_flash_config,
     hal_spi_flash_read,
     hal_spi_flash_write_wrapper,
     hal_spi_flash_erase,
     hal_spi_flash_erase_write,
   }
};

void flash_adaptor_init(void)
{
    hal_spi_flash_config();

    storage_dev_install(s_storage_dev, sizeof(s_storage_dev)/ sizeof(storage_device));
    storage_partition_init(s_storage_part, sizeof(s_storage_part)/ sizeof(storage_partition));
}
 
