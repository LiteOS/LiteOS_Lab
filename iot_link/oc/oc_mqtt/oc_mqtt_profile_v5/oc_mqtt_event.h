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
 *  DATE                    INSTRUCTION
 *  2020-07-21 18:43  The first version
 *
 */
#ifndef LITEOS_LAB_IOT_LINK_OC_OC_MQTT_OC_MQTT_PROFILE_V5_OC_MQTT_EVENT_H_
#define LITEOS_LAB_IOT_LINK_OC_OC_MQTT_OC_MQTT_PROFILE_V5_OC_MQTT_EVENT_H_

/**
 * @brief:the user could use this function to report the version to the iot platform
 *
 * @param: deiceID, the device id
 *
 * @param: objectDeviceID, the information sent to ,could be NULL
 *
 *
 * @param: softVersion, the software version of the device, could be NULL
 *
 * @param:firmVersion, the firmware version of the device, could be NULL
 *
 * @return:0 success while others failed
 *
 * */
int oc_mqtt_report_version(const char *deviceID, const char *objectDeviceID,\
                              const char *softVersion, const char *firmVersion);


typedef enum{

  EN_OC_MQTT_UPGRADERET_SUCCESS = 0,
  EN_OC_MQTT_UPGRADERET_BUSY,
  EN_OC_MQTT_UPGRADERET_WEAKSIGNAL,
  EN_OC_MQTT_UPGRADERET_SAMEVERSION,
  EN_OC_MQTT_UPGRADERET_POWERLESS,
  EN_OC_MQTT_UPGRADERET_SPACELESS,
  EN_OC_MQTT_UPGRADERET_DOWNLOADTIMEOUT,
  EN_OC_MQTT_UPGRADERET_PACKCHECKERR,
  EN_OC_MQTT_UPGRADERET_PACKTYPENOTSUPPORT,
  EN_OC_MQTT_UPGRADERET_MEMLESS,
  EN_OC_MQTT_UPGRADERET_UPGRADEERR,
  EN_OC_MQTT_UPGRADERET_INTERNALERR = 255,
}en_oc_mqtt_upgraderet_t;

/**
 * @brief:the user could use this function to report the upgrade result
 *
 * @param: deiceID, the device id
 *
 *  @param: objectDeviceID, the information sent to ,could be NULL
 *
 * @param: upgraderet:defined as en_oc_mqtt_upgraderet_t
 *
 * @param: version, the current version of the device, maybe software of hardware
 *
 * @param: progress, the upgrade progress, 0-100, and -1 means not report the option
 *
 * @return:0 success while others failed
 *
 * */
int oc_mqtt_report_upgradeprogress(const char *deviceID, const char *objectDeviceID, \
                                    int upgraderet, const char *version, int progress);


#endif /* LITEOS_LAB_IOT_LINK_OC_OC_MQTT_OC_MQTT_PROFILE_V5_OC_MQTT_EVENT_H_ */
