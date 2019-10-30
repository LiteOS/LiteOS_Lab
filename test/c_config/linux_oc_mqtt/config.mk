#/*----------------------------------------------------------------------------
# * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
# * All rights reserved.
# * Redistribution and use in source and binary forms, with or without modification,
# * are permitted provided that the following conditions are met:
# * 1. Redistributions of source code must retain the above copyright notice, this list of
# * conditions and the following disclaimer.
# * 2. Redistributions in binary form must reproduce the above copyright notice, this list
# * of conditions and the following disclaimer in the documentation and/or other materials
# * provided with the distribution.
# * 3. Neither the name of the copyright holder nor the names of its contributors may be used
# * to endorse or promote products derived from this software without specific prior written
# * permission.
# * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# *---------------------------------------------------------------------------*/
#/*----------------------------------------------------------------------------
# * Notice of Export Control Law
# * ===============================================
# * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
# * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
# * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
# * applicable export control laws and regulations.
# *---------------------------------------------------------------------------*/
#/**
# *  DATE                AUTHOR      INSTRUCTION
# *  2019-04-28 15:00  zhangqianfu  The first version  
# *
# */
### this is the template to tell you how to configure the iot_link, maybe 
### not good enough, please give some advice if any idea
################################################################################
###
### introduce each configuration variable function and which value it support
### and  introduce you how to  configure the sdk

### y means yes and enable, while n means no and disable
### "none" means no one matched and you could build your own
###

################################################################################
########################OS SERVICE START########################################

#CONFIG_OS_TYPE , could be: "linux" "macos" "liteos", and you could select one
CONFIG_OS_TYPE := "linux"

#CONFIG_ARCH_CPU_TYPE,could be "armv7-m" "armv6-m" "riscv32" "x86" "x86-64"
CONFIG_ARCH_CPU_TYPE := "x86-64"

#CONFIG_SHELL_ENABLE , we supply a simple shell on the liteos,which depends the liteos,support: y n
CONFIG_SHELL_ENABLE := n

#CONFIG_STIMER_ENABLE , we supply a simple softtimer on the osal,you could choose yes or no
CONFIG_STIMER_ENABLE := y

#CONFIG_DRIVER_ENABLE , we supply a easy and simple driver framework for the sdk
#this option based on the liteos, so you must choose liteos
CONFIG_DRIVER_ENABLE := n

#CONFIG_DRIVER_ENABLE , we supply a easy and simple at framework for the sdk
#this option based on the driver framework, so you must choose CONFIG_DRIVER_ENABLE
CONFIG_AT_ENABLE     := n
########################OS SERVICE END##########################################


########################INDEPENDENT SERVICE START#################################

#CONFIG_CJSON_ENABLE , we port the cJSON based on the osal,you could choose yes or no
CONFIG_CJSON_ENABLE := y

########################INDEPENDENT SERVICE END#################################


########################NETWORK SERVICE START###################################

#CONFIG_TCPIP_ENABLE , we build a sal for the tcpip functions, and you could choose yes or no
#you could build your own application based on the sal, which shielding the defference of 
#variouse tcpip function.what's more, you could add your own tcpip stack to the sdk.
#and if this option is enabled, then you select  one type for your program, we now
#CONFIG_TCPIP_TYPE could be:"lwip_socket" "linux_socket" "macos_socket" "esp8266_socket" "none"
CONFIG_TCPIP_ENABLE := y
CONFIG_TCPIP_TYPE := "linux_socket"

#CONFIG_DTLS_ENABLE , we supply a DTLS AL (building),you could choose yes or no
#CONFIG_DTLS_TYPE, could be "mbedtls_psk" "mbedtls_cert" "none"

CONFIG_DTLS_ENABLE   := y
CONFIG_DTLS_TYPE     := "mbedtls_cert"

