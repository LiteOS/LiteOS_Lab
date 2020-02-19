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
 *  2020-02-18 09:00  zhangqianfu  The first version
 *
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "los_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/timing.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/timing.h"



#include <sal.h>
#include <osal.h>
#include <dtls_al.h>

#define MBEDTLS_DEBUG

#ifdef MBEDTLS_DEBUG
#define MBEDTLS_LOG(fmt, ...) \
    do \
    { \
        (void)printf("[MBEDTLS][%s:%d] " fmt "\r\n", \
        __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define MBEDTLS_LOG(fmt, ...) ((void)0)
#endif


#ifndef CFG_TLS_SHAKEHAND_TIMEOUT_DEFAULT
#define CFG_TLS_SHAKEHAND_TIMEOUT_DEFAULT 1000
#endif


#define CN_MBED_PERS                 "ssl_client"

typedef struct
{
    mbedtls_ssl_context              *ssl;
    mbedtls_ssl_config               *conf;
    mbedtls_entropy_context          *entropy;
    mbedtls_ctr_drbg_context         *ctr_drbg;
    mbedtls_timing_delay_context     *timer;
    mbedtls_net_context              *netio;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt                 *ca_cert;
    mbedtls_x509_crt                 *client_ca;
    mbedtls_pk_context               *client_pk;
#endif
    int                               client_server;
    int                               tcp_udp;
}mbed_cb_t;

int mbed_new(dtls_para_t *para, void **handle)
{

    int ret = EN_DTSL_AL_ERR_PARA;
    mbed_cb_t  *cb;

    if((NULL == para) || (NULL == handle))
    {
        return ret;
    }
    cb = mbedtls_calloc(1, sizeof(mbed_cb_t));
    if(NULL == cb)
    {
        ret = EN_DTSL_AL_ERR_SYSMEM;
        return ret;
    }
    cb->client_server = para->isclient? MBEDTLS_SSL_IS_CLIENT:MBEDTLS_SSL_IS_SERVER;
    cb->tcp_udp = para->istcp ? MBEDTLS_SSL_TRANSPORT_STREAM:MBEDTLS_SSL_TRANSPORT_DATAGRAM;

    cb->ssl = mbedtls_calloc(1, sizeof(mbedtls_ssl_context));
    cb->conf = mbedtls_calloc(1, sizeof(mbedtls_ssl_config));
    cb->entropy = mbedtls_calloc(1, sizeof(mbedtls_entropy_context));
    cb->ctr_drbg = mbedtls_calloc(1, sizeof(mbedtls_ctr_drbg_context));

    if ((NULL == cb->ssl) || (NULL == cb->conf) || (NULL == cb->entropy) || (NULL == cb->ctr_drbg))
    {
        ret = EN_DTSL_AL_ERR_SYSMEM;
        goto EXIT_FAIL;
    }
    mbedtls_ssl_init(cb->ssl);
    mbedtls_ssl_config_init(cb->conf);
    mbedtls_ctr_drbg_init(cb->ctr_drbg);
    mbedtls_entropy_init(cb->entropy);

    if ((mbedtls_ctr_drbg_seed(cb->ctr_drbg, mbedtls_entropy_func, cb->entropy,
                               (const unsigned char *)CN_MBED_PERS,strlen(CN_MBED_PERS))) != 0)
    {
        ret = EN_DTSL_AL_ERR_SYS;
        MBEDTLS_LOG("mbedtls_ctr_drbg_seed failed: -0x%x", -ret);
        goto EXIT_FAIL;
    }
    mbedtls_ssl_conf_rng(cb->conf, mbedtls_ctr_drbg_random, cb->ctr_drbg);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if(para->security.type == EN_DTSL_SECURITY_TYPE_CERT)
    {
        if(NULL == para->security.u.cas.server_ca)
        {
            ret = EN_DTSL_AL_ERR_PARA;
            goto EXIT_FAIL;
        }

        cb->ca_cert = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
        if(NULL == cb->ca_cert)
        {
            ret = EN_DTSL_AL_ERR_SYSMEM;
            goto EXIT_FAIL;
        }
        mbedtls_x509_crt_init(cb->ca_cert);
        ret = mbedtls_x509_crt_parse(cb->ca_cert, para->security.u.cas.server_ca,  para->security.u.cas.server_ca_len);
        if(ret < 0)
        {
            MBEDTLS_LOG("mbedtls_x509_crt_parse failed -0x%x", -ret);
            goto EXIT_FAIL;
        }

        if((NULL != para->security.u.cas.client_ca) && (NULL != para->security.u.cas.client_pk)) /// we'd better to make the client ca and key
        {
            cb->client_ca    = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
            cb->client_pk    = mbedtls_calloc(1, sizeof(mbedtls_pk_context));
            if((NULL == cb->client_ca) || (NULL == cb->client_pk))
            {
                goto EXIT_FAIL;
            }
            mbedtls_x509_crt_init(cb->ca_cert);
            mbedtls_pk_init(cb->client_pk);

            if(mbedtls_x509_crt_parse(cb->client_ca,para->security.u.cas.client_ca,para->security.u.cas.client_ca_len)  !=0)
            {
                ret = EN_DTSL_AL_ERR_CLIENTCERTPARSE;
                goto EXIT_FAIL;
            }

            if(mbedtls_pk_parse_key(cb->client_pk,para->security.u.cas.client_pk,para->security.u.cas.client_pk_len,\
                    para->security.u.cas.client_pk_pwd,para->security.u.cas.client_pk_pwd_len) < 0)
            {
                ret = EN_DTSL_AL_ERR_CLIENTCERTPARSE;
                goto EXIT_FAIL;
            }

            if(mbedtls_ssl_conf_own_cert( cb->conf, cb->client_ca, cb->client_pk ) != 0)
            {
                ret = EN_DTSL_AL_ERR_SYSMEM;
                goto EXIT_FAIL;
            }
            mbedtls_ssl_conf_ca_chain(cb->conf, cb->ca_cert, NULL);
        }
    }

#endif

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if (para->security.type == EN_DTSL_SECURITY_TYPE_PSK)
    {
        if (mbedtls_ssl_conf_psk(cb->conf,para->security.u.psk.psk_key,para->security.u.psk.psk_key_len,\
                para->security.u.psk.psk_id,para->security.u.psk.psk_id_len) != 0)
        {
            ret = EN_DTSL_AL_ERR_SYSMEM;
            MBEDTLS_LOG("mbedtls_ssl_conf_psk failed: -0x%x", -ret);
            goto EXIT_FAIL;
        }
    }
#endif

    if (cb->tcp_udp ==  MBEDTLS_SSL_TRANSPORT_STREAM)
    {
        mbedtls_ssl_conf_read_timeout(cb->conf, CFG_TLS_SHAKEHAND_TIMEOUT_DEFAULT);
    }
    else
    {
        cb->timer = mbedtls_calloc(1, sizeof(mbedtls_timing_delay_context));
        if (NULL == cb->timer)
        {
            goto EXIT_FAIL;
        }
        mbedtls_ssl_set_timer_cb(cb->ssl, cb->timer, mbedtls_timing_set_delay,mbedtls_timing_get_delay);
    }

    if (mbedtls_ssl_config_defaults(cb->conf,cb->client_server,cb->tcp_udp, MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    {
        MBEDTLS_LOG("mbedtls_ssl_config_defaults failed: -0x%x", -ret);
        ret = EN_DTSL_AL_ERR_SYSMEM;
        goto EXIT_FAIL;
    }

    mbedtls_ssl_conf_authmode(cb->conf, MBEDTLS_SSL_VERIFY_REQUIRED);

    MBEDTLS_LOG("setting up the SSL structure");
    if (mbedtls_ssl_setup(cb->ssl, cb->conf) != 0)
    {
        MBEDTLS_LOG("mbedtls_ssl_setup failed: -0x%x", -ret);
        ret = EN_DTSL_AL_ERR_SYSMEM;
        goto EXIT_FAIL;
    }
    MBEDTLS_LOG("set SSL structure succeed");
    ret = EN_DTSL_AL_ERR_NONE;
    *handle = cb;
    return ret;

EXIT_FAIL:

    if (cb->conf)
    {
        mbedtls_ssl_config_free(cb->conf);
        mbedtls_free(cb->conf);
    }

    if (cb->ctr_drbg)
    {
        mbedtls_ctr_drbg_free(cb->ctr_drbg);
        mbedtls_free(cb->ctr_drbg);
    }

    if (cb->entropy)
    {
        mbedtls_entropy_free(cb->entropy);
        mbedtls_free(cb->entropy);
    }

    if (cb->timer)
    {
        mbedtls_free(cb->timer);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (cb->ca_cert)
    {
        mbedtls_x509_crt_free(cb->ca_cert);
        mbedtls_free(cb->ca_cert);
    }

    if(cb->client_ca)
    {
        mbedtls_x509_crt_free(cb->client_ca);
        mbedtls_free(cb->client_ca);
    }

    if(cb->client_pk)
    {
        mbedtls_pk_free(cb->client_pk);
        mbedtls_free(cb->client_pk);
    }
#endif

    if (cb->ssl)
    {
        mbedtls_ssl_free(cb->ssl);
        mbedtls_free(cb->ssl);
    }
    mbedtls_free(cb);
    return ret;
}




static inline uint32_t dtls_gettime()
{
    return (uint32_t)(osal_sys_time() / 1000);
}


int mbed_connect(void *handle,const char *server_ip, const char *server_port,int timeout)
{
    int ret =-1;
    int max_value;
    int change_value;
    mbed_cb_t  *cb;

    if(NULL == handle)
    {
        return ret;
    }

    cb = handle;
    cb->netio =  (mbedtls_net_context *)mbedtls_calloc(1,sizeof(mbedtls_net_context));
    if(NULL == cb->netio)
    {
        return ret;
    }

    if(0 != mbedtls_net_connect(cb->netio, server_ip, server_port, cb->tcp_udp))
    {
        goto EXIT_FAIL;
    }
    mbedtls_ssl_set_bio(cb->ssl, cb->netio,mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    MBEDTLS_LOG("performing the SSL/TLS handshake");

    max_value = ( cb->tcp_udp == MBEDTLS_NET_PROTO_UDP) ?(dtls_gettime() + timeout) :  50;

    do
    {
        ret = mbedtls_ssl_handshake(cb->ssl);

        if (cb->tcp_udp == MBEDTLS_NET_PROTO_TCP)
        {
            change_value++;
        }
        else
        {
            change_value = dtls_gettime();
        }
    }
    while ((ret == MBEDTLS_ERR_SSL_WANT_READ ||ret == MBEDTLS_ERR_SSL_WANT_WRITE ||\
            (ret == MBEDTLS_ERR_SSL_TIMEOUT && cb->tcp_udp == MBEDTLS_NET_PROTO_TCP)) &&\
            (change_value < max_value));

    if(ret != 0)
    {
        goto EXIT_FAIL;
    }

    return ret;


EXIT_FAIL:
    if (cb->netio)
    {
        mbedtls_net_free(cb->netio);
        cb->ssl->p_bio = NULL;
    }
    return ret;
}


int mbed_destroy(void *handle)
{
    int ret = -1;

    mbed_cb_t  *cb;

    cb = handle;

    if((NULL == cb)|| (NULL == cb->ssl))
    {
        return ret;
    }
    do{

        ret = mbedtls_ssl_close_notify( cb->ssl );

    }while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if (cb->netio)
    {
        mbedtls_net_free(cb->netio);
    }

    if (cb->conf)
    {
        mbedtls_ssl_config_free(cb->conf);
        mbedtls_free(cb->conf);
        cb->ssl->conf = NULL; //  need by mbedtls_debug_print_msg(), see mbedtls_ssl_free(ssl)
    }

    if (cb->ctr_drbg)
    {
        mbedtls_ctr_drbg_free(cb->ctr_drbg);
        mbedtls_free(cb->ctr_drbg);
    }

    if (cb->entropy)
    {
        mbedtls_entropy_free(cb->entropy);
        mbedtls_free(cb->entropy);
    }

    if (cb->timer)
    {
        mbedtls_free(cb->timer);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (cb->ca_cert)
    {
        mbedtls_x509_crt_free(cb->ca_cert);
        mbedtls_free(cb->ca_cert);
    }
    if( cb->client_ca )
    {
        mbedtls_x509_crt_free( cb->client_ca);
        mbedtls_free( cb->client_ca );
    }

    if( cb->client_pk )
    {
        mbedtls_pk_free( cb->client_pk );
        mbedtls_free( cb->client_pk );
    }
#endif

    mbedtls_ssl_free( cb->ssl );
    mbedtls_free( cb->ssl );

    return 0;
}


///< make it return as normal socket return

int mbed_write(void *handle, uint8_t *buf, size_t len,int timeout)
{

    int ret  = 0;

    mbed_cb_t  *cb;

    cb = handle;
    if( ( NULL == cb ) || ( NULL == cb->ssl ) )
    {
        return ret;
    }

    ret = mbedtls_ssl_write( cb->ssl, (unsigned char *) buf, len);

    if ((ret == MBEDTLS_ERR_SSL_WANT_WRITE) ||(ret == MBEDTLS_ERR_SSL_TIMEOUT))
    {
        ret =  -1;
    }
    else if (ret <= 0)
    {
        ret = 0;
    }

    return ret;
}


int mbed_read( void *handle, unsigned char *buf, size_t len, int timeout)
{

    int ret  = 0;

    mbed_cb_t  *cb;

    cb = handle;
    if( ( NULL == cb ) || ( NULL == cb->ssl )  || (NULL == cb->conf))
    {
        return ret;
    }

    mbedtls_ssl_conf_read_timeout( cb->conf, timeout );

    ret = mbedtls_ssl_read( cb->ssl, buf, len);

    if ((ret == MBEDTLS_ERR_SSL_WANT_READ) ||(ret == MBEDTLS_ERR_SSL_TIMEOUT))
    {
        ret = -1;
    }
    else if (ret <= 0)
    {
        ret = 0;
    }

    return ret;
}

static const dtls_al_t  s_mbedtls_io =
{
    .name = "mbed",
    .io = {
            .io_new = mbed_new,
            .io_connect = mbed_connect,
            .io_read = mbed_read,
            .io_write = mbed_write,
            .io_destroy = mbed_destroy,
    },
};


int dtls_imp_init(void)
{
    int ret =-1;
    (void)mbedtls_platform_set_calloc_free(osal_calloc, osal_free);
    (void)mbedtls_platform_set_snprintf(snprintf);
    (void)mbedtls_platform_set_printf(printf);
    ret = dtls_al_install(&s_mbedtls_io);
    return ret;
}

