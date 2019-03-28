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

#include <at.h>

#include "nb.h"

#define cn_nb_cmd_timeout       (2*1000)
#define cn_try_times            (32)

typedef void (*fnhandle)(u8_t *data, s32_t len);

#define cn_nb_cachelen 128

struct nb_cb
{
    bool_t      sndenable;
    u8_t        sndbuf[cn_nb_cachelen];  //used for the receive
    u8_t        rcvbuf[cn_nb_cachelen];
    fnhandle    rcvhandle;
    //for the debug
    u32_t   rcvlen;
    u32_t   sndlen;

    //states for the lwm2m
    bool_t  lwm2m_observe;
};

static struct nb_cb   s_nb_cb;

//use this function to register a handle to deal with the received data
void nb_regester_receivehandle(void *handle)
{
    s_nb_cb.rcvhandle = (fnhandle)handle;
}

//nb common at command
static bool_t nb_atcmd(const char *cmd, const char *index)
{
    s32_t ret = 0;
    ret = at_command((u8_t *)cmd, strlen(cmd), index, NULL, 0, cn_nb_cmd_timeout);

    return ret > 0;
}

//nb command  with response
//nb common at command
static bool_t nb_atcmd_response(const char *cmd, const char *index, char *buf, int len)
{
    s32_t ret = 0;
    ret = at_command((u8_t *)cmd, strlen(cmd), index, (u8_t *)buf, len, cn_nb_cmd_timeout);

    return ret > 0;
}

//this function to get the csq
bool_t nb_csq(u32_t *value)
{
    char cmd[64];
    char resp[64];
    const char *index = "OK";
    const char *str;
    u32_t csq = 0;
    bool_t ret = false;

    if (NULL == value)
    {
        return ret;
    }

    memset(cmd, 0, 64);
    memset(resp, 0, 64);
    snprintf(cmd, 64, "AT+CSQ\r");
    if (nb_atcmd_response(cmd, index, resp, 64))
    {
        str = strstr((char *)resp, "+CSQ:");
        if (NULL != str)
        {
            str += strlen("+CSQ:");
            for (; *str <= '9' && *str >= '0'; str++)
            {
                csq = (csq * 10 + (*str - '0'));
            }
            *value = csq;
            ret = true;
        }
    }
    return ret;
}

//make a byte to 2 ascii hex
static int nb_str_to_hex(const unsigned char *bufin, int len, char *bufout)
{
    int i = 0;
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for (i = 0; i < len; i++)
    {
        sprintf(bufout+i*2, "%02X", bufin[i]);
    }
    return 0;
}


//send data
bool_t nb_send(u8_t *buf, s32_t len, u32_t timeout)
{
    s32_t ret = 0;
    const char *cmd = "AT+NMGS=";
    const char *index = "OK";
    if (NULL == buf || len >= 128 ||(false == s_nb_cb.sndenable))
    {
        return false;
    }
    memset(s_nb_cb.sndbuf, 0, cn_nb_cachelen);
    snprintf((char *)s_nb_cb.sndbuf, cn_nb_cachelen, "%s%d,", cmd, len);
    ret = nb_str_to_hex((unsigned char *)buf, len, (char *)&s_nb_cb.sndbuf[strlen((char *)s_nb_cb.sndbuf)]);
    s_nb_cb.sndbuf[strlen((char *)s_nb_cb.sndbuf)] = '\r';
    ret = at_command((u8_t *)s_nb_cb.sndbuf, strlen((char *)s_nb_cb.sndbuf), index, NULL, 0, timeout);

    return ret > 0;
}

/* ============================================================
func name   :  neul_nb_hex_to_str
discription :  convert char to hex string

param       :  bufin @ input param , the data that need to convert
               len @ input param, data length
               bufout @ output param, hex string data
return      :  0 convert ok , !0 not ok
Revision    :
=============================================================== */
static int nb_hex_to_str(const char *bufin, int len, char *bufout)
{
    int i = 0;
    unsigned char tmp2 = 0x0;
    unsigned int tmp = 0;
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for (i = 0; i < len; i = i + 2)
    {
        tmp2 =  bufin[i];
        tmp2 =  tmp2 <= '9'?tmp2-0x30:tmp2-0x37;
        tmp =  bufin[i+1];
        tmp =  tmp <= '9'?tmp-0x30:tmp-0x37;
        bufout[i/2] = (tmp2<<4)|(tmp&0x0F);

    }
    return 0;
}

