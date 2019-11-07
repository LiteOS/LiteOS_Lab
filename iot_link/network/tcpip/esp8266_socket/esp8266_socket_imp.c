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
 *  2019-08-5 15:09    yuhengP     The first version
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include <sal.h>
#include <sal_imp.h>
#include <sal_define.h>
#include <sal_types.h>
#include <esp8266_socket_imp.h>
#include <at.h>
#include <link_misc.h>
#include <link_endian.h>
#include <osal.h>

#define cn_esp8266_cmd_timeout   (6*1000)
#define cn_esp8266_rcvindex      "\r\n+IPD"
#define cn_esp8266_cachelen      (1024 * 2)

typedef struct
{
    int sockfd;
    int domain;
    int type;
    int protocol;

    tag_ring_buffer_t esp8266_rcvring;
    unsigned char esp8266_rcvbuf[cn_esp8266_cachelen];

    unsigned int timeout;

    char oob_resp[1024];

    int isconnect;
}esp8266_sock_cb_t;

static esp8266_sock_cb_t s_esp8266_sock_cb;


typedef enum
{
    STA = 1,
    AP,
    STA_AP,
}enum_net_mode;

//esp8266 common at command
static bool_t esp8266_atcmd(const char *cmd,const char *index)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,NULL,0,cn_esp8266_cmd_timeout);
    if(ret >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//at command  with response
static bool_t esp8266_atcmd_response(const char *cmd,const char *index,char *buf, int len)
{
    int ret = 0;
    ret = at_command((unsigned char *)cmd,strlen(cmd),index,(char *)buf,len,cn_esp8266_cmd_timeout);
    if(ret >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static int esp8266_rcvdeal(void *args,void *msg,size_t len)
{
    int ret = 0;
    unsigned short datalen;
    char *data;
    char  *str;

    data = msg;
    if(len <strlen(cn_esp8266_rcvindex))
    {
        printf("%s:invalid frame:%d byte:%s\n\r",__FUNCTION__,len,(char *)data);
        return ret;
    }
    //now deal the data
    str = strstr((char *)data,",");
    if(NULL == str)
    {
        printf("%s:format error\n\r",__FUNCTION__);
        return ret; //format error
    }
    str++;

    //TODO: mux = 1

    datalen = 0;
    for (; *str <= '9' && *str >= '0' ;str++)
    {
        datalen = (datalen * 10 + (*str - '0'));
    }
    str++;

    //now this is data payload
    if(datalen > (cn_esp8266_cachelen - ring_buffer_datalen(&s_esp8266_sock_cb.esp8266_rcvring)))
    {
    	return 0;
    }

    if(s_esp8266_sock_cb.type == SOCK_DGRAM)
    {
    	ret = ring_buffer_write(&s_esp8266_sock_cb.esp8266_rcvring,(unsigned char *)&datalen,sizeof(datalen));
        ret = ring_buffer_write(&s_esp8266_sock_cb.esp8266_rcvring,(unsigned char *)str,datalen);
    }
    else if (s_esp8266_sock_cb.type == SOCK_STREAM)
    {
        ret = ring_buffer_write(&s_esp8266_sock_cb.esp8266_rcvring,(unsigned char *)str,datalen);
    }

    return ret;

}

static int esp8266_socket(int domain, int type, int protocol)
{
	s_esp8266_sock_cb.domain = domain;
	s_esp8266_sock_cb.type = type;
	s_esp8266_sock_cb.protocol = protocol;
	s_esp8266_sock_cb.sockfd = 0;

	return s_esp8266_sock_cb.sockfd;
}

static int __esp8266_bind(int fd, const struct sockaddr *addr, int addrlen)
{
	//not implemented for client
    return 0;
}

static int esp8266_listen(int fd, int backlog)
{
	//not implemented for client
    return 0;
}

static int __esp8266_accept(int fd, struct sockaddr *addr, int addrlen)
{
	//not implemented for client
    return 0;
}

static int __esp8266_connect(int fd, const struct sockaddr *addr, int addrlen)
{
    char cmd[64];
    int ret = -1;
    char *remote_ip;

    if(NULL != addr)
    {
        memset(cmd,0,64);
        //memset(remote_ip,0,16);

        const struct sockaddr_in *serv_addr = (const struct sockaddr_in *)addr;
        uint16_t remote_port = ntohs(serv_addr->sin_port);
        struct in_addr remote_ip_int = serv_addr->sin_addr;
        remote_ip = inet_ntoa(remote_ip_int);

        //TODO: mux = 1
        if(s_esp8266_sock_cb.type == SOCK_DGRAM)
        {
        	snprintf(cmd,64,"AT+CIPSTART=\"UDP\",\"%s\",%d\r\n",remote_ip,remote_port);
        }
        else if(s_esp8266_sock_cb.type == SOCK_STREAM)
        {
        	snprintf(cmd,64,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",remote_ip,remote_port);
        }
        else return ret;

        if(esp8266_atcmd(cmd,"OK"))
        {
        	s_esp8266_sock_cb.isconnect = 1;
        	ret = 0;
        }
    }
    return ret;
}

static int esp8266_send(int fd, const void *buf, int len, int flags)
{
	char cmd[64];
    int ret = -1;

    if(NULL != buf)
    {
        memset(cmd,0,64);
        memset(s_esp8266_sock_cb.oob_resp,0,1024);

        snprintf(cmd,64,"AT+CIPSEND=%d\r\n",len); //TODO:mux = 1

        if(esp8266_atcmd(cmd,">"))
        {
        	ret = at_command((unsigned char *)buf,len,"SEND OK",(char *)s_esp8266_sock_cb.oob_resp,1024,cn_esp8266_cmd_timeout);
        	printf("ret = %d\r\n",ret);
        	char *str;
        	str = strstr(s_esp8266_sock_cb.oob_resp,cn_esp8266_rcvindex);  //in some cases, +IPD is not at the beginning of one frame. process here
        	if(NULL != str)
        	{
        		esp8266_rcvdeal(NULL,str,1024);
        	}

        }
    }
    if(ret > 0)
    {
    	ret = len;
    }
    return ret;
}
static int __esp8266_sendto(int fd, const void *msg, int len, int flags, struct sockaddr *addr, int addrlen)
{
	if (!s_esp8266_sock_cb.isconnect)
	{
		__esp8266_connect(fd,addr,addrlen);
	}
    return esp8266_send(fd,msg,len,flags);
}

static int esp8266_recv(int fd,void *buf,size_t len,int flags)
{
	int ret = -1;
	unsigned int timeout = 0;

	timeout = s_esp8266_sock_cb.timeout;
	do{
        if(s_esp8266_sock_cb.type == SOCK_DGRAM)
        {
            unsigned short framelen = 0;
            ret = ring_buffer_read(&s_esp8266_sock_cb.esp8266_rcvring,(unsigned char *)&framelen,sizeof(framelen));
            ret = ring_buffer_read(&s_esp8266_sock_cb.esp8266_rcvring,(unsigned char *)buf,framelen);
            if(ret > 0)
            {
                break;
            }
        }
        else if (s_esp8266_sock_cb.type == SOCK_STREAM)
        {
            ret = ring_buffer_read(&s_esp8266_sock_cb.esp8266_rcvring,(unsigned char *)buf,len);
            if(ret > 0)
            {
                break;
            }
        }
        if(timeout > 0)
        {
            osal_task_sleep(1);
            timeout--;
        }
	}while(timeout > 0);


	if(0 == ret)
	{
		return -1;
	}
	else return ret;
}

static int __esp8266_recvfrom(int fd, void *msg, int len, int flags, struct sockaddr *addr, socklen_t *addrlen)
{
    return esp8266_recv(fd,msg,len,flags);
}


static int __esp8266_setsockopt(int fd, int level, int option, const void *option_value, int option_len)
{
	//TODO:MUX=1
	struct timeval *timedelay = (struct timeval *)option_value;
	s_esp8266_sock_cb.timeout = (timedelay->tv_sec * 1000) + (timedelay->tv_usec / 1000);
    return 0;
}

static int esp8266_getsockopt(int fd, int level, int optname, const void *optval, int optlen)
{
	//not implemented
    return 0;
}

static int esp8266_close(int fd)
{
	char cmd[64];
	int ret = -1;
	memset(cmd,0,64);
	snprintf(cmd,64,"AT+CIPCLOSE\r\n");//TODO: MUX = 1;
	if(esp8266_atcmd(cmd,"OK"))
	{
		s_esp8266_sock_cb.isconnect = 0;
    	ret = 0;
    }
	return ret;
}

static int esp8266_shutdown(int fd, int how)
{
	return esp8266_close(fd);
}

static int esp8266_getsockname(int fd, struct sockaddr *addr,socklen_t *addrlen)
{
    //not implemented
    return 0;
}

static int esp8266_getpeername(int fd, struct sockaddr *addr,socklen_t *addrlen)
{
    //not implemented
    return 0;
}

///< here we must make sure that the hostent returned by gethostbyname won't be freeed, and
///< this data structure must be a static one, so we set it as a local global one

static struct hostent s_esp8266_hostent;
static uint8_t        s_esp8266_ipv4_addr[4];
static char          *s_esp8266_ipv4_lst[2];    ///< must be 2 or more because it terminated by NULL
static struct hostent *esp8266_gethostbyname(const char *name)
{
	char cmd[64];
	char resp[64];

	char *str;
	int ipv4[4];
	int fmt_num;

	struct hostent *hptr = NULL;

	memset(cmd,0,64);
	snprintf(cmd,64,"AT+CIPDOMAIN=\"%s\"\r\n",name);
	if(false == esp8266_atcmd_response(cmd,"+CIPDOMAIN",resp,64))
	{
	    hptr = &s_esp8266_hostent;
	    memset(hptr,0,sizeof(struct hostent));
	    ///< we could not resolve it by the at device, so we check if your address if dot point
        fmt_num = sscanf(name,"%d.%d.%d.%d",&ipv4[0],&ipv4[1],&ipv4[2],&ipv4[3]);
        if(fmt_num > 0)
        {
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

        }
        else
        {
            return NULL;
        }
	}
	else  ///< decode it by the response
	{
	    str = strstr(resp,":");
	    str++;

        hptr = &s_esp8266_hostent;
        memset(hptr,0,sizeof(struct hostent));
        ///< we could not resolve it by the at device, so we check if your address if dot point
        fmt_num = sscanf(str,"%d.%d.%d.%d",&ipv4[0],&ipv4[1],&ipv4[2],&ipv4[3]);
        if(fmt_num > 0)
        {
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
        }
        else
        {
            return NULL;
        }
	}
}

static const tag_tcpip_ops s_tcpip_socket_ops =
{
   .socket = (fn_sal_socket)esp8266_socket,
   .bind = (fn_sal_bind)__esp8266_bind,
   .listen = (fn_sal_listen)esp8266_listen,
   .connect = (fn_sal_connect)__esp8266_connect,
   .accept = (fn_sal_accept)__esp8266_accept,
   .send = (fn_sal_send)esp8266_send,
   .sendto = (fn_sal_sendto)__esp8266_sendto,
   .recv = (fn_sal_recv)esp8266_recv,
   .recvfrom = (fn_sal_recvfrom)__esp8266_recvfrom,
   .setsockopt = (fn_sal_setsockopt)__esp8266_setsockopt,
   .getsockopt = (fn_sal_getsockopt)esp8266_getsockopt,
   .shutdown =(fn_sal_shutdown)esp8266_shutdown,
   .closesocket =(fn_sal_closesocket)esp8266_close,
   .getpeername =(fn_sal_getpeername)esp8266_getpeername,
   .getsockname = (fn_sal_getsockname)esp8266_getsockname,
   .gethostbyname = (fn_sal_gethostbyname)esp8266_gethostbyname,
};

static const tag_tcpip_domain s_tcpip_socket =
{
    .name = "esp8266 socket",
    .domain = AF_INET,
    .ops = &s_tcpip_socket_ops,
};


int tcpipstack_install_esp8266_socket(void)
{
    int ret = -1;

    ret = tcpip_sal_install(&s_tcpip_socket);

    if(0 == ret)
    {
        printf("sal:install socket success\r\n");
    }
    else
    {
        printf("sal:install socket failed\r\n");
    }

    return 0;
}

static bool_t esp8266_reset(void)
{
    return esp8266_atcmd("AT+RST\r\n","ready");
}

static bool_t esp8266_echo_off(void)
{
    return esp8266_atcmd("ATE0\r\n","OK");
}

static bool_t esp8266_show_dinfo(int flag)
{
	char cmd[64];
	memset(cmd,0,64);
	snprintf(cmd,64,"AT+CIPDINFO=%d\r\n",flag);
    return esp8266_atcmd(cmd,"OK");
}

static bool_t esp8266_set_mode(enum_net_mode mode)
{
	char cmd[64];
	memset(cmd,0,64);
	snprintf(cmd,64,"AT+CWMODE_CUR=%d\r\n",(int)mode);
	return esp8266_atcmd(cmd,"OK");
}

static bool_t esp8266_joinap(char *ssid, char *passwd)
{
	char cmd[64];
	memset(cmd,0,64);
    snprintf(cmd,64,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid, passwd);
    return esp8266_atcmd(cmd,"OK");
}

static bool_t esp8266_set_mux(int mux)
{
	char cmd[64];
	memset(cmd,0,64);
    snprintf(cmd,64,"AT+CIPMUX=%d\r\n",mux);
    return esp8266_atcmd(cmd,"OK");
}

int esp8266_boot(void)
{
    at_oobregister("esp8266rcv",cn_esp8266_rcvindex,strlen(cn_esp8266_rcvindex),esp8266_rcvdeal,NULL);
    ring_buffer_init(&s_esp8266_sock_cb.esp8266_rcvring,s_esp8266_sock_cb.esp8266_rcvbuf,cn_esp8266_cachelen,0,0);


    esp8266_reset();
    esp8266_echo_off();
    esp8266_show_dinfo(0);
    esp8266_set_mode(STA);
    esp8266_set_mux(0);

    while(false == esp8266_joinap(WIFI_SSID, WIFI_PASSWD))
    {
        printf("connect ap failed, repeat...\r\n");
    }
   //reach here means everything is ok, we can go now

    return 0;
}



