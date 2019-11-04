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

################################################################################
# this is used for configure which demo you will use
################################################################################

#CONFIG_DEMO_TYPE could be "oc_coap_demo" "oc_dtls_coap_demo" "oc_dtls_lwm2m_bs_demo" "oc_dtls_lwm2m_demo"
#"oc_lwm2m_bs_demo"  "oc_lwm2m_demo" "oc_lwm2m_ota_demo" "oc_tls_mqtt_bs_demo" "oc_tls_mqtt_demo"  "stimer_demo"

STANDARD_DEMOS_SOURCE :=

ifeq ($(CONFIG_DEMO_ENABLE), y)

    ifeq ($(CONFIG_DEMO_TYPE), "oc_coap_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_coap_demo.c	
    else ifeq ($(CONFIG_DEMO_TYPE), "oc_dtls_coap_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_dtls_coap_demo.c	
    else ifeq ($(CONFIG_DEMO_TYPE), "oc_dtls_lwm2m_bs_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_dtls_lwm2m_bs_demo.c	
    	C_DEFS += -D LWM2M_BOOTSTRAP=1
   	else ifeq ($(CONFIG_DEMO_TYPE), "oc_dtls_lwm2m_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_dtls_lwm2m_demo.c	    
   	else ifeq ($(CONFIG_DEMO_TYPE), "oc_lwm2m_bs_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_lwm2m_bs_demo.c
    	C_DEFS += -D LWM2M_BOOTSTRAP=1
    else ifeq ($(CONFIG_DEMO_TYPE), "oc_lwm2m_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_lwm2m_demo.c
   	else ifeq ($(CONFIG_DEMO_TYPE), "oc_lwm2m_ota_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_lwm2m_ota_demo.c	 	    
    else ifeq ($(CONFIG_DEMO_TYPE), "oc_tls_mqtt_bs_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_tls_mqtt_bs_demo.c	    
    else ifeq ($(CONFIG_DEMO_TYPE), "oc_tls_mqtt_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_tls_mqtt_demo.c	
   	else ifeq ($(CONFIG_DEMO_TYPE), "stimer_demo")
    	STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/stimer_demo.c	
    else ifeq ($(CONFIG_DEMO_TYPE), "oc_dtls_lwm2m_ota_demo")
        STANDARD_DEMOS_SOURCE  := $(TOP_DIR)/demos/oc_dtls_lwm2m_ota_demo.c
    else
 		#maybe you want to add your own demos
    endif

    C_DEFS += -D CONFIG_DEMOS_ENABLE=1 -D CONFIG_DEMOS_TYPE=$(CONFIG_DEMO_TYPE)
    C_SOURCES += $(STANDARD_DEMOS_SOURCE) 
    
endif



