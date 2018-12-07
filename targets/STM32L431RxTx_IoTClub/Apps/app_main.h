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
  
#ifndef APP_MAIN_H
#define APP_MAIN_H

#include <osport.h>

typedef enum
{
    en_app_msgid_start =0,
	en_app_msgid_intensity = en_app_msgid_start,
    en_app_msgid_ledctrl,
	en_app_msgid_netcsq,
    en_app_msgid_dht11,
    en_app_msgid_smoke,
    en_app_msgid_beep,
    en_app_msgid_gps,
//the following for the test
    en_app_msgid_cmdresponse,
    en_app_msgid_respcode,
	en_app_msgid_lightstate,
	en_app_msgid_last    //the last one
}en_app_msgid;

typedef enum
{
    en_app_direction_report = 0,   //if you register function with report, then app main will
                                   //call this function to get data and report it when timeout
    
    en_app_direction_command       //if you regitser function with command, then app main will 
                                   //call this function to deal the data received with the same 
                                   //message id

}en_app_msg_direction;


//if used for reporter, then will report how many data has beed get to report
//if used for dealer, then will tell you the message to dealt
//message id has been dealt
typedef s32_t (*app_msg_function) (u8_t *buf,s32_t buflen);   

#define cn_app_report_cyle      (1000)   //unit:ms, and the value for 1 second
//cycle, only could be used when report, unit:cn_app_report_cyle
//this function could be called before appsmain_init
bool_t app_register(const char *appname,en_app_msg_direction direction,en_app_msgid msgid, app_msg_function func, s32_t cycle); 

//this function must be called by manual or by shell command
bool_t app_main_init(const char *server,u16_t port,u16_t band);

//this function used for the raw send to the nb
bool_t app_send_raw(u8_t *msg, s32_t len);


#endif


