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
 *  2020-05-14 09:09  zhangqianfu  The first version
 *
 */
/**
 * @brief: we will do the img upgraded including the diff or full mode
 *         we hope you could do the img binded first
 * */
#include <stdio.h>
#include <osal.h>
#include <ota_img.h>
#include <ota_flag.h>
#include <ota_patch.h>
#include "libHDiffPatch/HPatch/patch.h"
#include "decompress_plugin_demo.h"

#define CONFIG_IMGDOWNLOAD_OFFSET 32
static hpatch_BOOL stream_downloadread(const hpatch_TStreamInput *stream, hpatch_StreamPos_t read_pos,
        uint8_t * data, uint8_t * data_end)
{
    if(ota_img_read((en_ota_type_t)(uintptr_t)stream->streamImport,EN_OTA_IMG_DOWNLOAD,(int) read_pos+CONFIG_IMGDOWNLOAD_OFFSET,data, (int)(uintptr_t) (data_end - data)) == 0)
    {
        return hpatch_TRUE;
    }
    else
    {
        return hpatch_FALSE;
    }
}

static void stream_downloadinit(hpatch_TStreamInput *stream , int size, en_ota_type_t ota_type)
{
    stream->streamSize = (hpatch_StreamPos_t)size;
    stream->read = stream_downloadread;
    stream->streamImport = (void*)(uintptr_t)(ota_type);

}

static hpatch_BOOL stream_backupread(const hpatch_TStreamInput *stream, hpatch_StreamPos_t read_pos,
        uint8_t * data, uint8_t * data_end)
{
    if(ota_img_read((en_ota_type_t)(uintptr_t)stream->streamImport,EN_OTA_IMG_BACKUP,(int) read_pos,data, (int)(uintptr_t) (data_end - data)) == 0)
    {
        return hpatch_TRUE;
    }
    else
    {
        return hpatch_FALSE;
    }
}

static void stream_backupinit(hpatch_TStreamInput *stream, int size,en_ota_type_t ota_type)
{
    stream->streamSize = (hpatch_StreamPos_t)size;
    stream->read = stream_backupread;
    stream->streamImport = (void*)(uintptr_t)(ota_type);
}


static hpatch_BOOL stream_runningwrite(const hpatch_TStreamOutput *stream, hpatch_StreamPos_t write_pos,
        const unsigned char* data, const unsigned char* data_end)
{
    if(ota_img_write((en_ota_type_t)(uintptr_t)stream->streamImport,EN_OTA_IMG_RUNNING,(int) write_pos,(const void *)data, (int)(uintptr_t) (data_end - data)) == 0)
    {
        return hpatch_TRUE;
    }
    else
    {
        return hpatch_FALSE;
    }
}

static hpatch_BOOL stream_runningread(const hpatch_TStreamOutput *stream, hpatch_StreamPos_t read_pos,
        uint8_t * data, uint8_t * data_end)
{
    if(0 == ota_img_read((en_ota_type_t)(uintptr_t)stream->streamImport,EN_OTA_IMG_RUNNING,(int) read_pos,data, (int)(uintptr_t) (data_end - data)))
    {
        return hpatch_TRUE;
    }
    else
    {
        return hpatch_FALSE;
    }
}

static void stream_runninginit(hpatch_TStreamOutput *stream, int size,en_ota_type_t ota_type)
{
    stream->streamSize = (hpatch_StreamPos_t)size;
    stream->read_writed = stream_runningread;
    stream->write = stream_runningwrite;
    stream->streamImport = (void*)(uintptr_t)(ota_type);
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
                    LINK_LOG_DEBUG("  diffFile added useless compress tag \"%s\"\n",diffInfo->compressType);
                decompressPlugin=0;
            }
        }else{
            LINK_LOG_DEBUG("hpatchz run with decompress plugin: \"%s\" (need decompress %d)\n",
            diffInfo->compressType,diffInfo->compressedCount);
        }
    }
    *out_decompressPlugin=decompressPlugin;
    return hpatch_TRUE;
}

#define CN_OTACACHE_SIZE  (hpatch_kStreamCacheSize * 4)
static uint8_t  g_otacache_mem[CN_OTACACHE_SIZE];

