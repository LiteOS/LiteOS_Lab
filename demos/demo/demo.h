/*----------------------------------------------------------------------------
 * Copyright (c) <2022>, <Huawei Technologies Co., Ltd>
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


#ifndef DEMO_H
#define DEMO_H
#include "queue.h"
#include "oc_mqtt_profile.h"

typedef enum {
    en_msg_report = 8,          // message type : report, from 8, because 0-7 is the type that IoTDA down
    en_msg_conn,                // message type : connect
    en_msg_disconn,             // message type : disconnect
    en_msg_shadow_desired,      // message type : shadow, you can add other type
} en_msg_type_t;                

typedef struct {
    char* request_id;
    char* payload;
} cmd_t;

typedef struct {
    int lum;
    int temp;
    int hum;
    int memory_check;           // the switch of memory check
    int memory_threshold;       // the threshold of memory used
    int port_check;             // the switch of port check
} report_t;                     // you can add the property that you want to report

typedef struct {
    en_msg_type_t msg_type;
    union {
        cmd_t cmd;
        report_t report;
    } msg;
} app_msg_t;

typedef struct {
    queue_t *app_msg;
    int connected;              
    int led;
    int motor;
    int threshold;
} app_cb_t;
 
/**
 * @brief use this function to push all the message to the buffer
 * 
 * @param msg message
 * @return int 
 */
int msg_rcv_callback(oc_mqtt_profile_msgrcv_t* msg);

/**
 * @brief response to the command from IoTDA
 * 
 * @param cmd command
 * @param cmdret the reponse value
 */
void oc_cmdresp(cmd_t* cmd, int cmdret);


/**
 * @brief response to the propertyset from IoTDA
 * 
 * @param cmd command
 * @param propertyset_ret the reponse value
 */
void oc_propertysetresp(cmd_t* cmd, int propertyset_ret);


/**
 * @brief report the version of sdk
 * 
 */
void ReportSDKInfo(void);

#endif