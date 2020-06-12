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
 *  2020-05-08 20:54  zhangqianfu  The first version
 *
 */

#include <string.h>
#include <crc.h>
#include <link_log.h>
#include <ota_flag.h>

///< attention that when we write the flag, it maybe powered down when erase or write
///< so we use need a backup img，supposed that the flash has no problem when read or write except unexpected powered down
/**
 * ACTION:
 * 1,WHEN WRITE, CHECK THE DATA IN THE BACKUP IS WELL PRESERVED OR NOT;
 *   IF WELL PRESERVED, THEN WRITE DATA TO MASTER FIRST, ELSE WRITE TO THE BACKUP ONE FIRST;
 *   AND THIS ACTION MAKE ANYTIME WE COULD FIND A GOOD IMG, MASTER ONE OR BACKUP ONE;AND WE COULD
 *   NOT AVOID THE CASE THAT WE LOSE THE DATA WHEN WRITTING; MAYBE NEXT TIME WE WILL DO THE LOG FILEFORMAT
 * 2,WHEN READ, READ THE MASTER ONE FIRST, IF MASTER IS OK THEN SYN IT TO THE BACKUP ONE; IF MASTER IS DESTROYED, TRY
 *   TO READ FROM THE BACKUP ONE, AND SYNC IT TO THE MASTER
 *
 * Then we could think some cases when unexpected power down comes: supposed the img we write will be destroyed when powered down
 * a, if comes at the time we write the master, then we are sure that the backup img is well preserved
 * b, if comes at the time we write the backup, then we are sure that the backup img is well preserved
 *
 * Anyway, we could not backup the data if both the img is destroyed! So when you read the flag failed,
 * you should do the flag initialize first!
 *
 * */

static int ota_flag_imgupdate(en_ota_type_t  otatype,en_ota_img_type_t img, ota_flag_t *flag)
{
    int ret;
    ret = ota_img_erase(otatype,EN_OTA_IMG_FLAG);
    if(ret != 0)
    {
        goto EXIT_UPDATE;
    }
    ret = ota_img_write(otatype,EN_OTA_IMG_FLAG,0,flag,sizeof(ota_flag_t));
    if(ret != 0)
    {
        goto EXIT_UPDATE;
    }
    ret = ota_img_flush(otatype,EN_OTA_IMG_FLAG);
    if(ret != 0)
    {
        goto EXIT_UPDATE;
    }
EXIT_UPDATE:
    return ret;
}



int ota_flag_save(en_ota_type_t  otatype, ota_flag_t *flag)
{
    int ret = -1;
    uint32_t    crc;
    ota_flag_t  flag_backup;

    if((otatype  >= EN_OTA_TYPE_LAST) || (NULL == flag))
    {
        return ret;
    }
    flag->crc = calc_crc32(0,&flag->info, sizeof(flag->info));
    ///< check if the backup one is ok or not
    ret = ota_img_read(otatype,EN_OTA_IMG_FLAGBACKUP,0,&flag_backup,sizeof(ota_flag_t));
    if(ret != 0)
    {
        LINK_LOG_ERROR("FLAGBACKUP READ ERROR");
        goto EXIT_SAVE;
    }
    crc = calc_crc32(0,&flag_backup.info, sizeof(flag_backup.info));
    if(crc != flag_backup.crc) ///< the backup one is not preserved
    {
        ret = ota_flag_imgupdate(otatype,EN_OTA_IMG_FLAGBACKUP,flag);
        if(ret != 0)
        {
            LINK_LOG_ERROR("FLAGBACKUP UPDATE ERROR");
            goto EXIT_SAVE;
        }
    }
    ret = ota_flag_imgupdate(otatype,EN_OTA_IMG_FLAG,flag);
    if(ret != 0)
    {
        LINK_LOG_ERROR("FLAGMASTER UPDATE ERROR");
        goto EXIT_SAVE;
    }
    ///< AND AT THE SAME TIME WE WILL DO WRITE THE DATA TO THE BACKUP IMG,
    if(crc == flag_backup.crc)
    {
        ret = ota_flag_imgupdate(otatype,EN_OTA_IMG_FLAGBACKUP,flag);
        if(ret != 0)
        {
            LINK_LOG_ERROR("FLAGBACKUP UPDATE ERROR");
            goto EXIT_SAVE;
        }
    }
EXIT_SAVE:
    return ret;
}

int ota_flag_get(en_ota_type_t  otatype,ota_flag_t *flag)
{
    int ret = -1;
    uint32_t    crc;
    ota_flag_t  flag_backup;
    if((NULL == flag) || (otatype >= EN_OTA_TYPE_LAST))
    {
        LINK_LOG_ERROR("PARAMETERS ERROR");
        goto EXIT_GET;
    }
    ///< read the master one
    ret = ota_img_read(otatype,EN_OTA_IMG_FLAG,0,flag,sizeof(ota_flag_t));
    if(ret != 0)
    {
        LINK_LOG_ERROR("FLAGMASTER READ ERROR");
        goto EXIT_GET;
    }
    crc = calc_crc32(0,&flag->info, sizeof(flag->info));
    if(crc == flag->crc)  ///< READ MASTER SUCCESS; and we should check if the
    {
        ///< CHECK THE BACKUP ONE, WE DON'T CARE READ SUCCESS OR NOT, WE JUST WANT TO DO THE SYNC
       (void) ota_img_read(otatype,EN_OTA_IMG_FLAGBACKUP,0,&flag_backup,sizeof(ota_flag_t));
       if(0 != memcmp(flag,&flag_backup,sizeof(ota_flag_t)))
       {
           (void) ota_flag_imgupdate(otatype,EN_OTA_IMG_FLAGBACKUP,flag);
       }
       goto EXIT_GET;
    }
    LINK_LOG_ERROR("MASTER FLAG DESTROYED");
    ///< the master one is not ok, so try to read the backup one
    ret = ota_img_read(otatype,EN_OTA_IMG_FLAGBACKUP,0,flag,sizeof(ota_flag_t));
    if(ret != 0)
    {
        LINK_LOG_ERROR(" FLAGBACKUP READ ERROR");
        goto EXIT_GET;
    }
    crc = calc_crc32(0,&flag->info, sizeof(flag->info));
    if(crc != flag->crc)   ///< back up one is also destroyed
    {
        LINK_LOG_ERROR(" FLAGBACKUP DESTRORED");
        ret = -1;
        goto EXIT_GET;
    }
    LINK_LOG_DEBUG("FLAGBACKUP SUCCESS");
    ///< WE WRITE THE DATA TO THE MASTER IMG
    (void) ota_flag_imgupdate(otatype,EN_OTA_IMG_FLAG,flag);

EXIT_GET:
    return ret;
}