en_ota_err_t ota_patch(en_ota_type_t ota_type, int download_filesize)
{
    en_ota_err_t ret = EN_OTA_ERR_UNKNOWN;
    int  img_size;
    hpatch_compressedDiffInfo diff_info;
    hpatch_TDecompress*  decompress_plugin = 0;
    hpatch_TStreamInput  download_img_stream;
    hpatch_TStreamInput  backup_img_stream;
    hpatch_TStreamOutput running_img_stream;

    if(ota_type >= EN_OTA_TYPE_LAST)
    {
        ret = EN_OTA_ERR_PARAS;
        goto EXIT;
    }

    if (0 != ota_img_read(ota_type,EN_OTA_IMG_DOWNLOAD,CONFIG_IMGDOWNLOAD_OFFSET,g_otacache_mem,CN_OTACACHE_SIZE))
    {
        LINK_LOG_ERROR("READ DOWNLOADING ERROR");
        ret = EN_OTA_ERR_NODOWNLOADIMG;
        goto EXIT;
    }

    if(hpatch_TRUE != getCompressedDiffInfo_mem(&diff_info, g_otacache_mem , g_otacache_mem + CN_OTACACHE_SIZE))
    {
        LINK_LOG_ERROR("DOWNLOADING INFORMATION ERROR");
        ret = EN_OTA_ERR_PATCHING;
        goto EXIT;
    }

    LINK_LOG_DEBUG("new img size:%d, old img size:%d\n", (int)diff_info.newDataSize, (int)diff_info.oldDataSize);
    if(hpatch_TRUE != getDecompressPlugin(&diff_info, &decompress_plugin))
    {
        LINK_LOG_ERROR("GET DECOMPRESSPLUGINS ERROR");
        ret = EN_OTA_ERR_DECOMPRESSMETHOD;
        goto EXIT;
    }
    ///< check if the data size matched
    if((0 != ota_img_size(ota_type, EN_OTA_IMG_BACKUP,&img_size)) || ((int)diff_info.oldDataSize >  img_size))
    {
        LINK_LOG_ERROR("BACKUPSIZE TOO SMALL");
        ret = EN_OTA_ERR_BACKUPIMGSIZE;
        goto EXIT;
    }

    if((0 != ota_img_size(ota_type, EN_OTA_IMG_DOWNLOAD,&img_size)) || ((int)diff_info.newDataSize >  img_size))
    {
        LINK_LOG_ERROR("DOWNLOAD IMG MAYBE NOT COMPLETE");
        ret = EN_OTA_ERR_DOWNLOADIMGSIZE;
        goto EXIT;
    }

    if((0 != ota_img_size(ota_type, EN_OTA_IMG_RUNNING,&img_size)) || ((int)diff_info.newDataSize >  img_size))
    {
        LINK_LOG_ERROR("RUNNING IMG TOO SMALL");
        ret = EN_OTA_ERR_RUNNINGIMGSIZE;
        goto EXIT;
    }

    ///< now we do the running erase
    if ( 0 != ota_img_erase(ota_type, EN_OTA_IMG_RUNNING))
    {
        LINK_LOG_ERROR("ERASE RUNNING FAILED");
        ret = EN_OTA_ERR_RUNNINGERASE;
        goto EXIT;
    }
    ///< now do th stream initialize
    stream_runninginit(&running_img_stream,(int)diff_info.newDataSize, ota_type);
    stream_backupinit(&backup_img_stream,(int)diff_info.oldDataSize, ota_type);
    stream_downloadinit(&download_img_stream,download_filesize -CONFIG_IMGDOWNLOAD_OFFSET, ota_type);

    if (patch_decompress_with_cache(&running_img_stream, &backup_img_stream, &download_img_stream,
            decompress_plugin,  g_otacache_mem , g_otacache_mem + CN_OTACACHE_SIZE) != hpatch_TRUE)
    {
        LINK_LOG_ERROR("PATCHING PROCESS FAILED");
        ret = EN_OTA_ERR_PATCHING;
        goto EXIT;
    }
    ret = EN_OTA_SUCCESS;

EXIT:
    return ret;
}

