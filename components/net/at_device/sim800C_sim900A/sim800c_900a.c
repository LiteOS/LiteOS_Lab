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

#define AT_CMD_AT    		"AT"
#define AT_CMD_CPIN         "AT+CPIN?\r"//check sim card
#define AT_CMD_COPS         "AT+COPS?"//check register network
#define AT_CMD_CLOSE    	"AT+CIPCLOSE"
#define AT_CMD_SHUT    		"AT+CIPSHUT\r"
#define AT_CMD_ECHO_OFF 	"ATE0\r"
#define AT_CMD_ECHO_ON  	"ATE1\r"
#define AT_CMD_MUX_ON       "AT+CIPMUX=1\r"
#define AT_CMD_CLASS        "AT+CGCLASS=\"B\"\r"//set MS type
#define AT_CMD_PDP_CONT   	"AT+CGDCONT=1,\"IP\",\"CMNET\"\r"//configure pdp context

#define AT_CMD_PDP_ATT    	"AT+CGATT=1\r"//pdp attach network
#define AT_CMD_PDP_ATT_Q    "AT+CGATT?\r"//pdp attach network
#define AT_CMD_PDP_ACT		"AT+CGACT=1,1\r"//active pdp context
#define AT_CMD_CSTT			"AT+CSTT\r"//start task
#define AT_CMD_CIICR		"AT+CIICR\r"//start gprs connect
#define AT_CMD_CIFSR		"AT+CIFSR\r"//get local ip
#define AT_CMD_CIPHEAD		"AT+CIPHEAD=1\r"
#define AT_CMD_CONN			"AT+CIPSTART"
#define AT_CMD_SEND			"AT+CIPSEND"

#define AT_DATAF_PREFIX      "\r\n+IPD"
#define AT_DATAF_PREFIX_MULTI      "\r\n+RECEIVE"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <osport.h>
#include <at.h>


#define cn_socket_max                         9
#define cn_socket_buflen                      768
struct socket_item
{
    char host[24];
    char port[8];
    char proto[4];
    semp_t   rcvsync;
    mutex_t  mutex;
    u8_t     rcvbuf[cn_socket_buflen];
    s32_t    rcvdatalen;
};
static struct socket_item *s_socket_cb[cn_socket_max];

//not support tcp yet
static s32_t socket_connect(const char* host, const char* port, int proto)
{
    u8_t cmdbuf[64];
    u8_t respbuf[64];
    s32_t ret = 0;
    s32_t resplen = 0;
    struct socket_item *item;
   
    for(ret = 0;ret<cn_socket_max;ret++ )
    {
        if(NULL == s_socket_cb[ret])
        {
            break;
        }
    }
    if(ret == cn_socket_max)
    {
        ret = -1;
        return ret;
    }
    
    item = malloc(sizeof(struct socket_item));
    if(NULL == item)
    {
        printf("%s:malloc error\n\r",__FUNCTION__);
        goto EXIT_MALLOC;
    }
    if(false == semp_create(&item->rcvsync,1,0))
    {
        printf("%s:semp create error\n\r",__FUNCTION__);
        goto EXIT_SEMP;  
    }
    if(false == mutex_create(&item->mutex))
    {
        printf("%s:mutex create error\n\r",__FUNCTION__);
        goto EXIT_MUTEX;  
    }
    strncpy(item->host,host,24);
    strncpy(item->port,port,8);
    
    
    //do some settings here
    at_command((u8_t *)AT_CMD_SHUT,strlen((char *)AT_CMD_SHUT),"OK",respbuf,64,1000);    
        
    at_command((u8_t *)AT_CMD_CLASS,strlen((char *)AT_CMD_CLASS),"OK",respbuf,64,1000);    
        
    at_command((u8_t *)AT_CMD_PDP_CONT,strlen((char *)AT_CMD_PDP_CONT),"OK",respbuf,64,1000); 

    at_command((u8_t *)AT_CMD_PDP_ATT,strlen((char *)AT_CMD_PDP_ATT),"OK",respbuf,64,1000);    
    
    at_command((u8_t *)AT_CMD_CIPHEAD,strlen((char *)AT_CMD_CIPHEAD),"OK",respbuf,64,1000);    
    
    at_command((u8_t *)AT_CMD_PDP_ACT,strlen((char *)AT_CMD_PDP_ACT),"OK",respbuf,64,1000);  

    at_command((u8_t *)AT_CMD_CSTT,strlen((char *)AT_CMD_CSTT),"OK",respbuf,64,1000);    

    at_command((u8_t *)AT_CMD_CIICR,strlen((char *)AT_CMD_CIICR),"OK",respbuf,64,1000);  

    at_command((u8_t *)AT_CMD_CIFSR,strlen((char *)AT_CMD_CIFSR),"OK",respbuf,64,1000);    
       
    
    //anyway ,we should create a hard socket here
    //close the hard socket
 #if 0   
    memset(cmdbuf,0,64);
    memset(respbuf,0,64);
    snprintf((char *)cmdbuf,64,"%s=%d\r",AT_CMD_CLOSE,ret);
    resplen = at_command(cmdbuf,strlen((char *)cmdbuf),"OK",respbuf,64,1000);
 #endif   
    memset(cmdbuf,0,64);
    memset(respbuf,0,64);
    snprintf((char *)cmdbuf,64,"%s=%d,\"%s\",\"%s\",\"%s\"\r", AT_CMD_CONN, ret, "UDP", host, port);

    resplen = at_command(cmdbuf,strlen((const char *)cmdbuf),"CONNECT OK",respbuf,64,10000);
    if(resplen <= 0)
    {
        printf("%s:at create socket command timeout\n\r",__FUNCTION__);
        goto EXIT_AT;
    }

    //reach here means create ok
    s_socket_cb[ret] = item;
    return ret;

EXIT_AT:
    mutex_del(&item->mutex);
EXIT_MUTEX:
    semp_del(&item->rcvsync);        
EXIT_SEMP:    
    free(item);
    item = NULL;
EXIT_MALLOC:
    ret = -1;
    return ret;
}

