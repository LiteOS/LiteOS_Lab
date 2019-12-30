/*----------------------------------------------------------------------------
 * Copyright (c) <2019>, <Huawei Technologies Co., Ltd>
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
 *  2019-11-13       xuliqun  The first version  
 *
 */



#ifndef __TEST_CASE_H
#define __TEST_CASE_H

#define TS_OK_HAS_DATA         1
#define TS_OK                  0
#define TS_FAILED             -1

#define TS_RECV_BUFFER_LEN 512


#define TEST_SORT_OC_LWM2M_AL  0
#define TEST_SORT_LWM2M_AL     1
#define TEST_SORT_OC_MQTT_AL   2
#define TEST_SORT_MQTT_AL      3
#define TEST_SORT_OC_COAP_AL   4
#define TEST_SORT_COAP_AL      5
#define TEST_SORT_MAX          6

/*oc lwm2m*/
#define TEST_OC_LWM2M_INIT            ((TEST_SORT_OC_LWM2M_AL << 16) | 0)
#define TEST_OC_LWM2M_REGISTER        ((TEST_SORT_OC_LWM2M_AL << 16) | 1)
#define TEST_OC_LWM2M_CONFIG          ((TEST_SORT_OC_LWM2M_AL << 16) | 2)
#define TEST_OC_LWM2M_REPORT          ((TEST_SORT_OC_LWM2M_AL << 16) | 3)
#define TEST_OC_LWM2M_UNREGISTER      ((TEST_SORT_OC_LWM2M_AL << 16) | 4)
#define TEST_OC_LWM2M_DECONFIG        ((TEST_SORT_OC_LWM2M_AL << 16) | 5)
#define TEST_OC_LWM2M_DEINIT          ((TEST_SORT_OC_LWM2M_AL << 16) | 6)
#define TEST_OC_LWM2M_DEREGISTER      ((TEST_SORT_OC_LWM2M_AL << 16) | 7)
#define TEST_OC_LWM2M_INVALID         ((TEST_SORT_OC_LWM2M_AL << 16) | 8) 

/*lwm2m al*/
#define TEST_LWM2M_AL_INSTALL         ((TEST_SORT_LWM2M_AL<<16) | 0)
#define TEST_LWM2M_AL_CONNETC         ((TEST_SORT_LWM2M_AL<<16) | 1)
#define TEST_LWM2M_AL_CONFIG          ((TEST_SORT_LWM2M_AL<<16) | 2)
#define TEST_LWM2M_AL_ADD_OBJECT      ((TEST_SORT_LWM2M_AL<<16) | 3)
#define TEST_LWM2M_AL_SEND            ((TEST_SORT_LWM2M_AL<<16) | 4)
#define TEST_LWM2M_AL_RECEIVE         ((TEST_SORT_LWM2M_AL<<16) | 5)
#define TEST_LWM2M_AL_DELETE_OBJECT   ((TEST_SORT_LWM2M_AL<<16) | 6)
#define TEST_LWM2M_AL_DISCONNECT      ((TEST_SORT_LWM2M_AL<<16) | 7)
#define TEST_LWM2M_AL_DECONFIG        ((TEST_SORT_LWM2M_AL<<16) | 8)
#define TEST_LWM2M_AL_UNINSTALL       ((TEST_SORT_LWM2M_AL<<16) | 9)
#define TEST_LWM2M_AL_INVALID         ((TEST_SORT_LWM2M_AL<<16) | 10)



/*oc mqtt*/
#define TEST_OC_MQTT_INIT             ((TEST_SORT_OC_MQTT_AL << 16) | 0)
#define TEST_OC_MQTT_REGISTER         ((TEST_SORT_OC_MQTT_AL << 16) | 1)
#define TEST_OC_MQTT_CONFIG           ((TEST_SORT_OC_MQTT_AL << 16) | 2)
#define TEST_OC_MQTT_JSON_FMT_REQ     ((TEST_SORT_OC_MQTT_AL << 16) | 3)
#define TEST_OC_MQTT_JSON_FMT_RES     ((TEST_SORT_OC_MQTT_AL << 16) | 4)
#define TEST_OC_MQTT_PUBLISH          ((TEST_SORT_OC_MQTT_AL << 16) | 5)
#define TEST_OC_MQTT_DECONFIG         ((TEST_SORT_OC_MQTT_AL << 16) | 6)
#define TEST_OC_MQTT_DEINIT           ((TEST_SORT_OC_MQTT_AL << 16) | 7)
#define TEST_OC_MQTT_GETVALUE         ((TEST_SORT_OC_MQTT_AL << 16) | 8)
#define TEST_OC_MQTT_INVALID          ((TEST_SORT_OC_MQTT_AL << 16) | 9)


