/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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

/*
 *  Simple DTLS client demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#include "dtls_interface.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl_internal.h"
#include <sal.h>
#include <osal.h>

#define MBEDTLS_LOG LINK_LOG_DEBUG

mbedtls_ssl_context *dtls_ssl_new(dtls_establish_info_s *info, char plat_type)
{
    int ret = -1;
    mbedtls_ssl_context *ssl = NULL;
    mbedtls_ssl_config *conf = NULL;
    mbedtls_entropy_context *entropy = NULL;
    mbedtls_ctr_drbg_context *ctr_drbg = NULL;
    mbedtls_timing_delay_context *timer = NULL;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt *cacert;
    mbedtls_x509_crt *client_ca = NULL;
    mbedtls_pk_context *client_pk = NULL;
#endif

    const char *pers = "ssl_client";
    int transport;
    ssl       = mbedtls_calloc(1, sizeof(mbedtls_ssl_context));
    conf      = mbedtls_calloc(1, sizeof(mbedtls_ssl_config));
    entropy   = mbedtls_calloc(1, sizeof(mbedtls_entropy_context));
    ctr_drbg  = mbedtls_calloc(1, sizeof(mbedtls_ctr_drbg_context));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    cacert    = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
#endif

    if (NULL == info || NULL == ssl
        || NULL == conf || NULL == entropy
        || NULL == ctr_drbg
#if defined(MBEDTLS_X509_CRT_PARSE_C)
        || NULL == cacert
#endif
        )
    {
        goto exit_fail;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if(NULL != info->v.c.client_ca) /// we'd better to make the client ca and key
    {
        client_ca    = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
        client_pk    = mbedtls_calloc(1, sizeof(mbedtls_pk_context));
        if((NULL == client_ca) || (NULL == client_pk))
        {
            goto exit_fail;
        }
        mbedtls_x509_crt_init(client_ca);
        mbedtls_pk_init(client_pk);
    }
#endif

    if (info->udp_or_tcp == MBEDTLS_NET_PROTO_UDP)
    {
        transport = MBEDTLS_SSL_TRANSPORT_DATAGRAM;
        timer = mbedtls_calloc(1, sizeof(mbedtls_timing_delay_context));
        if (NULL == timer)
            goto exit_fail;
    }
    else
    {
        transport = MBEDTLS_SSL_TRANSPORT_STREAM;
    }

    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);
    mbedtls_ctr_drbg_init(ctr_drbg);
    mbedtls_entropy_init(entropy);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (info->psk_or_cert == VERIFY_WITH_CERT)
    {
        mbedtls_x509_crt_init(cacert);
    }
#endif

    if ((ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
                                     (const unsigned char *) pers,
                                     strlen(pers))) != 0)
    {
        MBEDTLS_LOG("mbedtls_ctr_drbg_seed failed: -0x%x", -ret);
        goto exit_fail;
    }

    MBEDTLS_LOG("setting up the SSL structure");

    (void) mbedtls_ssl_config_defaults(conf,plat_type,transport,MBEDTLS_SSL_PRESET_DEFAULT);

    mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_rng(conf, mbedtls_ctr_drbg_random, ctr_drbg);

    if (info->udp_or_tcp == MBEDTLS_NET_PROTO_TCP)
    {
        mbedtls_ssl_conf_read_timeout(conf, TLS_SHAKEHAND_TIMEOUT);
    }

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if (info->psk_or_cert == VERIFY_WITH_PSK)
    {
        if ((ret = mbedtls_ssl_conf_psk(conf,
                                        info->v.p.psk,
                                        info->v.p.psk_len,
                                        info->v.p.psk_identity,
                                        strlen((const char *)info->v.p.psk_identity))) != 0)
        {
            MBEDTLS_LOG("mbedtls_ssl_conf_psk failed: -0x%x", -ret);
            goto exit_fail;
        }
    }
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (info->psk_or_cert == VERIFY_WITH_CERT)
    {
        ret = mbedtls_x509_crt_parse(cacert, info->v.c.ca_cert, info->v.c.cert_len);
        if(ret < 0)
        {
            MBEDTLS_LOG("mbedtls_x509_crt_parse failed -0x%x", -ret);
            goto exit_fail;
        }
        mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(conf, cacert, NULL);

        if(NULL != info->v.c.client_ca) ///< setup the client key and ca
        {
            ret = mbedtls_x509_crt_parse(client_ca,info->v.c.client_ca,info->v.c.client_ca_len);
            if(ret <0)
            {
                goto exit_fail;
            }

            ret = mbedtls_pk_parse_key(client_pk,info->v.c.client_pk,info->v.c.client_pk_len,\
                                       info->v.c.client_pk_pwd,info->v.c.client_pk_pwd_len);
            if(ret < 0)
            {
                goto exit_fail;
            }

            ret = mbedtls_ssl_conf_own_cert( conf, client_ca, client_pk );
            if(ret != 0)
            {
                goto exit_fail;
            }
        }
    }
#endif

    mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);  //TODO--test for no check the ca_rt--only for the debug

    if ((ret = mbedtls_ssl_setup(ssl, conf)) != 0)
    {
        MBEDTLS_LOG("mbedtls_ssl_setup failed: -0x%x", -ret);
        goto exit_fail;
    }

    if (info->udp_or_tcp == MBEDTLS_NET_PROTO_UDP)
    {
        mbedtls_ssl_set_timer_cb(ssl, timer, mbedtls_timing_set_delay,
                                 mbedtls_timing_get_delay);
    }

    MBEDTLS_LOG("set SSL structure succeed");

    return ssl;

exit_fail:

    if (conf)
    {
        mbedtls_ssl_config_free(conf);
        mbedtls_free(conf);
    }

    if (ctr_drbg)
    {
        mbedtls_ctr_drbg_free(ctr_drbg);
        mbedtls_free(ctr_drbg);
    }

    if (entropy)
    {
        mbedtls_entropy_free(entropy);
        mbedtls_free(entropy);
    }

    if (timer)
    {
        mbedtls_free(timer);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (cacert)
    {
        mbedtls_x509_crt_free(cacert);
        mbedtls_free(cacert);
    }

    if(client_ca)
    {
        mbedtls_x509_crt_free(client_ca);
        mbedtls_free(client_ca);
    }

    if(client_pk)
    {
        mbedtls_pk_free(client_pk);
        mbedtls_free(client_pk);
    }
#endif

    if (ssl)
    {
        mbedtls_ssl_free(ssl);
        mbedtls_free(ssl);
    }
    return NULL;
}

static inline uint32_t dtls_gettime()
{
    return (uint32_t)(osal_sys_time() );
}

int dtls_shakehand(mbedtls_ssl_context *ssl, const dtls_shakehand_info_s *info)
{
    int ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
    uint32_t change_value = 0;
    mbedtls_net_context *server_fd = NULL;
    uint32_t max_value;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    unsigned int flags;
#endif

    MBEDTLS_LOG("connecting to server");

    server_fd = (mbedtls_net_context *)osal_malloc(sizeof(mbedtls_net_context));
    if (server_fd == NULL)
    {
        MBEDTLS_LOG("connect failed! mode %d", info->client_or_server);
        ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
        goto exit_fail;
    }

    if (MBEDTLS_SSL_IS_CLIENT == info->client_or_server)
    {
        ret = mbedtls_net_connect(server_fd, info->u.c.host, info->u.c.port, info->udp_or_tcp);
        if( 0 != ret)
        {
            ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
            goto exit_fail;
        }
    }
    else
    {
        //server_fd = (mbedtls_net_context*)atiny_net_bind(NULL, info->u.s.local_port, MBEDTLS_NET_PROTO_UDP);
        ///< --TODO ,not implement yet
    }

    mbedtls_ssl_set_bio(ssl, server_fd,
                        mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    MBEDTLS_LOG("performing the SSL/TLS handshake");

    max_value = ((MBEDTLS_SSL_IS_SERVER == info->client_or_server || info->udp_or_tcp == MBEDTLS_NET_PROTO_UDP) ?
                (dtls_gettime() + info->timeout) :  50);

    do
    {
        ret = mbedtls_ssl_handshake(ssl);

        if (MBEDTLS_SSL_IS_CLIENT == info->client_or_server && info->udp_or_tcp == MBEDTLS_NET_PROTO_TCP)
        {
            change_value++;
        }
        else
        {
            change_value = dtls_gettime();
        }

        if (info->step_notify)
        {
            info->step_notify(info->param);
        }
    }
    while ((ret == MBEDTLS_ERR_SSL_WANT_READ ||
            ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
            (ret == MBEDTLS_ERR_SSL_TIMEOUT &&
            info->udp_or_tcp == MBEDTLS_NET_PROTO_TCP)) &&
            (change_value < max_value));

    if (info->finish_notify)
    {
        info->finish_notify(info->param);
    }

    if (ret != 0)
    {
        MBEDTLS_LOG("mbedtls_ssl_handshake failed: -0x%x", -ret);
        goto exit_fail;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (info->psk_or_cert == VERIFY_WITH_CERT)
    {
        if((flags = mbedtls_ssl_get_verify_result(ssl)) != 0)
        {
            char vrfy_buf[512];
            (void) mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
            MBEDTLS_LOG("cert verify failed: %s", vrfy_buf);
            goto exit_fail;
        }
        else
            MBEDTLS_LOG("cert verify succeed");
    }
#endif

    MBEDTLS_LOG("handshake succeed");

    return 0;

exit_fail:

    if (server_fd)
    {
        mbedtls_net_free(server_fd);
        ssl->p_bio = NULL;
    }

    return ret;

}
void dtls_ssl_destroy(mbedtls_ssl_context *ssl)
{
    int ret;
    mbedtls_ssl_config           *conf = NULL;
    mbedtls_ctr_drbg_context     *ctr_drbg = NULL;
    mbedtls_entropy_context      *entropy = NULL;
    mbedtls_net_context          *server_fd = NULL;
    mbedtls_timing_delay_context *timer = NULL;
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt             *cacert = NULL;
    mbedtls_x509_crt             *client_ca = NULL;
    mbedtls_pk_context           *client_pk = NULL;
    mbedtls_ssl_key_cert         *key_cert = NULL;
#endif

    if (ssl == NULL)
    {
        return;
    }

    conf       = (mbedtls_ssl_config *)ssl->conf;
    server_fd  = (mbedtls_net_context *)ssl->p_bio;
    timer      = (mbedtls_timing_delay_context *)ssl->p_timer;


    do ret = mbedtls_ssl_close_notify( ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );


    if (NULL != conf)
    {
        ctr_drbg   = conf->p_rng;

        if (NULL !=ctr_drbg)
        {
            entropy =  ctr_drbg->p_entropy;
        }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
        cacert     = (mbedtls_x509_crt *)conf->ca_chain;

        key_cert   = conf->key_cert;
        if(NULL != key_cert)
        {
            client_ca = (mbedtls_x509_crt *) key_cert->cert;
            client_pk = key_cert->key;
        }

#endif
    }

    if (NULL != server_fd)
    {
        mbedtls_net_free(server_fd);
    }

    if (NULL != conf)
    {
        mbedtls_ssl_config_free(conf);
        mbedtls_free(conf);
        ssl->conf = NULL; //  need by mbedtls_debug_print_msg(), see mbedtls_ssl_free(ssl)
    }

    if (ctr_drbg)
    {
        mbedtls_ctr_drbg_free(ctr_drbg);
        mbedtls_free(ctr_drbg);
    }

    if (entropy)
    {
        mbedtls_entropy_free(entropy);
        mbedtls_free(entropy);
    }

    if (timer)
    {
        mbedtls_free(timer);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (cacert)
    {
        mbedtls_x509_crt_free(cacert);
        mbedtls_free(cacert);
    }
    if( client_ca )
    {
        mbedtls_x509_crt_free(client_ca);
        mbedtls_free(client_ca);
    }

    if( client_pk )
    {
        mbedtls_pk_free(client_pk);
        mbedtls_free(client_pk);
    }

#endif

    mbedtls_ssl_free(ssl);
    mbedtls_free(ssl);
}

int dtls_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{

    int ret;

    ret = mbedtls_ssl_write(ssl, (unsigned char *) buf, len);

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


///< make it return as normal socket return
int dtls_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len, uint32_t timeout)
{
    int ret;


    mbedtls_ssl_conf_read_timeout((mbedtls_ssl_config *)ssl->conf, timeout);

    ret = mbedtls_ssl_read(ssl, buf, len);

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



int dtls_accept( mbedtls_net_context *bind_ctx,
                mbedtls_net_context *client_ctx,
                void *client_ip, size_t buf_size, size_t *ip_len )
{
    return mbedtls_net_accept(bind_ctx, client_ctx, client_ip, buf_size, ip_len);
}

void dtls_init(void)
{
    (void)mbedtls_platform_set_calloc_free(osal_calloc, osal_free);
    (void)mbedtls_platform_set_snprintf(snprintf);
    (void)mbedtls_platform_set_printf(printf);
}
