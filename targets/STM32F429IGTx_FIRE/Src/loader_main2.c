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
 *  2019-05-28 09:48  zhangqianfu  The first version
 *
 */

/**
 * @brief: this is the loader main file, if you need the loader, you should compile this
 *         this file  and run this code first
 * */
#include <stdio.h>
#include "osal.h"
#include "ota_adaptor.h"
#include "flash_adaptor.h"
#include "libHDiffPatch/HPatch/patch.h"
#include "decompress_plugin_demo.h"
#include "partition.h"

static hpatch_BOOL _read_storage_partition_stream(const hpatch_TStreamInput *stream, hpatch_StreamPos_t read_pos,
				      uint8_t * data, uint8_t * data_end)
{
  const uint32_t part = (const uint32_t)stream->streamImport;
  if ((read_pos + data_end - data) <= stream->streamSize) {
    if (part == PART_OTA_IMG_DOWNLOAD) read_pos += OTA_BINARY_OFFSET;
    int ret = storage_partition_read(part, data, data_end - data, read_pos);
  return hpatch_TRUE;
  } else {
    return hpatch_FALSE;
  }
}
void downloadimg_as_hStreamInput(hpatch_TStreamInput *stream, uint32_t part, uint32_t len)
{
  stream->streamImport = (void*)part;
  stream->streamSize = len;
  stream->read = _read_storage_partition_stream;
}

static hpatch_BOOL _write_storage_partition_stream(const hpatch_TStreamOutput *stream, hpatch_StreamPos_t write_pos,
					   const uint8_t *data, const uint8_t *data_end)
{
  const uint32_t part = (const uint32_t)stream->streamImport;
  if (write_pos + (data_end - data) <= stream->streamSize) {
    storage_partition_write(part, data, data_end - data, write_pos);
    return hpatch_TRUE;
  } else {
    return hpatch_FALSE;
  }
}
void upgradeimg_as_hStreamOutput(hpatch_TStreamOutput *stream, uint32_t part, uint32_t len)
{
  stream->streamImport = (void*)part;
  stream->streamSize = len;
  stream->read_writed = _read_storage_partition_stream;
  stream->write = _write_storage_partition_stream;
}

void originimg_as_hStreamInput(hpatch_TStreamInput *stream, uint32_t part, uint32_t len)
{
  stream->streamImport = (void *)part;
  stream->streamSize = len;
  stream->read = _read_storage_partition_stream;
}

hpatch_BOOL getDecompressPlugin(const hpatch_compressedDiffInfo* diffInfo,
                                       hpatch_TDecompress** out_decompressPlugin)
{
  hpatch_TDecompress*  decompressPlugin=0;
  if (strlen(diffInfo->compressType)>0){
    if ((!decompressPlugin)&&lzmaDecompressPlugin.is_can_open(diffInfo->compressType))
        decompressPlugin=&lzmaDecompressPlugin;

    if (!decompressPlugin){
        if (diffInfo->compressedCount>0){
            return hpatch_FALSE; //error
        }else{
            if (strlen(diffInfo->compressType)>0)
                printf("  diffFile added useless compress tag \"%s\"\n",diffInfo->compressType);
            decompressPlugin=0;
        }
    }else{
        printf("hpatchz run with decompress plugin: \"%s\" (need decompress %d)\n",
               diffInfo->compressType,diffInfo->compressedCount);
    }
  }

  *out_decompressPlugin=decompressPlugin;
  return hpatch_TRUE;
}

