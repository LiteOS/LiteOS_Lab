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
 *  DATE                      INSTRUCTION
 *  2020-07-21 18:43   The first version
 *
 */

#include <oc_mqtt_event.h>
#include <oc_mqtt_profile.h>

/**
Topic: $oc/devices/{device_id}/sys/events/up
数据格式：
{
    "object_device_id": "{object_device_id}",
    "services": [{
        "service_id": "$ota",
        "event_type": "version_report",
        "event_time": "20151212T121212Z",
        "paras": {
            "sw_version": "v1.0",
            "fw_version": "v1.0"
        }
    }]
}
*/
///< you could use this function to report the software or firmware version
int oc_mqtt_report_version(const char *deviceID,const char *objectDeviceID, const char *softVersion, const char *firmVersion)
{
    int ret = -1;
    oc_mqtt_profile_kv_t    firmVer;
    oc_mqtt_profile_kv_t    softVer;
    oc_mqtt_profile_kv_t *cur = NULL;
    oc_mqtt_profile_event_t event;

    if(softVersion != NULL){
        softVer.key = CN_OC_JSON_KEY_SWVERSION;
        softVer.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
        softVer.value = (void *)softVersion;
        softVer.nxt = cur;
        cur = &softVer;
    }

    if(firmVersion != NULL){
        firmVer.key =  CN_OC_JSON_KEY_FWVERSION;
        firmVer.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
        firmVer.value = (void *)firmVersion;
        firmVer.nxt = cur;
        cur = &firmVer;
    }

    event.object_device_id = (char *)objectDeviceID;
    event.event_time = NULL;
    event.event_type = CN_OC_MQTT_EVENTTYPE_VERSIONREPORT;
    event.service_id = "$ota";
    event.paras = cur;

    ret = oc_mqtt_profile_reportevent((char *)deviceID, &event);

    return ret;
}



///< data format
//Topic: $oc/devices/{device_id}/sys/events/up
//{
//    "object_device_id": "{object_device_id}",
//    "services": [{
//        "service_id": "$ota",
//        "event_type": "upgrade_progress_report",
//        "event_time": "20151212T121212Z",
//        "paras": {
//            "result_code": 0,
//            "progress": 80,
//            "version": "V2.0",
//            "description": "upgrade processing"
//        }
//    }]
//}
int oc_mqtt_report_upgradeprogress(const char *deviceID, const char *objectDeviceID, int upgraderet, const char *version, int progress)
{
    int ret = -1;
    oc_mqtt_profile_kv_t    prog;
    oc_mqtt_profile_kv_t    resultCode;
    oc_mqtt_profile_kv_t    ver;
    oc_mqtt_profile_kv_t   *cur = NULL;
    oc_mqtt_profile_event_t event;

    resultCode.key = CN_OC_JSON_KEY_RESULTCODE;
    resultCode.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    resultCode.value = &upgraderet;
    resultCode.nxt = cur;
    cur = &resultCode;

    if(version != NULL){
        ver.key =  CN_OC_JSON_KEY_VERSION;
        ver.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
        ver.value =(void *) version;
        ver.nxt = cur;
        cur = &ver;
    }

    if((progress >= 0)&& (progress <= 100)){
        prog.key =  CN_OC_JSON_KEY_PROGRESS;
        prog.type = EN_OC_MQTT_PROFILE_VALUE_INT;
        prog.value = &progress;
        prog.nxt = cur;
        cur = &prog;
    }

    event.object_device_id = (char *)objectDeviceID;
    event.event_time = NULL;
    event.event_type = CN_OC_MQTT_EVENTTYPE_UPGRADEPROGRESSREPORT;
    event.service_id = "$ota";
    event.paras = cur;

    ret = oc_mqtt_profile_reportevent((char *)deviceID, &event);

    return ret;
}