#define cn_nb_rcvindex  "\r\n+NNMI:"
static s32_t nb_rcvdeal(u8_t *data, s32_t len)
{
    s32_t ret = 0;
    s32_t datalen;

    char  *str;
    if (len <strlen(cn_nb_rcvindex))
    {
        printf("%s:invalid frame:%d byte:%s\n\r", __FUNCTION__, len, (char *)data);
        return ret;
    }
    //now deal the data
    str = strstr((char *)data, ":");
    if (NULL == str)
    {
        printf("%s:format error\n\r", __FUNCTION__);
        return ret; //format error
    }
    str++;
    datalen = 0;
    for (; *str <= '9' && *str >= '0'; str++)
    {
        datalen = (datalen * 10 + (*str - '0'));
    }
    str++;
    //now this is data of hex string
    if ((u32_t)(str + datalen*2 +2) > (u32_t)(data + len))
    {
        printf("%s:implement error\n\r", __FUNCTION__);
        return ret; //
    }
    if (datalen > cn_nb_cachelen)
    {
        printf("%s:frame over: frame:%d  cachelen:%d \n\r", __FUNCTION__, datalen, cn_nb_cachelen);
        return ret; //
    }
    memset(s_nb_cb.rcvbuf, 0, cn_nb_cachelen);
    nb_hex_to_str(str, datalen*2, (char *)s_nb_cb.rcvbuf);
    printf("%s:appdata:%d bytes\n\r", __FUNCTION__, datalen);
    if (NULL != s_nb_cb.rcvhandle)
    {
        s_nb_cb.rcvhandle(s_nb_cb.rcvbuf, datalen);
    }
    return len;
}

//use this function to do the module reboot
static bool_t nb_reboot(void)
{
    //do the module reset
    nb_atcmd("ATE0\r", "OK");
    nb_atcmd("ATE0\r", "OK");
    nb_atcmd("AT+NRB\r", "REBOOTING");
    task_sleepms(10000); //wait for the module boot
    nb_atcmd("ATE0\r", "OK");
    nb_atcmd("ATE0\r", "OK");


    return true;
}

//enable the echooff
static bool_t nb_echoff(void)
{
    return nb_atcmd("ATE0\r", "OK");
}

//use this function to set the band
static bool_t nb_setband(const char *bands)
{
    char cmd[64];
    char resp[64];
    bool_t ret = true;

    if (NULL != bands)  //which measn we need to set if the default is not the same
    {
        memset(resp, 0, 64);
        ret = nb_atcmd_response("AT+NBAND?\r", "OK", resp, 64);
        if ((false == ret)||(NULL == strstr(resp, bands)))//which means we need to set it
        {
            nb_atcmd("AT+CFUN=0\r", "OK");
            memset(cmd, 0, 64);
            snprintf(cmd, 64, "AT+NBAND=%s\r", bands);
            ret = nb_atcmd(cmd, "OK");
            nb_atcmd("AT+CFUN=1\r", "OK");
        }
        else
        {
            ret = true;  //we need not to set
        }
    }
    return ret;
}

//set the plmn
static bool_t nb_setplmn(const char *plmn)
{
    char cmd[64];
    char resp[64];
    bool_t ret;

    if (NULL != plmn)  //which measn we need to set if the default is not the same
    {
        memset(resp, 0, 64);
        ret = nb_atcmd_response("AT+COPS?\r", "+COPS", resp, 64);
        if ((false == ret)||(NULL == strstr(resp, plmn)))//which means we need to set it
        {
            memset(cmd, 0, 64);
            snprintf(cmd, 64, "AT+COPS=1,2,\"%s\"\r", plmn);
            ret = nb_atcmd(cmd, "OK");
        }
        else
        {
            ret = true;  //we need not to set
        }
    }
    else  //set it to auto
    {
        memset(cmd, 0, 64);
        snprintf(cmd, 64, "AT+COPS=0\r");
        ret = nb_atcmd(cmd, "OK");
    }

    return ret;
}

//set the apn
static bool_t nb_setapn(const char *apn)
{
    char cmd[64];
    char resp[64];
    bool_t ret = true;

    if (NULL != apn)  //which measn we need to set if the default is not the same
    {
        memset(resp, 0, 64);
        ret = nb_atcmd_response("AT+CGDCONT?\r", "OK", resp, 64);
        if ((false == ret)||(NULL == strstr(resp, apn)))//which means we need to set it
        {
            memset(cmd, 0, 64);
            snprintf(cmd, 64, "AT+CGDCONT=%s\r", apn);
            ret = nb_atcmd(cmd, "OK");
        }
        else
        {
            ret = true;  //we need not to set
        }
    }
    return ret;
}

//set the ncdp
static bool_t nb_setserver(const char *server)
{
    char cmd[64];
    char resp[64];
    bool_t ret = true;

    if (NULL != server)  //which measn we need to set if the default is not the same
    {
        memset(resp, 0, 64);
        ret = nb_atcmd_response("AT+NCDP?\r", "OK", resp, 64);
        if ((false == ret)||(NULL == strstr(resp, server)))//which means we need to set it
        {
            memset(cmd, 0, 64);
            snprintf(cmd, 64, "AT+NCDP=%s\r", server);
            ret = nb_atcmd(cmd, "OK");
        }
        else
        {
            ret = true;  //we need not to set
        }
    }
    return ret;
}

