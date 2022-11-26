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

#ifndef TLS_AL_H
#define TLS_AL_H


#include <stdint.h>
#include <stddef.h>

typedef enum
{
    EN_TLS_AL_ERR_OK = 0,
    EN_TLS_AL_ERR_PARA,
    EN_TLS_AL_ERR_SYS,
    EN_TLS_AL_ERR_SYSMEM,
    EN_TLS_AL_ERR_NOCONFIG,
    EN_TLS_AL_ERR_NETWORK,
    EN_TLS_AL_ERR_SERVERCERTPARSE,
    EN_TLS_AL_ERR_CLIENTCERTPARSE,
    EN_TLS_AL_ERR_CLIENTPKPARSE,
}EnTlsAlErrT;

/** @brief  this enum all the transport encode we support now*/
typedef enum
{
    EN_TLS_AL_SECURITY_TYPE_NONE = 0,   ///< no encode
    EN_TLS_AL_SECURITY_TYPE_PSK,        ///< use the psk mode in transport layer
    EN_TLS_AL_SECURITY_TYPE_CERT,       ///< use the ca mode in transport layer,only check the server
}EnTlsAlSecurityTypeT;

/** @brief this data defines for the psk mode*/
typedef struct
{
    uint8_t     *pskId;           ///< the psk id
    uint8_t     *pskKey;          ///< the psk key
    int          pskIdLen;       ///< the psk id length
    int          pskKeyLen;      ///< the psk key length
}TlsAlSecurityPskT;


/** @brief this data defines for the cas mode:only check the server  */
typedef struct
{
    uint8_t    *serverCa;
    uint8_t    *clientCert;
    uint8_t    *clientPriKey;
    uint8_t    *clientPriKeyPwd;
    int         serverCaLen;
    int         clientCertLen;
    int         clientPriKeyLen;
    int         clientPriKeyPwdLen;
    char       *serverName;
}TlsAlSecurityCertT;


/** @brief this data defines for the encode parameter for the connect */
typedef struct
{
    EnTlsAlSecurityTypeT      type;         ///< which security type of the data
    union
    {
        TlsAlSecurityPskT     psk;         ///< psk data  if the type is EN_DTSL_SECURITY_TYPE_PSK
        TlsAlSecurityCertT    cert;         ///< cert data  if the type is EN_DTSL_SECURITY_TYPE_CERT
    }u;
}TlsAlSecurityT;

typedef struct
{
    int                 istcp;
    int                 isclient;
    TlsAlSecurityT  security;
}TlsAlParaT;

EnTlsAlErrT  TlsAlNew(TlsAlParaT *para,void **handle);
int   TlsAlConnect(void *handle,const char *ip, const char *port, int timeout );
int   TlsAlWrite(void *handle, uint8_t *msg, size_t len, int timeout );
int   TlsAlRead(void *handle,uint8_t *buf, size_t len,int timeout );
EnTlsAlErrT   TlsAlDestroy(void *handle);

typedef EnTlsAlErrT (*FnTlsAlNew)(TlsAlParaT *para,void **handle);
typedef int (*FnTlsAlConnect)(void *handle,const char *server_ip, const char *server_port,int timeout);
typedef int (*FnTlsAlWrite)(void *handle,uint8_t *msg, size_t len, int timeout);
typedef int (*FnTlsAlRead)(void *handle, uint8_t *buf, size_t len, int timeout);
typedef EnTlsAlErrT (*FnTlsAlDestroy)(void *handle);

typedef struct
{
    FnTlsAlNew            io_new;
    FnTlsAlConnect        io_connect;
    FnTlsAlWrite          io_write;
    FnTlsAlRead           io_read;
    FnTlsAlDestroy        io_destroy;
}TlsAlIoT;

typedef struct
{
    const char     *name;
    TlsAlIoT        io;
}TlsAlT;

int TlsAlInstall(const TlsAlT *tls);
int TlsAlUninstall(const char*name);

///< this function should implemented by the developer of the tls
int TlsImpInit(void);
int TlsAlInit(void) ;  ///< this function will call TlsImpInit()



#endif /* TLS_AL_H_ */
