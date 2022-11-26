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


#ifndef __OC_COAP_AL_H
#define __OC_COAP_AL_H


#include <stddef.h>
#include <stdint.h>
//#include "coap_al.h"

/** @brief this is the message dealer module for the application*/
typedef int (*FnOcCoapMsgDeal)(void *msg, int len);

typedef enum
{
    EN_OC_BOOT_STRAP_MODE_FACTORY = 0,
    EN_OC_BOOT_STRAP_MODE_CLINET_INITIALIZE,
    EN_OC_BOOT_STRAP_MODE_SMARTCARD,
}EnOcBootStrapModeT;

typedef struct
{
    char *epId;                  ///< endpoint identifier, which could be recognized by the server
    char *address;                ///< server address,maybe domain name
    char *port;                   ///< server port
    char *pskId;                 ///< server encode by psk, if not set NULL here
    char *psk;
    int   pskLen;
}OcServerT;

/** @brief this is the agent configuration */
typedef struct
{
    EnOcBootStrapModeT     bootMode;       ///< bootmode,if boot client_initialize, then the bs must be set
    OcServerT              bootServer;     ///< which will be used by the bootstrap, if not, set NULL here
    OcServerT              appServer;      ///< if factory or smart boot, must be set here
    FnOcCoapMsgDeal        rcvFunc;        ///< receive function caller here
    void                  *usrData;        ///< used for the user
}OcConfigParmT;

typedef enum
{
    EN_OC_COAP_ERR_OK          = 0,      ///< this means the status ok
    EN_OC_COAP_ERR_PARAFMT,              ///< this means the parameter err format
    EN_OC_COAP_ERR_NETWORK,              ///< this means the network wrong status
    EN_OC_COAP_ERR_CONSERVER,            ///< this means the server refused the service for some reason(likely the id and pwd)
    EN_OC_COAP_ERR_NOCONFIGURED,         ///< this means we have not configure it yet,so could not connect
    EN_OC_COAP_ERR_CONFIGURED,           ///< this means we has configured it, so could not reconfigure it
    EN_OC_COAP_ERR_NOCONNECTED,          ///< this means the connection has not been built, so you could not send data
    EN_OC_COAP_ERR_GETHUBADDRTIMEOUT,    ///< this means get the hub address timeout
    EN_OC_COAP_ERR_SYSMEM,               ///< this means the system memory is not enough
    EN_OC_COAP_ERR_SYSTEM,               ///< this means that the system porting may have some problem,maybe not install yet
    en_oc_coap_err_last = 255,           ///< not defined
}EnOcCoapErrCodeT;
///////////////////////////COAP AGENT INTERFACE////////////////////////////////
typedef void* (*FnOcCoapConnect)(OcConfigParmT *param);                        ///< return the handle here
typedef int (*OcCoapDisConnect)(void *handle);                                    ///< use the handle as the params
typedef int (*FnOcCoapReport)(void *handle,char *msg,int len);                    ///< use the handle and report params
/**
 * @brief this data structure defines the coap agent implement
 */
typedef struct
{
    FnOcCoapConnect   config;   ///< this function used for the configuration
    FnOcCoapReport    report;   ///< this function used for the report data to the cdp
    OcCoapDisConnect  deconfig; ///< this function used for the deconfig
}oc_coap_opt_t;

/**
 *@brief the coap agent should use this function to register the method for the application
 *
 *@param[in] opt, the operation method implement by the coap agent developer
 *@return 0 success while -1 failed
 */
int oc_coap_register(const char *name,const oc_coap_opt_t *opt);


//////////////////////////APPLICATION INTERFACE/////////////////////////////////

/**
 * @brief the application use this function to configure the coap agent
 * @param[in] param, refer to tag_OcCoapConnect
 * @return oc coap handle else NULL failed
 */
void *OcCoapConnect(OcConfigParmT *param);
/**
 * @brief the application use this function to send the message to the cdp
 * @param[in] hanlde:the coap handle returned by OcCoapConnect
 * @param[in] report:the message to report
 *
 * @return 0 success while <0 failed
 */
int OcCoapReport(void *handle,char *msg,int len);

/**
 *@brief: the application use this function to deconfigure the coap agent
 *
 *@param[in] handle: returned by OcCoapConnect
 *
 * return 0 success while <0 failed
 */

int oc_coap_deconfig(void *handle);

/**
 *@brief this is the oc coap  initialize function,must be called first
 *
 *@return 0 success while <0 failed
 */
int oc_coap_init();


#endif /* __OC_COAP_AL_H */