//enable the cmee

static bool_t nb_setcmee()
{
    bool_t ret;

    ret = nb_atcmd("AT+CMEE=1\r", "OK");

    return ret;
}

//enable the network
static bool_t nb_setcgatt()  //unit second
{
    //wait for the server observed
    bool_t ret = false;

    ret = nb_atcmd("AT+CGATT=1\r", "OK");

    return ret;
}

//enable the NNMI
static bool_t nb_setnnmi()  //unit second
{
    //wait for the server observed
    bool_t ret = false;

    ret = nb_atcmd("AT+NNMI=1\r", "OK");

    return ret;
}
//enable the cfun
static bool_t nb_setcfun()  //unit second
{
    //wait for the server observed
    bool_t ret = false;

    ret = nb_atcmd("AT+CFUN=1\r", "OK");

    //i think we should do some wait here
    task_sleepms(2000);

    return ret;
}


//wait for the lwm2m observe
#define cn_urc_qlwevtind    "\r\n+QLWEVTIND:"
static s32_t urc_qlwevtind(u8_t *data, s32_t len)
{

    int index_str;
    int ind;
    index_str = strlen(cn_urc_qlwevtind);

    if (len > index_str)
    {
        ind = data[index_str]-'0';
        printf("GET THE LWM2M:ind:%d\n\r", ind);
        if (ind == 3)
        {
            s_nb_cb.lwm2m_observe = true;
        }
    }
    return len;
}


//check the lwm2m server observed
static bool_t nb_checklwm2m(s32_t time)  //unit second
{
    //wait for the server observed
    bool_t ret = false;
    s32_t times;

    for (times = 0; times < time; times++)
    {
        if (s_nb_cb.lwm2m_observe)
        {
            ret = true;
            break;
        }
        task_sleepms(1000);
    }

    return ret;
}

//check if attached to the network
static bool_t nb_checknetwork(s32_t time)  //unit second
{
    //wait for the server observed
    bool_t ret = false;
    s32_t times;

    for (times = 0; times < time; times++)
    {
        if (nb_atcmd("AT+CGATT?\r", "CGATT:1"))
        {
            ret = true;
            break;
        }
        task_sleepms(1000);
    }

    return ret;
}

//set the autoconfig mode
static bool_t nb_setnconfig(const char *mode)
{
    char cmd[64];
    char resp[64];
    bool_t ret = true;

    if (NULL != mode)   //which measn we need to set if the default is not the same
    {
        memset(resp, 0, 64);
        ret = nb_atcmd_response("AT+NCONFIG?\r", "+NCONFIG", resp, 64);
        if ((false == ret)||(NULL == strstr(resp, mode)))//which means we need to set it
        {
            memset(cmd, 0, 64);
            snprintf(cmd, 64, "AT+NCONFIG=%s\r", mode);
            ret = nb_atcmd(cmd, "OK");

            nb_reboot();
        }
        else
        {
            ret = true;  //we need not to set
        }
    }
    return ret;
}

//use this function to set the band, which corresponding with YUNYINGSHANG AND MOZU
bool_t nb_init(tagNbConfig *config)
{
    memset(&s_nb_cb, 0, sizeof(s_nb_cb));
    at_oobregister(urc_qlwevtind, cn_urc_qlwevtind);
    at_oobregister(nb_rcvdeal, cn_nb_rcvindex);

    while (1)
    {

        s_nb_cb.lwm2m_observe = false;

        nb_reboot();

        nb_echoff();

        nb_setcmee();

        nb_setnconfig("AUTOCONNECT,FALSE");     //cgatt and cfun must be called if autoconnect is false

        nb_setband(config->bands);

        nb_setcfun();

        nb_setplmn(config->plmn);

        nb_setapn(config->apn);

        nb_setserver(config->server);

        nb_setcgatt();

        nb_setnnmi();

        nb_checknetwork(16);

        if (false == nb_checklwm2m(16))
        {
            continue; //we should do the reboot for the nB
        }

        break;
    }
    //reach here means everything is ok, we can go now
    s_nb_cb.sndenable = true;
    printf("NB MODULE RXTX READY NOW\n\r");
    return true;
}

#include <shell.h>

static s32_t shell_nbsend(s32_t argc, const char *argv[])
{
    if (argc != 2)
    {
        printf("paras error\n\r");
        return -1;
    }
    if (nb_send((u8_t *)argv[1], strlen(argv[1]), 5*1000))
    {
        printf("%s: send success\n\r", __FUNCTION__);
    }
    else
    {
        printf("%s: send failed\n\r", __FUNCTION__);
    }

    return 0;

}

OSSHELL_EXPORT_CMD(shell_nbsend, "nbsend", "nbsend data");