//close the socket we create ,remember to free the firmware socket
static  void socket_close(s32_t fd)
{
    s32_t  resplen;
    u8_t cmdbuf[64];
    u8_t respbuf[64];
    
    struct socket_item *item;
    if((fd >=0)&&(fd < cn_socket_max)&&(NULL != s_socket_cb[fd]))
    {
        item =  s_socket_cb[fd];
        s_socket_cb[fd] = NULL;
        semp_del(&item->rcvsync); 
        mutex_del(&item->mutex);       
        free(item);
        //close the hard socket
        memset(respbuf, 0,64);
        memset(cmdbuf, 0,64);
        snprintf((char *)cmdbuf,64,"%s=%d\r",AT_CMD_CLOSE,fd);
        resplen = at_command(cmdbuf,strlen((char *)cmdbuf),"OK",respbuf,64,1000);
        if(resplen > 0)
        {
            printf("close socket OK\n\r");
        }
    }
}

//use the socket to send data,the server has been set by connect
static s32_t socket_send(s32_t fd, u8_t *buf,s32_t len)
{
    s32_t  ret = 0;
    s32_t  resplen;
    u8_t respbuf[64];
    u8_t cmdbuf[64];
    struct socket_item *item;

    if((fd <0)||(fd >= cn_socket_max)||(NULL == s_socket_cb[fd]))
    {
        return ret;
    }
     
    item =  s_socket_cb[fd];
    if(mutex_lock(item->mutex))
    {
        memset(respbuf, 0,64);
        memset(cmdbuf,0,64);
        snprintf((char *)cmdbuf,64,"%s=%d,%d\r",AT_CMD_SEND,fd,len);

        resplen = at_command(cmdbuf,strlen((const char *)cmdbuf),">",respbuf,64,1000);
        if(resplen > 0)
        {
            memset(respbuf, 0,64);
            resplen = at_command(buf,len,"SEND OK",respbuf,64,1000);
            if(resplen > 0)
            {
                ret = len;
            }
        }
        mutex_unlock(item->mutex);
    }

    return ret;
}

//use  socket to do the receive here
static s32_t socket_receive(s32_t fd, u8_t *buf,s32_t len,u32_t timeout)
{
    s32_t  ret = 0;

    struct socket_item *item;

    if((fd <0)||(fd >= cn_socket_max)||(NULL == s_socket_cb[fd]))
    {
        return ret;
    }
    item =  s_socket_cb[fd]; 
    
    if(semp_pend(item->rcvsync,timeout))
    {
        if(mutex_lock(item->mutex))
        {
            ret = item->rcvdatalen > len?len:item->rcvdatalen;
            memcpy(buf,item->rcvbuf,ret);
            item->rcvdatalen = 0;
            mutex_unlock(item->mutex);
        }
    }
    return ret;
}