///< which you could use this function to do a img backUP:move the running to the backup
en_ota_err_t ota_backup(en_ota_type_t  ota_type)
{
    en_ota_err_t ret = EN_OTA_ERR_UNKNOWN;
    int size_left;
    int movelen;
    int offset = 0;

    if(ota_type >= EN_OTA_TYPE_LAST)
    {
        ret = EN_OTA_ERR_PARAS;
        goto EXIT;
    }

    if(0 != ota_img_size(ota_type, EN_OTA_IMG_RUNNING, &size_left))
    {
        ret = EN_OTA_ERR_NORUNNINGIMG;
        goto EXIT;
    }

    if(0 != ota_img_erase(ota_type, EN_OTA_IMG_BACKUP))
    {
        ret = EN_OTA_ERR_RUNNINGERASE;
        goto EXIT;
    }

    while(size_left > 0)
    {
        movelen = size_left > CN_OTACACHE_SIZE ?CN_OTACACHE_SIZE:size_left;
        if(ota_img_read(ota_type,EN_OTA_IMG_RUNNING,offset,g_otacache_mem, movelen) != 0)
        {
            LINK_LOG_ERROR("READ RUNNING:offset:%d len:%d FAILED",offset, movelen);
            ret = EN_OTA_ERR_RUNNNINGREAD;
            goto EXIT;
        }
        if(ota_img_write(ota_type,EN_OTA_IMG_BACKUP,offset,g_otacache_mem, movelen) != 0)
        {
            LINK_LOG_ERROR("WRITE BACKUP:offset:%d len:%d FAILED",offset,movelen);
            ret = EN_OTA_ERR_BACKUPWRITE;
            goto EXIT;
        }
        offset += movelen;
        size_left -= movelen;
    }

    if( 0 != ota_img_flush(ota_type,EN_OTA_IMG_BACKUP))
    {
        ret = EN_OTA_ERR_BACKUPFLUSH;
        goto EXIT;
    }
    ret = EN_OTA_SUCCESS;
EXIT:
    return ret;
}

///< which you could use this fucntion to do a img recover
en_ota_err_t ota_recover(en_ota_type_t  ota_type)
{
    en_ota_err_t ret = EN_OTA_ERR_UNKNOWN;
    int size_left;
    int movelen;
    int offset = 0;

    if(ota_type >= EN_OTA_TYPE_LAST)
    {
        ret = EN_OTA_ERR_PARAS;
        goto EXIT;
    }

    if(0 != ota_img_size(ota_type, EN_OTA_IMG_RUNNING, &size_left))
    {
        ret = EN_OTA_ERR_NORUNNINGIMG;
        goto EXIT;
    }

    if(0 != ota_img_erase(ota_type, EN_OTA_IMG_RUNNING))
    {
        ret = EN_OTA_ERR_RUNNINGERASE;
        goto EXIT;
    }

    while(size_left > 0)
    {
        movelen = size_left > CN_OTACACHE_SIZE ?CN_OTACACHE_SIZE:size_left;
        if(ota_img_read(ota_type,EN_OTA_IMG_BACKUP,offset,g_otacache_mem, movelen) != 0)
        {
            LINK_LOG_ERROR("READ BACKUP:offset:%d len:%d FAILED",offset, movelen);
            ret = EN_OTA_ERR_BACKUPREAD;
            goto EXIT;
        }
        if(ota_img_write(ota_type,EN_OTA_IMG_RUNNING,offset,g_otacache_mem, movelen) != 0)
        {
            LINK_LOG_ERROR("WRITE RUNNING:offset:%d len:%d FAILED",offset,movelen);
            ret = EN_OTA_ERR_RUNNINGWRITE;
            goto EXIT;
        }
        offset += movelen;
        size_left -= movelen;
    }

    if( 0 != ota_img_flush(ota_type,EN_OTA_IMG_RUNNING))
    {
        ret = EN_OTA_ERR_RUNNINGFLUSH;
        goto EXIT;
    }
    ret = EN_OTA_SUCCESS;
EXIT:
    return ret;
}


