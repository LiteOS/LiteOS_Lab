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

#include <stdlib.h>
#include <at.h>
//open the network
static bool_t __close_echo(void)
{
    bool_t ret = false;
    s32_t  resplen;
    const char *cmd = "ate0\r";
    u8_t respbuf[64];
    
    memset(respbuf, 0,64);
    resplen = at_command((u8_t *)cmd,strlen(cmd),"OK",respbuf,64,1000);
    if(resplen > 0)
    {
        ret = true;
    }
    return ret;
}


static bool_t __open_network(void)
{
    bool_t ret = false;
    s32_t  resplen;
    const char *cmd = "at+netopen\r";
    u8_t respbuf[64];
    
    memset(respbuf, 0,64);
    resplen = at_command((u8_t *)cmd,strlen(cmd),"OK",respbuf,64,1000);
    if(resplen > 0)
    {
        ret = true;
    }
    return ret;
}
static bool_t __csq_get(s32_t *signal,s32_t *ber)
{
    bool_t ret = false;
    s32_t  resplen;
    s32_t  argc = 6;
    char*  argv[6];
    const char *cmd = "at+csq\r";
    u8_t respbuf[64];
    
    memset(respbuf, 0,64);
    resplen = at_command((u8_t *)cmd,strlen(cmd),"+CSQ",respbuf,64,1000);
    if(resplen > 0)
    {
        argc = string_split((char *)respbuf,"\r\n",argv,argc);
        if((argc > 0)&&(0 == strcmp(argv[argc-1],"OK")))
        {
            sscanf(argv[argc-2],"+CSQ: %d,%d",signal,ber);
            ret = true;
        }
    }
    return ret;
}
static bool_t __cpin_check(void)  //return true ,read get else failed
{
    bool_t ret = false;
    s32_t  resplen;
    const char *cmd = "at+cpin?\r";
    u8_t respbuf[64];
    
    memset(respbuf, 0,64);
    resplen = at_command((u8_t *)cmd,strlen(cmd),"READY",respbuf,64,1000);
    if(resplen > 0)
    {             
        ret = true;
    }
    return ret;
}
static bool_t __cgatt_check(void)  //return true ,read get else failed
{
    bool_t ret = false;
    s32_t  resplen;
    const char *cmd = "at+cgatt?\r";
    u8_t respbuf[64];
        
    memset(respbuf, 0,64);
    resplen = at_command((u8_t *)cmd,strlen(cmd),"+CGATT: 1",respbuf,64,1000);
    if(resplen > 0)
    {
        ret = true;
    }
    return ret;
}

#define cn_5320e_socket_max 9
#define cn_5320e_socket_rcvlen 768
struct socket_item
{
    char host[24];
    char port[8];
    char proto[4];
    semp_t   rcvsync;
    mutex_t  mutex;
    u8_t     rcvbuf[cn_5320e_socket_rcvlen];
    s32_t    rcvdatalen;
};
static struct socket_item *g_socket_cb[cn_5320e_socket_max];

