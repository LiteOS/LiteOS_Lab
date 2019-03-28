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

#include <app_main.h>


#include <led.h>   //for led driver


//this function used to deal all the received message here
//message format:content
//
static s32_t  __app_cmd_response(u8_t *msg, s32_t msglen)
{
    s32_t i =0;
    u8_t resp[5];
    const char *result = "YN"; 
    static u8_t code_execute = 0;
    const u8_t *payload;
   
    payload = msg;
    printf("cmdresponse:");
    for (i =0;i< msglen;i++)
    {
        printf("0x%02x ",*payload++);
    }
    printf("\n\r");
    
    payload = msg;
    code_execute ++;
    code_execute = code_execute%2;
    //fill the messageid
    resp[0] =  en_app_msgid_respcode;
    //execute code
    resp[1] = code_execute;
    //mid
    resp[2] = *payload++;
    resp[3] = *payload++;
    
    //result message
    resp[4] = (u8_t)result[code_execute];
     
    app_send_raw(resp,sizeof(resp));

    return msglen;
}

bool_t app_cmd_response()
{
    bool_t ret;
    ret = app_register("appresponse",en_app_direction_command,en_app_msgid_cmdresponse,__app_cmd_response,0);

    return ret;
}