en_ota_err_t ota_commonflow(en_ota_type_t ota_type)
{
    en_ota_err_t  ota_ret = EN_OTA_ERR_UNKNOWN;
    en_ota_upgrade_step_t  upgrade_step;
    ota_flag_t ota_flag;

    if(ota_type >= EN_OTA_TYPE_LAST)
    {
        ota_ret = EN_OTA_ERR_PARAS;
        goto FLOW_EXIT;
    }

    if(0 != ota_flag_get(ota_type,&ota_flag))
    {
        LINK_LOG_ERROR("GET FLAG ERR AND SHOULD DO THE INITIALIZE");
        memset(&ota_flag,0,sizeof(ota_flag));
        ota_flag.info.curstatus = EN_OTA_STATUS_IDLE;
        ota_flag.info.upgrade_step = EN_OTA_UPGRADE_STEP_INIT;
        (void)ota_flag_save(ota_type,&ota_flag);
        ota_ret = EN_OTA_ERR_FLAG_GET;
        goto FLOW_EXIT;
    }
    ///< first check if we should do the upgrade
    if(ota_flag.info.curstatus != EN_OTA_STATUS_DOWNLOADED)
    {
        LINK_LOG_DEBUG("DO NORMAL BOOT.");
        ota_ret = EN_OTA_SUCCESS;
        goto FLOW_EXIT;
    }
    LINK_LOG_DEBUG("NOW OTA SEQUENCE START");

    ///< DO THE BACKUP IF NEEDED
    upgrade_step = ota_flag.info.upgrade_step;
    if((upgrade_step == EN_OTA_UPGRADE_STEP_START) || (upgrade_step == EN_OTA_UPGRADE_STEP_BACKUP))
    {
         LINK_LOG_DEBUG("BACKUP START");
         if(EN_OTA_SUCCESS != ota_backup(ota_type))
         {
             LINK_LOG_ERROR("BACKUP FAILED");
             upgrade_step = EN_OTA_UPGRADE_STEP_DONE;
             ota_flag.info.curstatus = EN_OTA_STATUS_UPGRADED_FAILED;
             ota_flag.info.upgrade_step = upgrade_step;
             (void)ota_flag_save(ota_type,&ota_flag);
             ota_ret = EN_OTA_ERR_BACKUP;
             goto FLOW_EXIT;
         }
         upgrade_step = EN_OTA_UPGRADE_STEP_PATCH;
         ota_flag.info.upgrade_step = upgrade_step;
         (void)ota_flag_save(ota_type,&ota_flag);
         LINK_LOG_DEBUG("BACKUP SUCCESS");
    }

    ///< DO THE PATCH IF NEEDED
    if(upgrade_step == EN_OTA_UPGRADE_STEP_PATCH)
    {
        LINK_LOG_DEBUG("PATCH START");
        if(EN_OTA_SUCCESS != ota_patch(ota_type,ota_flag.info.img_download.file_size))
        {
            upgrade_step = EN_OTA_UPGRADE_STEP_RECOVER;
            ota_flag.info.upgrade_step = upgrade_step;
            (void)ota_flag_save(ota_type,&ota_flag);
            LINK_LOG_ERROR("PATCH FAILED");
            ota_ret = EN_OTA_ERR_PATCH;
        }
        else
        {
            upgrade_step = EN_OTA_UPGRADE_STEP_DONE;
            ota_flag.info.upgrade_step = upgrade_step;
            ota_flag.info.curstatus = EN_OTA_STATUS_UPGRADED_SUCCESS;
            (void)ota_flag_save(ota_type,&ota_flag);
            LINK_LOG_DEBUG("PATCH SUCCESS");
            ota_ret = EN_OTA_SUCCESS;
            goto FLOW_EXIT;
        }
    }

    ///< DO THE RECOVER
    if(upgrade_step == EN_OTA_UPGRADE_STEP_RECOVER)
    {
        LINK_LOG_DEBUG("RECOVER START");
        if(EN_OTA_SUCCESS != ota_recover(ota_type) )
        {
            LINK_LOG_ERROR("RECOVER FAILED");
            ota_ret = EN_OTA_ERR_RECOVER;
        }
        else
        {
            LINK_LOG_DEBUG("RECOVER SUCCESS");
        }
        ///< FOR THE RECOVER, THIS IS THE LAST STATUS, AND WE COULD MAKE MORE METHOD TO SAVE THE MACHINE
        upgrade_step = EN_OTA_UPGRADE_STEP_DONE;
        ota_flag.info.upgrade_step = upgrade_step;
        ota_flag.info.curstatus = EN_OTA_STATUS_UPGRADED_FAILED;
        (void)ota_flag_save(ota_type,&ota_flag);
        goto FLOW_EXIT;
    }

FLOW_EXIT:
    return ota_ret;  ///< maybe never come back
}





