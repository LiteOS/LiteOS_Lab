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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "demo.h"

app_cb_t g_app_cb;
// use this function to push all the message to the buffer
int msg_rcv_callback(oc_mqtt_profile_msgrcv_t* msg)
{
    int ret = 0;
    char* buf;
    int buf_len;
    app_msg_t* app_msg;

    if ((msg == NULL) || (msg->request_id == NULL)) {
        return ret;
    }

    buf_len = sizeof(app_msg_t) + strlen(msg->request_id) + 1 + msg->msg_len + 1;
    buf = malloc(buf_len);
    if (buf == NULL) {
        return ret;
    }
    app_msg = (app_msg_t*)buf;
    buf += sizeof(app_msg_t);

    app_msg->msg_type = msg->type;
    app_msg->msg.cmd.request_id = buf;
    buf_len = strlen(msg->request_id);
    buf += buf_len + 1;
    memcpy_s(app_msg->msg.cmd.request_id, buf_len, msg->request_id, buf_len);
    app_msg->msg.cmd.request_id[buf_len] = '\0';

    buf_len = msg->msg_len;
    app_msg->msg.cmd.payload = buf;
    memcpy_s(app_msg->msg.cmd.payload, buf_len, msg->msg, buf_len);
    app_msg->msg.cmd.payload[buf_len] = '\0';

    ret = queue_push(g_app_cb.app_msg, app_msg, 10);
    if (ret != 0) {
        free(app_msg);
    }

    return ret;
}

// response to the command from IoTDA
void oc_cmdresp(cmd_t* cmd, int cmdret)
{
    oc_mqtt_profile_cmdresp_t cmdresp;
    // do the response
    cmdresp.paras = NULL;
    cmdresp.request_id = cmd->request_id;
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = NULL;
    (void)oc_mqtt_profile_cmdresp(NULL, &cmdresp);
}

// response to the propertyset from IoTDA
void oc_propertysetresp(cmd_t* cmd, int propertyset_ret)
{
    oc_mqtt_profile_propertysetresp_t propertysetresp;
    propertysetresp.ret_code = propertyset_ret;
    propertysetresp.request_id = cmd->request_id;
    propertysetresp.ret_description = propertyset_ret == 0 ? "success" : "failed";
    (void)oc_mqtt_profile_propertysetresp(NULL, &propertysetresp);
}

void ReportSDKInfo(void)
{
    printf("hello xingli\n");
    oc_mqtt_profile_event_t service;
    oc_mqtt_profile_kv_t sdk;
    oc_mqtt_profile_kv_t sw;
    oc_mqtt_profile_kv_t fw;
    oc_mqtt_profile_kv_t device_ip;

    service.event_time = NULL;
    service.service_id = "$sdk_info";
    service.event_type = "sdk_info_report";
    service.paras = &sdk;

    sdk.key = "device_sdk_version";
    sdk.value = "C_v2.2.2";
    sdk.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    sdk.nxt = &sw;

    sw.key = "sw_version";
    sw.value = "v1.0";
    sw.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    sw.nxt = &fw;

    fw.key = "fw_version";
    fw.value = "v1.0";
    fw.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    fw.nxt = &device_ip;

    device_ip.key = "device_ip";
    device_ip.value = "127.0.0.1";
    device_ip.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    device_ip.nxt = NULL;

    oc_mqtt_profile_reportevent(NULL, &service);
    return;
}