//match the data to the socket here
static void  __push_data(s32_t fd, u8_t *data, s32_t datalen)
{
    u8_t sockhost[64];
    struct socket_item *item;
    s32_t cpylen;
    if((fd < cn_socket_max)&&(NULL != s_socket_cb[fd]))
    {   
        item = s_socket_cb[fd];
        snprintf((char *)sockhost,64,"%s:%s",item->host,item->port);
        if(mutex_lock(item->mutex))
        {
            cpylen = datalen > cn_socket_buflen?cn_socket_buflen:datalen;
            memcpy(item->rcvbuf,data,cpylen);
            item->rcvdatalen = cpylen;

            mutex_unlock(item->mutex);
            semp_post(item->rcvsync);
        }
    }
}
//format:       RECV FROM:47.106.88.116:8888 IPD30  AAAAA
//this function used to resgiter to the at for oob monitor
static s32_t handler_rcvdata(u8_t *buf,s32_t len)
{
    s32_t  ret = 0;
    char  *cmpaddr;
    u8_t  *data;
    s32_t datalen = 0;
    const char *checkaddr;
    s32_t  fd;
    
    checkaddr = (const char *)buf;
    cmpaddr = strstr(checkaddr,":");
    if(NULL == cmpaddr)
    {
        return ret;
    }
    *cmpaddr = '\0';
    
    sscanf(checkaddr,"\r\n+RECEIVE,%d,%d",&fd,&datalen);
    data = (u8_t *)(cmpaddr + 3);  //jump the ":\r\n"
    
    if((u32_t)(buf + len) < (u32_t)(data + datalen)) //if not equel,which means something wrong happened to the frame
    {
        return ret;
    }
    
    //push the data to the socket
    __push_data(fd,data,datalen);
    ret = len;

    return ret;
}

#include <sal/atiny_socket.h>
static const struct atiny_netdevice  s_sim800c_900a=
{
    .connect = socket_connect,
    .close = socket_close,
    .snd = socket_send,
    .rcv  = socket_receive,
};

//the sim5320e device init function here
bool_t  sim800c_900a_init(void)
{
    bool_t ret = false;
    
    //each time reset the module
    
    while(1)
    {
           
        task_sleepms(1000*5);
        at_command((u8_t *)AT_CMD_SHUT,strlen(AT_CMD_SHUT),"OK",NULL,0,1000);
        if(at_command((u8_t *)AT_CMD_ECHO_OFF,strlen(AT_CMD_ECHO_OFF),"OK",NULL,0,1000)  == 0)
        {
            printf("echo close err,do continue...\n\r");
            continue;
        }
        else
        {
            printf("echo close success\n\r");
        }
        
        if(at_command((u8_t *)AT_CMD_CPIN,strlen(AT_CMD_CPIN),"OK",NULL,0,1000)  == 0)
        {
            printf("sim card not detected,continue to detect...\n\r");
            continue;
        }
        else
        {
            printf("cpin check success\n\r");
        }
        
        if(at_command((u8_t *)AT_CMD_PDP_ATT_Q,strlen(AT_CMD_PDP_ATT_Q),"+CGATT: 1",NULL,0,1000)  == 0)
        {
            printf("net work not detected,continue to detect...\n\r");
            continue;
        }
        else
        {
            printf("cgatt check success\n\r");
        }
        
        at_command((u8_t *)AT_CMD_MUX_ON,strlen(AT_CMD_MUX_ON),"OK",NULL,0,1000);
            
        //reach here means all the detect has passed
        ret = true;
        break;
    }
    
    atiny_install_netdevice((struct atiny_netdevice *)&s_sim800c_900a);
    at_oobregister(handler_rcvdata,"\r\n+RECEIVE,");
    return ret;
}

#include <shell.h>

static bool_t shell_connect(s32_t argc, const char *argv[])
{
    s32_t ret ;
    ret = socket_connect(argv[1],argv[2],17);
    return ret;
}
OSSHELL_EXPORT_CMD(shell_connect,"connect","connect");


