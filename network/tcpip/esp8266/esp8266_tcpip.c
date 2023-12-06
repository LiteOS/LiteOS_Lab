/* ----------------------------------------------------------------------------
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
 * --------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 * --------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sal.h>
#include <sal_imp.h>
#include <sal_define.h>
#include <sal_types.h>
#include <at.h>
#include <link_misc.h>
#include <link_endian.h>
#include <osal.h>
#include <iot_link_config.h>

// /<anyway, you should do the config for the wifi
#ifndef CONFIG_ESP8266_SSID
#define CONFIG_ESP8266_SSID "IoTdebug"
#endif
#ifndef CONFIG_ESP8266_PWD
#define CONFIG_ESP8266_PWD "IoTdebug"
#endif
#ifndef CONFIG_ESP8266_RCVCACHE
#define CONFIG_ESP8266_RCVCACHE (1024)
#endif
#ifndef CONFIG_ESP8266_CMDTIMEOUT
#define CONFIG_ESP8266_CMDTIMEOUT (2 * 1000)
#endif

#define CN_ESP8266_SOCKMAX 5
#define CN_ESP8266_RCVINDEX "\r\n+IPD"
#define CN_STACK_STRMAX 128

typedef enum {
    STA = 1,
    AP,
    STA_AP,
} enum_net_mode;
//  this is the mux==1 mode
typedef struct {
    int sockfd;
    int domain;
    int type;
    int protocol;

    int isConnect;
    unsigned int timeout;
    tag_ring_buffer_t rcvRing;

    char oob_resp[CONFIG_ESP8266_RCVCACHE];
    unsigned char esp8266_rcvbuf[CONFIG_ESP8266_RCVCACHE];
} Esp8266Sock_t;

typedef struct {
    osal_mutex_t atLock;
    osal_mutex_t ioLock;
    int flagSendOKGetByOOB : 1;    //  "The Send OK  appending by the oob"
    int flagSendIndexGetByOOB : 1; //  "The > appending by the oob "
    Esp8266Sock_t *sockArray[CN_ESP8266_SOCKMAX];
} Esp8266CB_t;
static Esp8266CB_t gEsp8266CB; //  This is the esp8266 control block here

static int Esp8266SockMalloc(Esp8266Sock_t *sock)
{
    //  try to get one
    int fd = -1;
    if (osal_mutex_lock(gEsp8266CB.atLock)) {
        for (int i = 0; i < CN_ESP8266_SOCKMAX; i++) {
            if (gEsp8266CB.sockArray[i] == NULL) {
                fd = i;
                sock->sockfd = fd;
                gEsp8266CB.sockArray[i] = sock;
                break;
            }
        }

        osal_mutex_unlock(gEsp8266CB.atLock);
    }
    return fd;
}

static void Esp8266SockFree(int fd)
{
    //  try to release one
    if (osal_mutex_lock(gEsp8266CB.atLock)) {
        gEsp8266CB.sockArray[fd] = NULL;
        osal_mutex_unlock(gEsp8266CB.atLock);
    }
    return;
}

static Esp8266Sock_t *Esp8266SockGet(int fd)
{
    return gEsp8266CB.sockArray[fd];
}

// esp8266 common at command
static bool_t esp8266_atcmd(const char *cmd, const char *index)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd, strlen(cmd), index, NULL, 0, CONFIG_ESP8266_CMDTIMEOUT);
    if (ret >= 0) {
        return true;
    } else {
        return false;
    }
}

// at command  with response
static bool_t esp8266_atcmd_response(const char *cmd, const char *index, char *buf, int len)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd, strlen(cmd), index, (char *)buf, len, CONFIG_ESP8266_CMDTIMEOUT);
    if (ret >= 0) {
        return true;
    } else {
        return false;
    }
}

//  dataformat: +IPD,FD,LEN:PAYLOAD
static char *Esp8266MsgDealSingle(char *msg, size_t len)
{
    unsigned short datalen;
    char *data;
    char *str;
    char *ret = NULL;

    Esp8266Sock_t *sock;
    int fd = 0;

    data = msg;
    ret = msg + len;
    if (len < strlen(CN_ESP8266_RCVINDEX)) {
        LINK_LOG_ERROR("invalid frame:%d byte:%s", len, (char *)data);
        return ret;
    }
    //  get the fd;
    str = strstr((char *)data, ",");
    if (NULL == str) {
        LINK_LOG_ERROR("format error:CAN'T FIND ',fd'");
        return ret; // format error
    }
    str++;
    for (; *str <= '9' && *str >= '0'; str++) {
        fd = (fd * 10 + (*str - '0'));
    }
    if (fd >= CN_ESP8266_SOCKMAX) {
        return ret;
    }
    sock = Esp8266SockGet(fd);
    if (NULL == sock) {
        return ret;
    }
    // now deal the datalen
    str = strstr((char *)str, ",");
    if (NULL == str) {
        LINK_LOG_ERROR("format error:CAN'T FIND ',LEN'");
        return ret; // format error
    }
    str++;
    datalen = 0;
    for (; *str <= '9' && *str >= '0'; str++) {
        datalen = (datalen * 10 + (*str - '0'));
    }
    str++; // /<SKIP OVER THE ":"

    // now this is data payload
    if (datalen > (CONFIG_ESP8266_RCVCACHE - ring_buffer_datalen(&sock->rcvRing))) {
        return ret;
    }
    ret = str + datalen;
    if (sock->type == SOCK_DGRAM) {
        ring_buffer_write(&sock->rcvRing, (unsigned char *)&datalen, sizeof(datalen));
        ring_buffer_write(&sock->rcvRing, (unsigned char *)str, datalen);
    } else if (sock->type == SOCK_STREAM) {
        ring_buffer_write(&sock->rcvRing, (unsigned char *)str, datalen);
    }
    ret = str + datalen;

    return ret;
}

static int Esp8266MsgDeal(void *args, void *msg, size_t len)
{
    char *dataNxt = NULL;
    int dataLeft;

    dataNxt = (char *)msg;
    dataLeft = len;

    while (((uintptr_t)(dataNxt)) < ((uintptr_t)msg + len)) {
        if (osal_mutex_lock(gEsp8266CB.ioLock)) {
            dataNxt = Esp8266MsgDealSingle(dataNxt, dataLeft);
            osal_mutex_unlock(gEsp8266CB.ioLock);
        }
        dataLeft = (uintptr_t)msg + len - (uintptr_t)dataNxt;
        if (dataLeft < strlen(CN_ESP8266_RCVINDEX)) {
            break;
        }
        if (0 != memcmp(dataNxt, CN_ESP8266_RCVINDEX, strlen(CN_ESP8266_RCVINDEX))) {
            break;
        }
    }
    if (dataLeft > 0) {
        if (NULL != strstr(dataNxt, ">")) {
            gEsp8266CB.flagSendIndexGetByOOB = 1;
        }

        if (NULL != strstr(dataNxt, "SEND OK")) {
            gEsp8266CB.flagSendOKGetByOOB = 1;
        }
    }
    return len;
}

static int Esp8266Socket(int domain, int type, int protocol)
{
    int fd = -1;
    Esp8266Sock_t *sock;

    sock = osal_malloc(sizeof(Esp8266Sock_t));
    if (NULL == sock) {
        return fd;
    }
    ring_buffer_init(&sock->rcvRing, sock->esp8266_rcvbuf, CONFIG_ESP8266_RCVCACHE, 0, 0);
    sock->domain = domain;
    sock->type = type;
    sock->protocol = protocol;

    fd = Esp8266SockMalloc(sock);
    if (fd == -1) {
        osal_free(sock);
    }

    return fd;
}

static int Esp8266Bind(int fd, const struct sockaddr *addr, int addrlen)
{
    // not implemented for client
    return -1;
}

static int Esp8266Listen(int fd, int backlog)
{
    // not implemented for client
    return -1;
}

static int Esp8266Accept(int fd, struct sockaddr *addr, int addrlen)
{
    // not implemented for client
    return -1;
}

static int Esp8266Connect(int fd, const struct sockaddr *addr, int addrlen)
{
    char cmd[CN_STACK_STRMAX];
    int ret = -1;
    char *ip;
    Esp8266Sock_t *sock;
    if ((fd < 0) || (fd >= CN_ESP8266_SOCKMAX) || ((sock = Esp8266SockGet(fd)) == NULL) || (addr == NULL)) {
        return ret;
    }
    const struct sockaddr_in *serv_addr = (const struct sockaddr_in *)addr;
    uint16_t serverPort = ntohs(serv_addr->sin_port);
    struct in_addr serverIp = serv_addr->sin_addr;
    ip = inet_ntoa(serverIp);

    if (sock->type == SOCK_DGRAM) {
        (void)snprintf(cmd, CN_STACK_STRMAX, "AT+CIPSTART=%d,\"UDP\",\"%s\",%d\r\n", fd, ip, serverPort);
    } else if (sock->type == SOCK_STREAM) {
        (void)snprintf(cmd, CN_STACK_STRMAX, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d\r\n", fd, ip, serverPort);
    } else {
        return ret;
    }

    if (osal_mutex_lock(gEsp8266CB.atLock)) {
        if (esp8266_atcmd(cmd, "OK")) {
            sock->isConnect = 1;
            ret = 0;
        }
        osal_mutex_unlock(gEsp8266CB.atLock);
    }
    return ret;
}

static int Esp8266Send(int fd, const void *buf, int len, int flags)
{
    int ret = -1;
    char cmd[CN_STACK_STRMAX];
    Esp8266Sock_t *sock;

    if ((fd < 0) || (fd >= CN_ESP8266_SOCKMAX) || ((sock = Esp8266SockGet(fd)) == NULL) || (buf == NULL) ||
        (len <= 0)) {
        return ret;
    }
    (void)memset(sock->oob_resp, 0, CONFIG_ESP8266_RCVCACHE);

    if (osal_mutex_lock(gEsp8266CB.atLock)) { //  this two at command must be executed in sequence in lock
        (void)snprintf(cmd, 64, "AT+CIPSEND=%d,%d\r\n", fd, len);
        gEsp8266CB.flagSendIndexGetByOOB = 0;
        if (esp8266_atcmd(cmd, ">") || gEsp8266CB.flagSendIndexGetByOOB) {
            gEsp8266CB.flagSendOKGetByOOB = 0;
            ret = at_command((unsigned char *)buf, len, "SEND OK", (char *)sock->oob_resp, CONFIG_ESP8266_RCVCACHE,
                CONFIG_ESP8266_CMDTIMEOUT);
            if (ret > 0) {
                char *str;
                str = strstr(sock->oob_resp,
                    CN_ESP8266_RCVINDEX); // in some cases, +IPD is not at the beginning of one frame. process here
                if (NULL != str) {
                    Esp8266MsgDeal(NULL, str, (char *)sock->oob_resp + ret - str);
                }
                ret = len;
            } else if (gEsp8266CB.flagSendOKGetByOOB) {
                ret = len;
            }
        }
        osal_mutex_unlock(gEsp8266CB.atLock);
    } else {
        LINK_LOG_ERROR("GET ATLOCK FAILED:fd:%d len:%d", fd, len);
    }
    if (ret <= 0) {
        ret = 0;
        sock->isConnect = 0; //  i think some errors happened, maybe lost
        LINK_LOG_ERROR("SEND FD:%d Len:%d Failed  and set it to disconnect", fd, len);
    }
    return ret;
}

static int Esp8266Sendto(int fd, const void *msg, int len, int flags, struct sockaddr *addr, int addrlen)
{
    int ret = -1;
    Esp8266Sock_t *sock;
    if ((fd < 0) || (fd >= CN_ESP8266_SOCKMAX) || ((sock = Esp8266SockGet(fd)) == NULL) || (msg == NULL) ||
        (len <= 0)) {
        return ret;
    }

    if (!sock->isConnect) {
        Esp8266Connect(fd, addr, addrlen);
    }
    return Esp8266Send(fd, msg, len, flags);
}

static int Esp8266Recv(int fd, void *buf, size_t len, int flags)
{
    int ret = -1;
    unsigned int timeout = 0;
    Esp8266Sock_t *sock;
    if ((fd < 0) || (fd >= CN_ESP8266_SOCKMAX) || ((sock = Esp8266SockGet(fd)) == NULL) || (buf == NULL) ||
        (len <= 0)) {
        return ret;
    }

    timeout = sock->timeout;
    do {
        if (osal_mutex_lock(gEsp8266CB.ioLock)) {
            if (ring_buffer_datalen(&sock->rcvRing) > 0) {
                if (sock->type == SOCK_DGRAM) {
                    unsigned short framelen = 0;
                    ret = ring_buffer_read(&sock->rcvRing, (unsigned char *)&framelen, sizeof(framelen));
                    ret = ring_buffer_read(&sock->rcvRing, (unsigned char *)buf, framelen);
                } else if (sock->type == SOCK_STREAM) {
                    ret = ring_buffer_read(&sock->rcvRing, (unsigned char *)buf, len);
                } else {
                }
            }
            osal_mutex_unlock(gEsp8266CB.ioLock);
        }
        if (ret > 0) {
            break;
        } else if (sock->isConnect == 0) {
            ret = 0;
            break;
        } else {
            osal_task_sleep(1);
        }
    } while ((timeout--) > 0);
    return ret;
}

static int Esp8266RecvFrom(int fd, void *msg, int len, int flags, struct sockaddr *addr, socklen_t *addrlen)
{
    return Esp8266Recv(fd, msg, len, flags);
}

static int Esp8266SetSockOpt(int fd, int level, int option, const void *option_value, int option_len)
{
    int ret = -1;
    Esp8266Sock_t *sock;
    if ((fd < 0) || (fd >= CN_ESP8266_SOCKMAX) || ((sock = Esp8266SockGet(fd)) == NULL)) {
        return ret;
    }

    struct timeval *timedelay = (struct timeval *)option_value;
    sock->timeout = (timedelay->tv_sec * 1000) + (timedelay->tv_usec / 1000);
    return 0;
}

static int Esp8266GetSockOpt(int fd, int level, int optname, const void *optval, int optlen)
{
    // not implemented
    return -1;
}

static int Esp8266Close(int fd)
{
    char cmd[CN_STACK_STRMAX];
    int ret = -1;
    Esp8266Sock_t *sock;
    if ((fd < 0) || (fd >= CN_ESP8266_SOCKMAX) || ((sock = Esp8266SockGet(fd)) == NULL)) {
        return ret;
    }

    (void)snprintf(cmd, 64, "AT+CIPCLOSE=%d\r\n", fd);

    if (osal_mutex_lock(gEsp8266CB.atLock)) {
        if (esp8266_atcmd(cmd, "OK")) {
            LINK_LOG_ERROR("ATCLOSE:%d FAILED", fd);
        }
        osal_mutex_unlock(gEsp8266CB.atLock);
    }
    Esp8266SockFree(fd);
    osal_free(sock);
    ret = 0;
    return ret;
}

static int Esp8266Shutdown(int fd, int how)
{
    return -1; //  not implement
}

static int Esp8266GetSockName(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    // not implemented
    return 0;
}

static int Esp8266GetPeerName(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    // not implemented
    return 0;
}

//  here we must make sure that the hostent returned by gethostbyname won't be freeed, and
//  this data structure must be a static one, so we set it as a local global one

static struct hostent gEsp8266HostEnt;
static uint8_t s_esp8266_ipv4_addr[4];
static char *s_esp8266_ipv4_lst[2]; //  must be 2 or more because it terminated by NULL
static struct hostent *Esp8266GetHostByName(const char *name)
{
    char cmd[CN_STACK_STRMAX];
    char resp[CN_STACK_STRMAX];

    char *str;
    int ipv4[4];
    int fmt_num;

    struct hostent *hptr = NULL;

    (void)memset(resp, 0, CN_STACK_STRMAX);
    (void)snprintf(cmd, CN_STACK_STRMAX, "AT+CIPDOMAIN=\"%s\"\r\n", name);
    if (false == esp8266_atcmd_response(cmd, "+CIPDOMAIN", resp, CN_STACK_STRMAX)) {
        hptr = &gEsp8266HostEnt;
        (void)memset(hptr, 0, sizeof(struct hostent));
        //  we could not resolve it by the at device, so we check if your address if dot point
        fmt_num = sscanf(name, "%d.%d.%d.%d", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]);
        if (fmt_num > 0) {
            hptr->h_addrtype = AF_INET;
            hptr->h_length = 4;
            s_esp8266_ipv4_addr[0] = ipv4[0];
            s_esp8266_ipv4_addr[1] = ipv4[1];
            s_esp8266_ipv4_addr[2] = ipv4[2];
            s_esp8266_ipv4_addr[3] = ipv4[3];

            s_esp8266_ipv4_lst[0] = (char *)&s_esp8266_ipv4_addr[0];
            s_esp8266_ipv4_lst[1] = NULL;

            hptr->h_addr_list = &s_esp8266_ipv4_lst[0];
            return hptr;
        } else {
            return NULL;
        }
    } else { //  decode it by the response
        str = strstr(resp, ":");
        str++;

        hptr = &gEsp8266HostEnt;
        (void)memset(hptr, 0, sizeof(struct hostent));
        fmt_num = sscanf(str, "%d.%d.%d.%d", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]);
        if (fmt_num > 0) {
            hptr->h_addrtype = AF_INET;
            hptr->h_length = 4;
            s_esp8266_ipv4_addr[0] = ipv4[0];
            s_esp8266_ipv4_addr[1] = ipv4[1];
            s_esp8266_ipv4_addr[2] = ipv4[2];
            s_esp8266_ipv4_addr[3] = ipv4[3];

            s_esp8266_ipv4_lst[0] = (char *)&s_esp8266_ipv4_addr[0];
            s_esp8266_ipv4_lst[1] = NULL;

            hptr->h_addr_list = &s_esp8266_ipv4_lst[0];
            return hptr;
        } else {
            return NULL;
        }
    }
}

static const tag_tcpip_ops s_tcpip_socket_ops = {
    .socket = (fn_sal_socket)Esp8266Socket,
    .bind = (fn_sal_bind)Esp8266Bind,
    .listen = (fn_sal_listen)Esp8266Listen,
    .connect = (fn_sal_connect)Esp8266Connect,
    .accept = (fn_sal_accept)Esp8266Accept,
    .send = (fn_sal_send)Esp8266Send,
    .sendto = (fn_sal_sendto)Esp8266Sendto,
    .recv = (fn_sal_recv)Esp8266Recv,
    .recvfrom = (fn_sal_recvfrom)Esp8266RecvFrom,
    .setsockopt = (fn_sal_setsockopt)Esp8266SetSockOpt,
    .getsockopt = (fn_sal_getsockopt)Esp8266GetSockOpt,
    .shutdown = (fn_sal_shutdown)Esp8266Shutdown,
    .closesocket = (fn_sal_closesocket)Esp8266Close,
    .getpeername = (fn_sal_getpeername)Esp8266GetPeerName,
    .getsockname = (fn_sal_getsockname)Esp8266GetSockName,
    .gethostbyname = (fn_sal_gethostbyname)Esp8266GetHostByName,
};

static const tag_tcpip_domain s_tcpip_socket = {
    .name = "esp8266 socket",
    .domain = AF_INET,
    .ops = &s_tcpip_socket_ops,
};


static bool_t esp8266_reset(void)
{
    return esp8266_atcmd("AT+RST\r\n", "ready");
}

static bool_t esp8266_echo_off(void)
{
    return esp8266_atcmd("ATE0\r\n", "OK");
}

static bool_t esp8266_show_dinfo(int flag)
{
    char cmd[64];
    (void)memset(cmd, 0, 64);
    (void)snprintf(cmd, 64, "AT+CIPDINFO=%d\r\n", flag);
    return esp8266_atcmd(cmd, "OK");
}

static bool_t esp8266_set_mode(enum_net_mode mode)
{
    char cmd[64];
    (void)memset(cmd, 0, 64);
    (void)snprintf(cmd, 64, "AT+CWMODE_CUR=%d\r\n", (int)mode);
    return esp8266_atcmd(cmd, "OK");
}

static bool_t esp8266_joinap(char *ssid, char *passwd)
{
    int ret;
    char cmd[CN_STACK_STRMAX];
    (void)snprintf(cmd, CN_STACK_STRMAX, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, passwd);
    ret = at_command((unsigned char *)cmd, strlen(cmd), "OK", NULL, 0, CONFIG_ESP8266_CMDTIMEOUT * 5);
    if (ret >= 0) {
        return true;
    } else {
        return false;
    }
}

static bool_t esp8266_set_mux(int mux)
{
    char cmd[64];
    (void)memset(cmd, 0, 64);
    (void)snprintf(cmd, 64, "AT+CIPMUX=%d\r\n", mux);
    return esp8266_atcmd(cmd, "OK");
}

int link_tcpip_imp_init(void)
{
    int ret = -1;

    if (false == osal_mutex_create(&gEsp8266CB.atLock)) {
        LINK_LOG_ERROR("AT MUTEX COULD NOT CREATED");
        goto EXIT_ATMUTEX;
    }
    if (false == osal_mutex_create(&gEsp8266CB.ioLock)) {
        LINK_LOG_ERROR("AT MUTEX COULD NOT CREATED");
        goto EXIT_IOMUTEX;
    }

    LINK_LOG_DEBUG("ESP8266 CONFIGURE:SSID:%s PWD:%s RCVCACHE:%d(Byte) CMDTIMEOUT:%d(ms)", CONFIG_ESP8266_SSID,
        CONFIG_ESP8266_PWD, CONFIG_ESP8266_RCVCACHE, CONFIG_ESP8266_CMDTIMEOUT);

    at_oobregister("esp8266rcv", CN_ESP8266_RCVINDEX, strlen(CN_ESP8266_RCVINDEX), Esp8266MsgDeal, NULL);

    do {
        LINK_LOG_DEBUG("Try to connect ap..");
        osal_task_sleep(1000);
        esp8266_reset();
        esp8266_echo_off();
        esp8266_show_dinfo(0);
        esp8266_set_mode(STA);
        esp8266_set_mux(1);
    } while (false == esp8266_joinap(CONFIG_ESP8266_SSID, CONFIG_ESP8266_PWD));
    at_debugclose();
    // reach here means everything is ok, we can go now
    ret = link_sal_install(&s_tcpip_socket);
    if (0 != ret) {
        LINK_LOG_ERROR("REGISTER ESP8266 TO TCPIP FAILED");
        goto EXIT_TCPIP;
    }
    return 0;


EXIT_TCPIP:
    osal_mutex_del(gEsp8266CB.ioLock);
EXIT_IOMUTEX:
    gEsp8266CB.ioLock = cn_mutex_invalid;
    osal_mutex_del(gEsp8266CB.atLock);
EXIT_ATMUTEX:
    gEsp8266CB.atLock = cn_mutex_invalid;
    return -1;
}
