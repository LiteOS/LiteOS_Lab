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
 *  DATE                AUTHOR      INSTRUCTION
 *  2019-05-14 17:22  zhangqianfu  The first version  
 *
 */
#ifndef LITEOS_LAB_IOT_LINK_OC_OC_LWM2M_OC_LWM2M_AL_OC_LWM2M_AL_H_
#define LITEOS_LAB_IOT_LINK_OC_OC_LWM2M_OC_LWM2M_AL_OC_LWM2M_AL_H_


#include <stddef.h>
#include <stdint.h>

/** @brief this is the message dealer module for the application*/
typedef int (*fn_oc_msg_deal)(char *msg, int len);

/** @brief this is the agent configuration */
typedef struct
{
    const char   *devname;         ///< device name
    const char   *server;          ///< cdp server address
    const char   *port;            ///< cdp server port
    const char   *psk_id;          ///< pskid if use psk
    const char   *psk_value;       ///< psk value if use psk
    int           psk_value_len;   ///< psk value length if use psk
}oc_config_param_t;

///////////////////////////LWM2M AGENT INTERFACE////////////////////////////////
typedef int (*fn_oc_lwm2m_report)(char *buf, int len, int timeout);
typedef int (*fn_oc_lwm2m_config)(oc_config_param_t *param);
typedef int (*fn_oc_lwm2m_deconfig)();
/**
 * @brief this data structure defines the lwm2m agent implement
 */
typedef struct
{
    fn_oc_lwm2m_config   config;   ///< this function used for the configuration
    fn_oc_lwm2m_report   report;   ///< this function used for the report data to the cdp
    fn_oc_lwm2m_deconfig deconfig; ///< this function used for the deconfig
}oc_lwm2m_opt_t;

#define CFG_OC_LWM2M_ENABLE  1  //for test

#if CFG_OC_LWM2M_ENABLE
/**
 *@brief the lwm2m agent should use this function to register the method for the application
 *
 *@param[in] opt, the operation method implement by the lwm2m agent developer
 *@return 0 success while <0 failed
 */
int oc_lwm2m_register(const oc_lwm2m_opt_t *opt);

/**
 *@brief when the lwm2m agent receive any application message, please call this function
 *@param[in] msg, the received message buffer
 *@param[in] len, the received message length
 *@return 0 success while <0 failed
 */
int oc_lwm2m_msg_push(char *msg, int len);

//////////////////////////APPLICATION INTERFACE/////////////////////////////////
/**
 * @brief the application could use this function to register a dealer to deal the message received
 * @param[in] dealer when the lwm2m agent receive a message, and match the preset data, then call the dealer
 * @param[in] match_buf, used to match the message,if NULL then this one is default
 * @param[in] match_len, match_buf length
 * @return 0 success while <0 failed
 */
int oc_lwm2m_install_msgdealer(fn_oc_msg_deal dealer,const char *match_buf,int match_len);

/**
 * @brief the application use this function to send the message to the cdp
 * @param[in] buf the message to send
 * @param[in] len the message length
 * @param[in] timeout block time
 *
 * @return 0 success while <0 failed
 */
int oc_lwm2m_report(char *buf, int len,int timeout);

/**
 * @brief the application use this function to configure the lwm2m agent
 * @param[in] param, refer to oc_config_param_t
 * @return 0 success while <0 failed
 */
int oc_lwm2m_config(oc_config_param_t *param);

/**
 *@brief: the application use this function to deconfigure the lwm2m agent
 *
 * return 0 success while <0 failed
 */

int oc_lwm2m_deconfig();

/**
 *@brief this is the oc lwm2m agent initialize function,must be called first
 *
 *@return 0 success while <0 failed
 */
int oc_lwm2m_init();


#else   //not configure the lwm2m agent

#define oc_lwm2m_register(opt)                                              -1
#define oc_lwm2m_msg_push(msg,len)                                          -1
#define oc_lwm2m_install_msgdealer(dealer,match_buf, match_len)             -1
#define oc_lwm2m_report(buf,len,timeout)                                    -1
#define oc_lwm2m_config(param)                                              -1
#define oc_lwm2m_init                                                       -1
#define oc_lwm2m_deconfig                                                   -1

#endif

#endif /* LITEOS_LAB_IOT_LINK_OC_OC_LWM2M_OC_LWM2M_AL_OC_LWM2M_AL_H_ */