//not support tcp yet
static s32_t socket_connect(const char* host, const char* port, int proto)
{
    u8_t cmdbuf[64];
    u8_t respbuf[64];
    s32_t ret = 0;
    s32_t resplen = 0;
    struct socket_item *item;
    s32_t argc =4;
    char *argv[4];
   
    for(ret = 0;ret<cn_5320e_socket_max;ret++ )
    {
        if(NULL == g_socket_cb[ret])
        {
            break;
        }
    }
    if(ret == cn_5320e_socket_max)
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
    //anyway ,we should create a hard socket here
   //close the hard socket
    memset(cmdbuf,0,64);
    memset(respbuf,0,64);
    snprintf((char *)cmdbuf,64,"at+cipclose=%d\r",ret);
    resplen = at_command(cmdbuf,strlen((char *)cmdbuf),"OK",respbuf,64,1000);
    
    memset(cmdbuf,0,64);
    memset(respbuf,0,64);
    snprintf((char *)cmdbuf,64,"at+cipopen=%d,\"%s\",,,%d\r",ret,"udp",ret);
    resplen = at_command(cmdbuf,strlen((const char *)cmdbuf),"+CIPOPEN",respbuf,64,10000);
    if(resplen <= 0)
    {
        printf("%s:at create socket command timeout\n\r",__FUNCTION__);
        goto EXIT_AT;
    }
    argc = string_split((char *)respbuf,"\n\r",argv,argc);
    if((argc < 2)||(0 != strcmp(argv[argc -1],"OK")))
    {
        printf("%s:at create socket failed\n\r",__FUNCTION__);
        goto EXIT_AT;
    }
    
    //reach here means create ok
    g_socket_cb[ret] = item;
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
    if((fd >=0)&&(fd < cn_5320e_socket_max)&&(NULL != g_socket_cb[fd]))
    {
        item =  g_socket_cb[fd];
        g_socket_cb[fd] = NULL;
        semp_del(&item->rcvsync); 
        mutex_del(&item->mutex);       
        free(item);
        //close the hard socket
        memset(respbuf, 0,64);
        memset(cmdbuf, 0,64);
        snprintf((char *)cmdbuf,64,"at+cipclose=%d\r",fd);
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

    if((fd <0)||(fd >= cn_5320e_socket_max)||(NULL == g_socket_cb[fd]))
    {
        return ret;
    }
     

    item =  g_socket_cb[fd];
    if(mutex_lock(item->mutex))
    {
        memset(respbuf, 0,64);
        memset(cmdbuf,0,64);
        snprintf((char *)cmdbuf,64,"at+cipsend=%d,%d,\"%s\",%s\r",fd,len,item->host,item->port);

        resplen = at_command(cmdbuf,strlen((const char *)cmdbuf),">",respbuf,64,1000);
        if(resplen > 0)
        {
            memset(respbuf, 0,64);
            resplen = at_command(buf,len,"+CIPSEND:",respbuf,64,1000);
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

    if((fd <0)||(fd >= cn_5320e_socket_max)||(NULL == g_socket_cb[fd]))
    {
        return ret;
    }
    item =  g_socket_cb[fd]; 
    
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
static void  __push_data(char *host, u8_t *data, s32_t datalen)
{
    u8_t sockhost[64];
    struct socket_item *item;
    u8_t i;
    s32_t cpylen;
    for(i =0;i < cn_5320e_socket_max;i++)
    {   
        item = g_socket_cb[i];
        if(item != NULL)
        {
            snprintf((char *)sockhost,64,"%s:%s",item->host,item->port);
            if(0 == memcmp(sockhost,host,strlen(host))) //match it
            {
                if(mutex_lock(item->mutex))
                {
                    cpylen = datalen > cn_5320e_socket_rcvlen?cn_5320e_socket_rcvlen:datalen;
                    memcpy(item->rcvbuf,data,cpylen);
                    item->rcvdatalen = cpylen;

                    mutex_unlock(item->mutex);
                    semp_post(item->rcvsync);
                }
            }

        }
    }
}
//format:       RECV FROM:47.106.88.116:8888 IPD30  AAAAA
//this function used to resgiter to the at for oob monitor
static s32_t handler_rcvdata(u8_t *buf,s32_t len)
{
    s32_t  ret = 0;
    char  *cmpaddr;
    char  *host;
    u8_t  *data;
    s32_t datalen = 0;
    const char *checkaddr;
    
    checkaddr = (const char *)buf;
    cmpaddr = strstr(checkaddr,":");
    if(NULL == cmpaddr)
    {
        return ret;
    }
    host = cmpaddr+1;
    checkaddr = host;

    cmpaddr = strstr(checkaddr,"\r");
    if(NULL == cmpaddr)
    {
        return ret;
    }
    *cmpaddr = '\0';  //find the host and  the end
    checkaddr = cmpaddr +2;
    
    cmpaddr = strstr(checkaddr,"IPD");
    if(NULL == cmpaddr)
    {
        return ret;
    }
    checkaddr = cmpaddr+3;
    for (; *checkaddr <= '9' && *checkaddr >= '0' ;checkaddr++)
    {
        datalen = (datalen * 10 + (*checkaddr - '0'));
    }
    
    cmpaddr = strstr(checkaddr,"\n");
    if(NULL == cmpaddr)
    {
        return ret;
    }
    data = (u8_t*)(cmpaddr +1); //find the data start address
    len  =len - (s32_t)(data - buf);
    if(len < datalen)
    {
        return ret;  //not implement data;
    }
  //  printf("applicationdata:len:%d bytes\n\r",datalen);
    __push_data(host,data,datalen);
    ret = len;

    return ret;
}

#include <sal/atiny_socket.h>
const struct atiny_netdevice  g_sim5320E=
{
    .connect = socket_connect,
    .close = socket_close,
    .snd = socket_send,
    .rcv  = socket_receive,
};

//the sim5320e device init function here
bool_t  sim5320e_init(void)
{
    bool_t ret = false;
    
    atiny_install_netdevice((struct atiny_netdevice *)&g_sim5320E);
    at_oobregister(handler_rcvdata,"\r\nRECV FROM:");
    while(1)
    {
        task_sleepms(5000);
        if(false == __close_echo())
        {
            printf("echo close err,do continue...\n\r");
            continue;
        }
        else
        {
            printf("echo close success\n\r");
        }

        if(false == __cpin_check())
        {
            printf("sim card not detected,continue to detect...\n\r");
            task_sleepms(1000);
            continue;
        }
        else
        {
            printf("cpin check success\n\r");
        }

        if(false == __cgatt_check())
        {
            printf("net work not detected,continue to detect...\n\r");
            task_sleepms(1000);
            continue;
        }
        else
        {
            printf("cgatt check success\n\r");
        }
        __open_network();  //we don't check the result for sometimes the network has been opened
        //reach here means all the detect has passed
        ret = true;
        break;
    }
    return ret;
}





//

static bool_t s_gps_report = false;

static u32_t __task_gps_entry(void *arg)
{
    u8_t   respbuf[128];
    u32_t  resplen;
    const char *cmdbuf;
   
    while(1)
    {
        if(s_gps_report)
        {
             memset(respbuf,0,128);
             cmdbuf =  "AT+CGPS=1\r";  //which will open the gps
             resplen = at_command((u8_t *)cmdbuf,strlen((char *)cmdbuf),"OK",respbuf,128,1000);
             if(resplen > 0)
             {
                 printf("%s:%s\r\n",__FUNCTION__,respbuf);
             }
             
             memset(respbuf,0,128);
             cmdbuf =  "AT+CGPSINFO\r";  //which will open the gps
             resplen = at_command((u8_t *)cmdbuf,strlen((char *)cmdbuf),"OK",respbuf,128,1000);
             if(resplen > 0)
             {
                 printf("%s:%s\r\n",__FUNCTION__,respbuf);
             }       
        }
        task_sleepms(5*1000);
    }

}



static s32_t __sim5320e_gps_en(s32_t argc,const char *argv[])
{
    task_create("gpsreport",__task_gps_entry,0x800,NULL,NULL,14);

    return 0;
}

static s32_t __sim5320e_gps_report(s32_t argc,const char *argv[])
{
    if ( 2 == argc )
    {
        if( 0 == strcmp ( "enable" , argv[1] ) )
        {
            s_gps_report = true ;
        }
        else 
        {
            s_gps_report = false ;
 
        }
    
    }
    
    return 0;
}


#include <shell.h>

OSSHELL_EXPORT_CMD(__sim5320e_gps_en,"gpsen","gpsen");
OSSHELL_EXPORT_CMD(__sim5320e_gps_report,"gpsreport","gpsreport  enable/disable");





















