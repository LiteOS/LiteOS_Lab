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


#ifndef __OC_LWM2M_AL_H
#define __OC_LWM2M_AL_H


#include <stddef.h>
#include <stdint.h>


#define LOS_CFG_OC_LWM2M_AGENT  1

/** @brief this is the message dealer module for the application*/
typedef int32_t (*fn_oc_msg_deal)(uint8_t *msg, int32_t len);

/** @brief this is the agent configuration */
typedef struct
{
    const char   *devname;         ///< device name
    const char   *server;          ///< cdp server address
    const char   *port;            ///< cdp server port
    const char   *psk_id;          ///< pskid if use psk
    const char   *psk_value;       ///< psk value if use psk
    int32_t       psk_value_len;   ///< psk value length if use psk
}tag_oc_config_param;

///////////////////////////LWM2M AGENT INTERFACE////////////////////////////////
typedef int (*fn_oc_lwm2m_report)(uint8_t *buf, int32_t len, int32_t timeout);
typedef int (*fn_oc_lwm2m_config)(tag_oc_config_param *param);
/**
 * @brief this data structure defines the lwm2m agent implement
 */
typedef struct
{
    fn_oc_lwm2m_config config; ///< this function used for the configuration
    fn_oc_lwm2m_report report; ///< this function used for the report data to the cdp
}tag_oc_lwm2m_opt;


#if LOS_CFG_OC_LWM2M_AGENT
/**
 *@brief the lwm2m agent should use this function to register the method for the application
 *
 *@param[in] opt, the operation method implement by the lwm2m agent developer
 *@return 0 success while <0 failed
 */
int32_t oc_lwm2m_register(const tag_oc_lwm2m_opt *opt);

/**
 *@brief when the lwm2m agent receive any application message, please call this function
 *@param[in] msg, the received message buffer
 *@param[in] len, the received message length
 *@return 0 success while <0 failed
 */
int32_t oc_lwm2m_msg_push(uint8_t *msg, int32_t len);

//////////////////////////APPLICATION INTERFACE/////////////////////////////////
/**
 * @brief the application could use this function to register a dealer to deal the message received
 * @param[in] dealer when the lwm2m agent receive a message, and match the preset data, then call the dealer
 * @param[in] match_buf, used to match the message,if NULL then this one is default
 * @param[in] match_len, match_buf length
 * @return 0 success while <0 failed
 */
int32_t oc_lwm2m_install_msgdealer(fn_oc_msg_deal dealer,const char *match_buf,int32_t match_len);

/**
 * @brief the application use this function to send the message to the cdp
 * @param[in] buf the message to send
 * @param[in] len the message length
 * @param[in] timeout block time
 *
 * @return 0 success while <0 failed
 */
int32_t oc_lwm2m_report(uint8_t *buf, int32_t len,int32_t timeout);

/**
 * @brief the application use this function to configure the lwm2m agent
 * @param[in] param, refer to tag_oc_config_param
 * @return 0 success while <0 failed
 */
int32_t oc_lwm2m_config(tag_oc_config_param *param);

/**
 *@brief this is the oc lwm2m agent initialize function,must be called first
 *
 *@return 0 success while <0 failed
 */
int32_t oc_lwm2m_agent_init();


#else   //not configure the lwm2m agent

#define oc_lwm2m_register(opt)                                              -1
#define oc_lwm2m_msg_push(msg,len)                                          -1
#define oc_lwm2m_install_msgdealer(dealer,match_buf, match_len)             -1
#define oc_lwm2m_report(buf,len,timeout)                                    -1
#define oc_lwm2m_config(param)                                              -1
#define oc_lwm2m_agent_init                                                 -1

#endif

#endif /* __OC_LWM2M_AL_H */
