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
#include <osport.h>


#include <app_main.h>

#include "sys_init.h"

//do the light intensity report
static s32_t beep_switch(u8_t *buf, s32_t buflen)
{
    if(strcmp((char *)buf,"ON")==0)                                 //开蜂鸣器
    {	
        HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,GPIO_PIN_SET);    // 输出高电平
    }
    if(strcmp((char *)buf,"OFF")==0)                                //关蜂鸣器
    {	
        HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,GPIO_PIN_RESET);  // 输出低电平
    }
    return buflen;
}

bool_t app_beep_switch()
{
    bool_t ret = true;
        
    ret = app_register("appbeep",en_app_direction_command,en_app_msgid_beep,\
          beep_switch,2);

    return ret;
}

#include <shell.h>

static s32_t shell_beepcmd(s32_t argc, const char *argv[])
{
    if(strcmp((char *)argv[1],"on")==0)                                 //开蜂鸣器
    {	
        HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,GPIO_PIN_SET);        // 输出高电平
    }
    if(strcmp((char *)argv[1],"off")==0)                                //关蜂鸣器
    {	
        HAL_GPIO_WritePin(Beep_GPIO_Port,Beep_Pin,GPIO_PIN_RESET);      // 输出低电平
    }

    return 0;
}
OSSHELL_EXPORT_CMD(shell_beepcmd,"beepcmd","beepcmd on/off");