/*mqtt al*/
#define TEST_MQTT_AL_INIT             ((TEST_SORT_MQTT_AL << 16) | 0)
#define TEST_MQTT_AL_DEINT            ((TEST_SORT_MQTT_AL << 16) | 1)
#define TEST_MQTT_AL_INSTALL          ((TEST_SORT_MQTT_AL << 16) | 2)
#define TEST_MQTT_AL_UNINSTALL        ((TEST_SORT_MQTT_AL << 16) | 3)
#define TEST_MQTT_AL_CONNECT          ((TEST_SORT_MQTT_AL << 16) | 4)
#define TEST_MQTT_AL_DISCONNECT       ((TEST_SORT_MQTT_AL << 16) | 5)
#define TEST_MQTT_AL_SUBSCRIBLE       ((TEST_SORT_MQTT_AL << 16) | 6)
#define TEST_MQTT_AL_UNSUBSCRIBLE     ((TEST_SORT_MQTT_AL << 16) | 7)
#define TEST_MQTT_AL_PUBLISH          ((TEST_SORT_MQTT_AL << 16) | 8)
#define TEST_MQTT_AL_CHECKSTATUS      ((TEST_SORT_MQTT_AL << 16) | 9)

/*oc coap*/
#define TEST_OC_COAP_INIT             ((TEST_SORT_OC_COAP_AL << 16) | 0)
#define TEST_OC_COAP_REGISTER         ((TEST_SORT_OC_COAP_AL << 16) | 1)
#define TEST_OC_COAP_CONFIG           ((TEST_SORT_OC_COAP_AL << 16) | 2)
#define TEST_OC_COAP_REPORT           ((TEST_SORT_OC_COAP_AL << 16) | 3)
#define TEST_OC_COAP_GETVALUE         ((TEST_SORT_OC_COAP_AL << 16) | 4)
#define TEST_OC_COAP_DECONFIG         ((TEST_SORT_OC_COAP_AL << 16) | 5)
#define TEST_OC_COAP_DEINIT           ((TEST_SORT_OC_COAP_AL << 16) | 6)


/*coap al*/
/*
#define TEST_COAP_AL_INIT             ((TEST_SORT_COAP_AL << 16) | 0)
#define TEST_COAP_AL_DEINIT           ((TEST_SORT_COAP_AL << 16) | 1)
#define TEST_COAP_AL_ADD_OPTION       ((TEST_SORT_COAP_AL << 16) | 2)
#define TEST_COAP_AL_NEW_REQUEST      ((TEST_SORT_COAP_AL << 16) | 3)
#define TEST_COAP_AL_SEND             ((TEST_SORT_COAP_AL << 16) | 4)
#define TEST_COAP_AL_RECV             ((TEST_SORT_COAP_AL << 16) | 5)
#define TEST_COAP_AL_INSTALL          ((TEST_SORT_COAP_AL << 16) | 6)
#define TEST_COAP_AL_UNINSTALL        ((TEST_SORT_COAP_AL << 16) | 7)
*/


typedef int (*test_sort)(int entry_id, char *message, int len);
typedef int (*test_entry)(char *message, int len);

void autotest_start(void);



int ts_sort_oc_lwm2m_al(int entry_id, char *message, int len);
int ts_sort_lwm2m_al(int entry_id, char *message, int len);
int ts_sort_oc_mqtt_al(int entry_id, char *message, int len);

int ts_sort_mqtt_al(int entry_id, char *message, int len);
/*oc coap al*/
int ts_sort_oc_coap_al(int entry_id, char *message, int len);
/*coap al*/
int ts_sort_coap_al(int entry_id, char *message, int len);
#endif