#define CACHE_SIZE (hpatch_kStreamCacheSize * 4)
int ota_detection()
{
  ota_flag_t ota_flag;
  int32_t ret;
  int32_t patch_ret = UPGRADE_RESULT_SUCC;
  ota_storage_flag_read(&ota_flag);
  hpatch_compressedDiffInfo diff_info;
  hpatch_TDecompress*  decompress_plugin = 0;

  printf("ota.ver = %s\n", ota_flag.ver);
  printf("ota.vercode = %#lx\n", ota_flag.ver_code);
  printf("ota.state = %ld\n", ota_flag.cur_state);
  printf("ota.file size = %ld\n", ota_flag.file_size);
  printf("ota.ret_upgrade = %ld\n", ota_flag.ret_upgrade);
  printf("ota.updater = %lx\n", ota_flag.updater);

  //if state is not EN_OTA_STATUS_UPGRADING, jump to app
  if (ota_flag.cur_state == EN_OTA_STATUS_UPGRADING) {
    uint8_t *cache = (uint8_t *)osal_malloc(CACHE_SIZE);
    if (cache == NULL) {
      printf("no enough mem for ota upgrade!\n");
      ota_update_upgrade_result(&ota_flag, UPGRADE_RESULT_MEMEXHAUSTED);
      return 0;
    }

    ota_storage_bin_read(0, cache, CACHE_SIZE);

/*  tell full upgrade or patch upgrade by diff info, no need this code
    if (get_package_type(cache, ota_flag.file_size) == PACKAGE_TYPE_FULL) {
      printf("upgrage full patch!\n");
//      app_image_backup(PART_APP, PART_OTA_IMG_BACKUP, OTA_IMAGE_BCK_SIZE, cache, CACHE_SIZE);
      app_image_restore(PART_OTA_IMG_DOWNLOAD, PART_APP, \
        ota_flag.file_size, sizeof(ota_binary_info), cache, CACHE_SIZE);
      patch_ret = UPGRADE_RESULT_SUCC;
      goto EXIT;
    }
*/
    // diff upgrade
    // read diff head, get compress type and file size
    printf("upgrage diff patch!\n");
    ret = getCompressedDiffInfo_mem(&diff_info, cache + OTA_BINARY_OFFSET, cache + CACHE_SIZE);
    if(ret != hpatch_TRUE) {
      printf("get diff info failed!\n");
      patch_ret = UPGRADE_RESULT_INNERERROR;
      goto EXIT;
    }
    printf("new img size:%ld, old img size:%ld\n", diff_info.newDataSize, diff_info.oldDataSize);

    if ((diff_info.newDataSize >= OTA_IMAGE_DIFF_UPGRADE_SIZE)
        || (diff_info.oldDataSize >= OTA_IMAGE_BCK_SIZE)) {
        printf("upgrade failed, space is not enough!\n");
        patch_ret = UPGRADE_RESULT_INNERERROR;
        goto EXIT;
    }

    getDecompressPlugin(&diff_info, &decompress_plugin);

    hpatch_TStreamInput download_img_stream;
    hpatch_TStreamInput origin_img_stream;
    hpatch_TStreamOutput upgrade_img_stream;

    downloadimg_as_hStreamInput(&download_img_stream, PART_OTA_IMG_DOWNLOAD, ota_flag.file_size - OTA_BINARY_OFFSET);
    originimg_as_hStreamInput(&origin_img_stream, PART_APP, diff_info.oldDataSize);
    upgradeimg_as_hStreamOutput(&upgrade_img_stream, PART_OTA_DIFF_UPGTADE, diff_info.newDataSize);

    ret = patch_decompress_with_cache(&upgrade_img_stream, &origin_img_stream, &download_img_stream,
						decompress_plugin, cache, cache + CACHE_SIZE);

    if (ret != hpatch_TRUE) {
      printf("patch falied\n");
      patch_ret = UPGRADE_RESULT_FIRMWARE_NOTMATCH;
      goto EXIT;
    }

    //write upgraded img to origin image
    printf("backup the old image!\n");
    ret = app_image_backup(PART_APP, PART_OTA_IMG_BACKUP, diff_info.oldDataSize, cache, CACHE_SIZE);
    if (ret != 0)
    {
        printf("backup the old image falied\n");
        patch_ret = UPGRADE_RESULT_INNERERROR;
        goto EXIT;
    }

    printf("upgrade image!\n");
    ret = app_image_restore(PART_OTA_DIFF_UPGTADE, PART_APP, diff_info.newDataSize, 0, cache, CACHE_SIZE); // restore new
    if (ret == 0)
    {
        goto EXIT;
    }

    // restore origin image
    printf("restore the old image!\n");
    ret = app_image_restore(PART_OTA_IMG_BACKUP, PART_APP, diff_info.oldDataSize, 0, cache, CACHE_SIZE);
    if (ret != 0)
    {
        printf("restore the old falied\n");
        patch_ret = UPGRADE_RESULT_INNERERROR;
    }

EXIT:
    //save upgrade result
    ota_update_upgrade_result(&ota_flag, patch_ret);

    osal_free(cache);
  }
  printf("jumping to app!\n");
  return 0;
}
