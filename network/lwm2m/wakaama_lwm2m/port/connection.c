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

/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/
#include <ctype.h>
#include "connection.h"

#include <sal.h>
//#include <osal.h>

#if defined (WITH_DTLS)
    #include "dtls_interface.h"
#endif
//#include "sal/atiny_socket.h"
//#include <atiny_log.h>
#include "object_comm.h"

#define COAP_PORT "5683"
#define COAPS_PORT "5684"

static lwm2m_connection_err_notify_t g_connection_err_notify = NULL;

static inline void inc_connection_stat(connection_t *connection, connection_err_e type)
{
    static const uint16_t max_num[CONNECTION_ERR_MAX] = {MAX_SEND_ERR_NUM,
                                                         MAX_RECV_ERR_NUM
                                                        };
    connection->errs[type]++;

    if (connection->errs[type] >= max_num[type])
    {
        connection->errs[type] = 0;

        if (g_connection_err_notify)
        {
            g_connection_err_notify(connection->lwm2mH, type, connection->bootstrap_flag);
        }
    }
}


int connection_parse_host_ip(char *uri, char **parsed_host, char **parsed_port)
{
    char *host;
    char *port;
    char *defaultport;

    if (NULL == uri)
    {
        ATINY_LOG(LOG_INFO, "uri is NULL!!!");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    //ATINY_LOG(LOG_INFO, "uri is %s\n", uri);

    // parse uri in the form "coaps://[host]:[port]"

    if (0 == strncmp(uri, "coaps://", strlen("coaps://")))
    {
        host = uri + strlen("coaps://");
        defaultport = COAPS_PORT;
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://")))
    {
        host = uri + strlen("coap://");
        defaultport = COAP_PORT;
    }
    else
    {
        ATINY_LOG(LOG_INFO, "come here1!!!");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    port = strrchr(host, ':');

    if (NULL == port)
    {
        port = defaultport;
    }
    else
    {
        // remove brackets
        if ('[' == host[0])
        {
            host++;

            if (']' == *(port - 1))
            {
                *(port - 1) = 0;
            }
            else
            {
                ATINY_LOG(LOG_INFO, "come here2!!!");
                return COAP_500_INTERNAL_SERVER_ERROR;
            }
        }

        // split strings
        *port = 0;
        port++;
    }

    *parsed_host = host;
    *parsed_port = port;
    return COAP_NO_ERROR;
}

#ifdef LWM2M_BOOTSTRAP
void connection_striger_server_initiated_bs(connection_t *sessionH)
{
    (void)sessionH;
    (void)lwm2m_cmd_ioctl(LWM2M_CMD_TRIGER_SERVER_INITIATED_BS, NULL, 0, LWM2M_AL_OP_SERVERREBS, URI_TRIGER_SERVER_INITIATED_BS);
}
#endif

#ifdef WITH_DTLS
int connection_connect_dtls(connection_t *connP, security_instance_t *targetP, const char *host, const char *port, int client_or_server)
{
    int ret;
    dtls_shakehand_info_s info;
    dtls_establish_info_s establish_info;
    establish_info.psk_or_cert = VERIFY_WITH_PSK;
    establish_info.udp_or_tcp = MBEDTLS_NET_PROTO_UDP;
    establish_info.v.p.psk = (const unsigned char *)targetP->secretKey;
    establish_info.v.p.psk_len = targetP->secretKeyLen;
    establish_info.v.p.psk_identity = (const unsigned char *)targetP->publicIdentity;
    connP->net_context = (void *)dtls_ssl_new(&establish_info, client_or_server);

    if (NULL == connP->net_context)
    {
        ATINY_LOG(LOG_INFO, "connP->ssl is NULL in connection_create");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    (void) memset(&info, 0, sizeof(info));
    info.client_or_server = client_or_server;
    info.finish_notify = NULL;
    info.step_notify   = NULL;
    info.udp_or_tcp = MBEDTLS_NET_PROTO_UDP;
    info.psk_or_cert = VERIFY_WITH_PSK;
#ifdef LWM2M_BOOTSTRAP
    info.step_notify = (void(*)(void *))lwm2m_step_striger_server_initiated_bs;
    info.param = (void(*)(void *))connP;
#endif

    if (MBEDTLS_SSL_IS_CLIENT == client_or_server)
    {
        info.u.c.host = host;
        info.u.c.port = port;
        info.timeout = DTLS_UDP_CLIENT_SHAKEHAND_TIMEOUT;
    }
    else
    {
#ifdef LWM2M_BOOTSTRAP
        info.timeout = targetP->clientHoldOffTime;
        info.u.s.local_port = port;
        timer_init(&connP->server_triger_timer, LWM2M_TRIGER_SERVER_MODE_INITIATED_TIME, (void(*)(void *))connection_striger_server_initiated_bs, connP);
        timer_start(&connP->server_triger_timer);
#endif
    }

    ret = dtls_shakehand(connP->net_context, &info);
#ifdef LWM2M_BOOTSTRAP
    timer_stop(&connP->server_triger_timer);
#endif

    if (0 != ret)
    {
        ATINY_LOG(LOG_INFO, "ret is %d in connection_create", ret);
        dtls_ssl_destroy((mbedtls_ssl_context *)connP->net_context);
        connP->net_context = NULL;
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    connP->dtls_flag = true;
    return COAP_NO_ERROR;
}

#endif


typedef struct
{
    int fd;
} con_net_conext_t;

static void *__socket_connect(char *host, char *port, int is_server)
{
    con_net_conext_t *ret =  NULL;
    int fd = -1;
    struct sockaddr_in addr;
    struct hostent *entry = NULL;
    entry = sal_gethostbyname(host);

    if (!(entry && entry->h_addr_list[0] && (AF_INET == entry->h_addrtype)))
    {
        goto EXIT_GETIP;
    }

    ret = osal_malloc(sizeof(con_net_conext_t));

    if (NULL == ret)
    {
        goto EXIT_MEM;
    }

    fd = sal_socket(AF_INET, SOCK_DGRAM, 0);

    if (-1 == fd)
    {
        goto EXIT_SOCKET;
    }

    (void) memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    (void) memcpy(&addr.sin_addr.s_addr, entry->h_addr_list[0], sizeof(addr.sin_addr.s_addr));
    addr.sin_port = htons(atoi(port));

    if (is_server)
    {
        if (-1 == sal_bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
        {
            goto EXIT_BIND;
        }
    }
    else
    {
        if (-1 == sal_connect(fd, (struct sockaddr *)&addr, sizeof(addr)))
        {
            goto EXIT_CONNECT;
        }
    }

    ret->fd = fd;
    return ret;
EXIT_CONNECT:
EXIT_BIND:
    (void)sal_closesocket(fd);
EXIT_SOCKET:
    osal_free(ret);
    ret = NULL;
EXIT_MEM:
EXIT_GETIP:
    return ret;
}

static void __socket_free(con_net_conext_t  *context)
{
    if (NULL != context)
    {
        (void)sal_closesocket(context->fd);
        osal_free(context);
    }
}
#include <time.h>

static int __socket_recv_timeout(con_net_conext_t *ctx, unsigned char *buf, size_t len, int timeout)
{
    int fd;
    int ret = -1;
    struct timeval timedelay;

    if (NULL == ctx)
    {
        return ret;
    }

    fd = ctx->fd;
    timedelay.tv_sec = timeout / 1000;
    timedelay.tv_usec = (timeout % 1000) * 1000;

    ///< set the recv timeout
    if (0 != sal_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timedelay, sizeof(timedelay)))
    {
        return ret;  //could not support the rcv timeout
    }

    ret = sal_recv(fd, buf, len, 0);
    return ret;
}

static int __socket_send(con_net_conext_t *ctx, const unsigned char *buf, size_t len)
{
    int fd;
    int ret = -1;

    if (NULL == ctx)
    {
        return ret;
    }

    fd = ctx->fd;
    ret = sal_send(fd, buf, len, 0);
    return ret;
}


connection_t *connection_create(connection_t *connList,
                                lwm2m_object_t *securityObj,
                                int instanceId,
                                lwm2m_context_t *lwm2mH,
                                int client_or_server)
{
    connection_t *connP = NULL;
    char *host;
    char *port;
    security_instance_t *targetP;
    char *uri = NULL;
    connection_t *ret = NULL;
    ATINY_LOG(LOG_INFO, "now come into connection_create!!!");
    targetP = (security_instance_t *)LWM2M_LIST_FIND(securityObj->instanceList, instanceId);

    if ((NULL == targetP) || (NULL == targetP->uri))
    {
        return NULL;
    }

    if (LWM2M_IS_CLIENT == client_or_server)
    {
        uri = osal_strdup(targetP->uri);

        if (NULL == uri)
        {
            ATINY_LOG(LOG_INFO, "osal_strdup null!!!");
            goto fail;
        }

        if (COAP_NO_ERROR != connection_parse_host_ip(uri, &host, &port))
        {
            goto fail;
        }
    }
    else
    {
        host = NULL;
        port = (char *)((LWM2M_SECURITY_MODE_NONE != targetP->securityMode) ? COAPS_PORT : COAP_PORT);
    }

    connP = (connection_t *)lwm2m_malloc(sizeof(connection_t));

    if (NULL == connP)
    {
        ATINY_LOG(LOG_INFO, "connP is NULL!!!");
        goto fail;
    }

    (void) memset(connP, 0, sizeof(connection_t));
#ifdef WITH_DTLS

    if (LWM2M_SECURITY_MODE_NONE != targetP->securityMode)
    {
        if (connection_connect_dtls(connP, targetP, host, port, client_or_server) != COAP_NO_ERROR)
        {
            goto fail;
        }
    }
    else
#endif
    {
        // no dtls session
        connP->net_context = __socket_connect(host, port, client_or_server);

        if (LWM2M_IS_SERVER   == client_or_server)
        {
#ifdef LWM2M_BOOTSTRAP

            if (connP->net_context)
            {
                connection_striger_server_initiated_bs(connP);
                timer_init(&connP->server_triger_timer, LWM2M_TRIGER_SERVER_MODE_INITIATED_TIME, (void(*)(void *))connection_striger_server_initiated_bs, connP);
                timer_start(&connP->server_triger_timer);
            }

#endif
        }

        if (NULL == connP->net_context)
        {
            ATINY_LOG(LOG_INFO, "net_context is NULL in connection_create");
            goto fail;
        }

        connP->dtls_flag = false;
    }

    connP->next = connList;
    connP->securityObj = securityObj;
    connP->securityInstId = instanceId;
    connP->lwm2mH = lwm2mH;
    connP->bootstrap_flag = targetP->isBootstrap;
    ret = connP;
fail:

    if (uri)
    {
        lwm2m_free(uri);
    }

    if ((NULL == ret) && connP)
    {
        lwm2m_free(connP);
    }

    return ret;
}

void connection_free(connection_t *connP)
{
    if (NULL == connP)
    {
        return;
    }

#ifdef WITH_DTLS

    if (true == connP->dtls_flag)
    {
        dtls_ssl_destroy(connP->net_context);
    }
    else
#endif
    {
        __socket_free(connP->net_context);
    }

    return;
}


void *lwm2m_connect_server(uint16_t secObjInstID, void *userData, bool isServer)
{
    client_data_t *dataP;
    lwm2m_list_t *instance;
    connection_t *newConnP = NULL;
    lwm2m_object_t   *securityObj;
    dataP = (client_data_t *)userData;
    securityObj = dataP->securityObjP;
    ATINY_LOG(LOG_INFO, "Now come into Connection creation in lwm2m_connect_server %d.\n", isServer);
    instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);

    if (NULL == instance)
    {
        return NULL;
    }

    newConnP = connection_create(dataP->connList, securityObj, instance->id, dataP->lwm2mH,
                                 isServer ? LWM2M_IS_SERVER : LWM2M_IS_CLIENT);

    if (NULL == newConnP)
    {
        ATINY_LOG(LOG_INFO, "Connection creation failed.\n");
        return NULL;
    }

    ATINY_LOG(LOG_INFO, "Connection creation successfully in lwm2m_connect_server.\n");
    dataP->connList = newConnP;
    return (void *)newConnP;
}

void lwm2m_close_connection(void *sessionH, void *userData)
{
    client_data_t *app_data;
    connection_t *targetP;
    app_data = (client_data_t *)userData;
    targetP = (connection_t *)sessionH;
#ifdef LWM2M_BOOTSTRAP
    timer_stop(&targetP->server_triger_timer);
#endif

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
        connection_free(targetP);
        lwm2m_free(targetP);
    }
    else
    {
        connection_t *parentP;
        parentP = app_data->connList;

        while ((NULL != parentP) && parentP->next != targetP)
        {
            parentP = parentP->next;
        }

        if (NULL != parentP)
        {
            parentP->next = targetP->next;
            connection_free(targetP);
            lwm2m_free(targetP);
        }
    }

    return;
}


int lwm2m_buffer_recv(void *sessionH, uint8_t *buffer, size_t length, uint32_t timeout)
{
    connection_t *connP = (connection_t *) sessionH;
    int ret = -1;
    timeout *= 1000;
#ifdef WITH_DTLS

    if (true == connP->dtls_flag)
    {
        // security
        ret = dtls_read(connP->net_context, buffer, length, timeout);

        if (0 == ret)
        {
            inc_connection_stat(connP, CONNECTION_RECV_ERR);
        }
        else
        {
            connP->errs[CONNECTION_RECV_ERR] = 0;
        }
    }
    else
#endif
    {
        ret = __socket_recv_timeout(connP->net_context, buffer, length, timeout);

        if (0 == ret)
        {
            inc_connection_stat(connP, CONNECTION_RECV_ERR);
        }
        else
        {
            connP->errs[CONNECTION_RECV_ERR] = 0;
        }
    }

    return ret;
}

static bool connection_is_valid(void *user_data, void *session)
{
    client_data_t *data = (client_data_t *)user_data;
    connection_t *conn;

    if ((NULL == data) || (NULL == data->connList))
    {
        return false;
    }

    conn = data->connList;

    while (NULL != conn)
    {
        if (conn == session)
        {
            return true;
        }

        conn = conn->next;
    }

    return false;
}

uint8_t lwm2m_buffer_send(void *sessionH,
                          uint8_t *buffer,
                          size_t length,
                          void *userdata)
{
    connection_t *connP = (connection_t *) sessionH;
    int ret;

    /* should check the valid of the connection,because coap tranctions does not update the session */
    if ((NULL == connP) || (!connection_is_valid(userdata, sessionH)))
    {
        ATINY_LOG(LOG_INFO, "#> failed sending %lu bytes, missing connection\r\n", (unsigned long)length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    ATINY_LOG(LOG_INFO, "call connection_send in lwm2m_buffer_send, length is %d\n", length);
#ifdef WITH_DTLS

    if (true == connP->dtls_flag)
    {
        // security
        ret = dtls_write(connP->net_context, buffer, length);

        if (0 == ret)
        {
            inc_connection_stat(connP, CONNECTION_SEND_ERR);
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
        else
        {
            connP->errs[CONNECTION_SEND_ERR] = 0;
            return COAP_NO_ERROR;
        }
    }
    else
#endif
    {
        ret = __socket_send(connP->net_context, buffer, length);

        if (ret > 0)
        {
            connP->errs[CONNECTION_SEND_ERR] = 0;
            return COAP_NO_ERROR;
        }
        else
        {
            inc_connection_stat(connP, CONNECTION_SEND_ERR);
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
    }
}

bool lwm2m_session_is_equal(void *session1, void *session2, void *userData)
{
    return (session1 == session2);
}

void lwm2m_register_connection_err_notify(lwm2m_connection_err_notify_t nofiy)
{
    g_connection_err_notify = nofiy;
}

#ifdef LWM2M_BOOTSTRAP
void lwm2m_step_striger_server_initiated_bs(connection_t *sessionH)
{
    if (sessionH == NULL)
    {
        return;
    }

    timer_step(&sessionH->server_triger_timer);
}
void lwm2m_stop_striger_server_initiated_bs(connection_t *sessionH)
{
    if (sessionH == NULL)
    {
        return;
    }

    timer_stop(&sessionH->server_triger_timer);
}


bool lwm2m_is_sec_obj_uri_valid(uint16_t secObjInstID, void *userData)
{
    client_data_t *dataP;
    lwm2m_object_t   *securityObj;
    dataP = (client_data_t *)userData;
    securityObj = dataP->securityObjP;
    security_instance_t *targetP;
    targetP = (security_instance_t *)LWM2M_LIST_FIND(securityObj->instanceList, secObjInstID);
    return !(((NULL == targetP)
              || (NULL == targetP->uri)
              || ('\0' == targetP->uri[0])));
}
#endif


