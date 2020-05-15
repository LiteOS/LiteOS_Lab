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
 *  2020-05-14 17:07  zhangqianfu  The first version
 *
 */
#include <string.h>
#include <stdlib.h>
#include <link_log.h>
#include <ota_patch.h>
#include <ota_flag.h>

#ifndef CONFIG_APP_OTATYPE
#define CONFIG_APP_OTATYPE EN_OTA_TYPE_FOTA
#endif

#ifndef CONFIG_OTA_APPADDR
#define CONFIG_OTA_APPADDR 0x08020004
#endif

static int jump_addr(void *addr)
{
    unsigned int start_cmd;

    int (*jump_func)(void );
    start_cmd = *(unsigned int *)(addr);
    jump_func = (int (*)(void))(start_cmd);
    jump_func();
    return 0;  ///< maybe never come back
}


int loader_main(void)
{
    ota_flag_t ota_flag;
    if(0 != ota_flag_get(CONFIG_APP_OTATYPE,&ota_flag))
    {
        LINK_LOG_DEBUG("No OTA FLAG");
        goto EXIT;
    }
    ///< first check if we should do the upgrade
    LINK_LOG_DEBUG("OTASTATUS:%d",(int)ota_flag.info.curstatus);
    if(ota_flag.info.curstatus != EN_OTA_STATUS_DOWNLOADED)
    {
        LINK_LOG_DEBUG("No OTA need to do");
        goto EXIT;
    }
    ///< if we need do the upgrade, then do  the backup
    if(EN_OTA_SUCCESS != ota_backup(CONFIG_APP_OTATYPE))
    {
        LINK_LOG_DEBUG("BACKUP failed");
    }
    else
    {
        LINK_LOG_DEBUG("BACKUP success");
    }

    ///< if we backup success, then we do the upgrade
    if(EN_OTA_SUCCESS != ota_patch(CONFIG_APP_OTATYPE,ota_flag.info.img_download.file_size))
    {
        LINK_LOG_DEBUG("PATCH FAILED AND DO THE RECOVER");
        ota_flag.info.curstatus = EN_OTA_STATUS_UPGRADED_FAILED;
        (void)ota_flag_save(CONFIG_APP_OTATYPE,&ota_flag);
        (void)ota_recover(CONFIG_APP_OTATYPE);
        goto EXIT;
    }
    LINK_LOG_DEBUG("PATCH SUCCESS");
    ota_flag.info.curstatus = EN_OTA_STATUS_UPGRADED_SUCCESS;
    (void)ota_flag_save(CONFIG_APP_OTATYPE,&ota_flag);

EXIT:
    LINK_LOG_DEBUG("BEGIN TO JUMP TO APPLICATION:%08X",CONFIG_OTA_APPADDR);
    jump_addr((void *)CONFIG_OTA_APPADDR);
    return 0;  ///< maybe never come back
}

static int loader_shell(int argc, const char *argv[])
{
    void *addr = (void *)CONFIG_OTA_APPADDR;
    if(argc == 2)
    {
        addr = (void *)strtol(argv[1],NULL,0);
    }
    jump_addr(addr);
    return 0;///< never comeback
}


#if CONFIG_LITEOS_ENABLE

#include <shell.h>
OSSHELL_EXPORT_CMD(loader_shell,"loader","loader addr");
#endif

#if CONFIG_NOVAOS_ENABLE

#include <cmder.h>
static int nova_command( cmder_t *cer,int argc,char *argv[])
{

    return ota_update(argc, (const char **)argv);
}

CMDER_CMD_DEF ("loader","loader addr",nova_command);
#endif


int standard_app_demo_main(void)
{
    LINK_LOG_DEBUG("%s:This is the loader main function\n\r",__FUNCTION__);
    extern int loader_main(void);
    loader_main();
    return 0;
}