#CONFIG_MQTT_ENABLE, we build a mqtt abstraction, which shield the difference of 
#the implement of mqtt. 
#CONFIG_MQTT_TYPE could be "paho_mqtt" "none"
CONFIG_MQTT_ENABLE   := y
CONFIG_MQTT_TYPE     := "lite_mqtt"

#CONFIG_LWM2M_ENABLE, we build a lwm2m abstraction, which shield the difference of 
#the implement of lwm2m. 
#CONFIG_LWM2M_TYPE could be "wakaama_lwm2m" "none"
CONFIG_LWM2M_ENABLE   := n
CONFIG_LWM2M_TYPE     := "wakaama_lwm2m"

#CONFIG_COAP_ENABLE, we build a coap abstraction, which shield the difference of 
#the implement of coap. 
#CONFIG_COAP_TYPE could be "lite_coap" "none"
CONFIG_COAP_ENABLE   := n
CONFIG_COAP_TYPE     := "lite_coap"
########################NETWORK SERVICE END#####################################


########################OC SERVICE START########################################
#CONFIG_OC_COAP_ENABLE, we build a oc coap abstraction for huawei OceanConnect service,
#which shield the difference of the implement of oc coap. 
#CONFIG_OC_MQTT_TYPE could be "soft" "none"
CONFIG_OC_COAP_ENABLE := n
CONFIG_OC_COAP_TYPE   := "soft"

#CONFIG_OC_MQTT_ENABLE, we build a oc mqtt abstraction for huawei OceanConnect service,
#which shield the difference of the implement of oc mqtt. 
#CONFIG_OC_MQTT_TYPE could be "soft" "ec20_oc" "none"
CONFIG_OC_MQTT_ENABLE := y
CONFIG_OC_MQTT_TYPE   := "soft"

#CONFIG_OC_LWM2M_ENABLE, we build a oc lwm2m abstraction for huawei OceanConnect service,
#which shield the difference of the implement of oc lwm2m. 
#CONFIG_OC_MQTT_TYPE could be "soft" "boudica150_oc" "none"
CONFIG_OC_LWM2M_ENABLE := n
CONFIG_OC_LWM2M_TYPE   := "soft"

########################OC SERVICE END##########################################

########################OTA SERVICE START#######################################
#if you want to use the ota, then please enable it
#warning:
#1, the loader maybe need ota
#2, the upgrade method need ota,now we only  support the pcp mode

CONFIG_OTA_ENABLE      := n
CONFIG_PCP_ENABLE      := n

########################OTA SERVICE END#########################################

########################STANDARD DEMO START#####################################
#we create many standard demos, which maybe hardware independent,may help you to 
#create your own service,if the CONFIG_DEMO_ENABLE is enable and CONFIG_DEMO_TYPE
#is "none", then you should build a demo has the same name standard_app_demo_main.if you want
#to build a specified differnt name for your own demo, please make CONFIG_DEMO_ENABLE
#disable.
#warning:
#1, if you use the oc mqtt, then the tls must support cert mode
#2，if you use the oc lwm2m with encode mode,then the dtls must support psk mode

#CONFIG_DEMO_TYPE could be "oc_coap_demo" "oc_dtls_coap_demo" "oc_dtls_lwm2m_bs_demo" "oc_dtls_lwm2m_demo"
#"oc_lwm2m_bs_demo"  "oc_lwm2m_demo" "oc_lwm2m_ota_demo" "oc_tls_mqtt_bs_demo" "oc_tls_mqtt_demo"  "stimer_demo"

CONFIG_DEMO_ENABLE := n
CONFIG_DEMO_TYPE   := "none"

#########################STANDARD DEMO END######################################
include $(TOP_DIR)/iot_link/iot.mk
TEST_CONFIG_OC_LWM2M_ENABLE := n
TEST_CONFIG_LWM2M_AL_ENABLE := n
TEST_CONFIG_OC_MQTT_ENABLE := y
TEST_CONFIG_MQTT_AL_ENABLE := n
include $(TOP_DIR)/test/c_dependcy/test.mk
