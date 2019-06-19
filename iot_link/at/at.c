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
#include <stdlib.h>
#include <string.h>
#include <driver.h>


//these defines could be moved to the configuration of the at module
#define cn_at_oob_len          6            //only allow 6 oob command monitor here,you could configure it more
#define cn_at_resp_maxlen      768           //PROSING THAT COULD GET THE MOST REPSLENGTH

//at control block here
struct at_cmd
{
    char       *cmd;               //pointed to the at command
    int         cmdlen;            //how many bytes of the command for the command may not be string(binary string)
    const char *index;             //used to check if the frame is the response of the command
    char       *respbuf;           //which used to storage the response,supplied by the at command
    int         respbuflen;        //index the response buffer length,supplied by the at command
    int         respdatalen;       //index how many data in the response buffer, filled by the at engine
    osal_semp_t      respsync;     //binary semphore, activated by the at engine,if any response matched
    osal_semp_t      cmdsync;      //used to make the at command sync, binary sempore, only one at command could be excuted at one time
    osal_mutex_t     cmdlock;      //make the command list atomic
};//the member cmd and cmdlen not used yet,will be used for the debug

struct at_oob
{
    fnoob       func;     //the function that will deal the out of band data
    const char *index;    //used to match the out of band data,only compared from the header
};

typedef enum
{
    en_at_debug_none = 0,
    en_at_debug_ascii,
    en_at_debug_hex,
}enRxTxDebugMode;        //FUNCTION USED TO CONTROL THE DEBUG MODE


struct at_cb
{
    const char             *devname;  //we use the device frame work to do this
    los_dev_t               devhandle;//the device handle used 

    fnoob                   funcpass; //the member function to deal the passby data
    struct at_cmd           cmd;      //the at command,only one command could be excuted
    struct at_oob           oob[cn_at_oob_len];        //storage the out of band dealer
    char                    rcvbuf[cn_at_resp_maxlen]; //used storage one frame,read from the at channel
    u32_t                   passmode:1;                //if zero, then use the at mode,else use the pass mode,pass all the data to fnoob
    u32_t                   rxdebugmode:2;             //receive debug mode
    u32_t                   txdebugmode:2;             //send debug mode
};
static struct at_cb g_at_cb;   //this is the at controller here

//this function used to send the data to the AT channel
static int __cmd_send(char *buf,int buflen,int timeout)
{
    int i = 0;
    int ret = 0;
    int debugmode;
    ret = los_dev_write(g_at_cb.devhandle,0,buf,buflen,timeout);
    if(ret > 0)
    {
        debugmode = g_at_cb.txdebugmode;
        switch (debugmode)
        {
            case en_at_debug_ascii:
                printf("ATSND:%d Bytes:%s\n\r",ret,(char *)buf);
                break;
            case en_at_debug_hex:
                printf("ATSND:%d Bytes:",ret);
                for(i =0;i<ret;i++)
                {
                    printf("%02x ",buf[i]);
                }
                printf("\n\r");
                break;
            default:
                break;
        }
    }    
    return ret;
}

//this function used to receive data from the AT channel
static int __resp_rcv(char *buf,int buflen,int timeout)
{
    int i = 0;
    int ret = 0;
    int debugmode;
    
    ret = los_dev_read(g_at_cb.devhandle,0,buf,buflen,timeout);
    if(ret > 0)
    {
        debugmode = g_at_cb.rxdebugmode;
        switch (debugmode)
        {
            case en_at_debug_ascii:
                printf("ATRCV:%d Bytes:%s\n\r",ret,(char *)buf);
                break;
            case en_at_debug_hex:
                printf("ATRCV:%d Bytes:",ret);
                for(i =0;i<ret;i++)
                {
                    printf("%02x ",buf[i]);
                }
                printf("\n\r");
                break;
            default:
                break;
        }
    }  
 
    return ret;
}

//create a command
static bool_t  __cmd_create(char *cmdbuf,int cmdlen,const char *index,char *respbuf,int respbuflen,u32_t timeout)
{
    bool_t  ret = false;
    struct at_cmd *cmd;

    cmd = &g_at_cb.cmd;
    if(osal_semp_pend(cmd->cmdsync,timeout))
    {
        if(osal_mutex_lock(cmd->cmdlock))
        {
            cmd->cmd = cmdbuf;
            cmd->cmdlen = cmdlen;
            cmd->index = index;
            cmd->respbuf = respbuf;
            cmd->respbuflen = respbuflen;
            osal_semp_pend(cmd->respsync,0); //used to clear the sync
            osal_mutex_unlock(cmd->cmdlock);
        }
        ret = true;
    }
    return ret;
}

//clear the at command here
static bool_t __cmd_clear(void)
{
     struct at_cmd *cmd;

     cmd = &g_at_cb.cmd;
     if(osal_mutex_lock(cmd->cmdlock))
     {
        cmd->cmd = NULL;
        cmd->cmdlen = 0;
        cmd->index = NULL;
        cmd->respbuf = NULL;
        cmd->respbuflen = 0;
        cmd->respdatalen = 0;
        osal_mutex_unlock(cmd->cmdlock);
     }
     osal_semp_post(cmd->cmdsync);
     return true;
}

//check if the data received is the at command need
static bool_t  __cmd_match(char *data,int len)
{
    bool_t ret = false;
    int  cpylen;
    struct at_cmd *cmd = NULL;
    
    cmd = &g_at_cb.cmd;
    if(osal_mutex_lock(cmd->cmdlock))
    {
        if((NULL != cmd->index)&&(NULL != strstr((const char *)data,cmd->index)))
        {
            if(NULL != cmd->respbuf)
            {
                cpylen = len > cmd->respbuflen?cmd->respbuflen:len;
                memcpy(cmd->respbuf,data,cpylen);
                cmd->respdatalen = cpylen;
            }
            else
            {
                cmd->respdatalen = len; //tell the command that how many data has been get
            }
            osal_semp_post(cmd->respsync);
            ret = true;
        }
        osal_mutex_unlock(cmd->cmdlock);
    }
    return ret;
}

//check if any out of band method could deal the data
static bool_t  __oob_match(char *data,int len)
{
    bool_t ret = false;
    struct at_oob *oob;
    int i = 0;
    for(i =0;i<cn_at_oob_len;i++)
    {
        oob = &g_at_cb.oob[i];
        if((oob->func != NULL)&&(oob->index != NULL)&&\
            (0 == memcmp(oob->index,data,strlen(oob->index))))
        {
            oob->func(data,len);
            ret = true;
            break;
        }
    }
    return ret;
}

/*******************************************************************************
function     :this is the  at receiver engine
parameters   :
instruction  :this task read the device continousely and blocked by the read function
              if pass by mode, then pass it to the pass function;else will match the 
              at command, if not then match the out of band
*******************************************************************************/
static int __rcv_task_entry(void *args)
{
    bool_t matchret;
    int  rcvlen;
    
    g_at_cb.devhandle = los_dev_open(g_at_cb.devname,O_RDWR);
    if(NULL == g_at_cb.devhandle)
    {
        printf("%s:open %s err\n\r",__FUNCTION__,g_at_cb.devname);
        return 0;
    }
    
    while(1)
    {
        memset(g_at_cb.rcvbuf,0,cn_at_resp_maxlen);
        rcvlen = __resp_rcv(g_at_cb.rcvbuf,cn_at_resp_maxlen,LOS_WAIT_FOREVER);
        if(rcvlen > 0)
        {
            if(0 == g_at_cb.passmode)
            {
                matchret = __cmd_match(g_at_cb.rcvbuf,rcvlen);
                if(false == matchret)
                {
                    __oob_match(g_at_cb.rcvbuf,rcvlen);
                }
            }
            else
            {
                if(NULL != g_at_cb.funcpass)
                {
                    g_at_cb.funcpass(g_at_cb.rcvbuf,rcvlen);
                }
            }
        }
    }    
}

/*******************************************************************************
function     :use this function make the at frame work as a pass mode, like the ppp
parameters   :passby:if true, then the at module will work in pass by mode, else at mode
              func:if in bypass mode, then it will use this function to deal all the received data
instruction  :
*******************************************************************************/
bool_t at_workmode(bool_t passby,fnoob func)
{
    if(passby)
    {
        g_at_cb.passmode = 1;
        g_at_cb.funcpass = func;
    }
    else
    {
        g_at_cb.passmode = 0;
        g_at_cb.funcpass = NULL;
    }
    
    return true;
}



/*******************************************************************************
function     :this is our at command here,you could send any command as you wish
parameters   :
instruction  :only one command could be dealt at one time, for we use the semphore
              here do the sync;if the respbuf is not NULL,then we will cpoy the 
              response data to the respbuf as much as the respbuflen permit
*******************************************************************************/
int  at_command(char *cmd,int cmdlen,const char *index,char *respbuf,\
                int respbuflen,int timeout)
{
    int ret = 0;
    if((NULL != cmd))//which means no response need
    {
        if(NULL != index)
        {
            if(__cmd_create(cmd,cmdlen,index,respbuf,respbuflen,timeout))
            {
                __cmd_send(cmd,cmdlen,timeout);
                if(osal_semp_pend(g_at_cb.cmd.respsync,timeout))
                {
                    ret = g_at_cb.cmd.respdatalen;         
                }
                __cmd_clear();
            }
        }
        else
        {
            ret = __cmd_send(cmd,cmdlen,timeout);
        }
    }
    return ret;
}

/*******************************************************************************
function     :you could use this function to register a method to deal the out of band data
parameters   :
instruction  :as you know, we only check the frame begin,using memcmp, so you must
              write the header of the frame as the index
*******************************************************************************/
bool_t at_oobregister(fnoob func,const char *index)
{
    bool_t ret = false;
    struct at_oob *oob;
    int i = 0;

    if((NULL == func)||(NULL == index))
    {
        return ret;
    }

    for(i =0;i<cn_at_oob_len;i++)
    {
        oob = &g_at_cb.oob[i];
        if((oob->func == NULL)&&(oob->index == NULL))
        {
            oob->func = func;
            oob->index = index;
            ret = true;
            break;
        }
    }

    return ret;
}

/*******************************************************************************
function     :this is the  at module initialize function
parameters   :func_read:which will read a frame from the device
              func_write:which will be used to write a frame to the device
instruction  :if you want to use the at frame work, please call this function 
              please supply the function read and write.the read function must be
              a block function controlled by timeout
*******************************************************************************/
bool_t at_init(const char *devname)
{
    bool_t ret = false;
    if(NULL == devname)
    {
        printf("%s:parameters error\n\r",__FUNCTION__);
        goto EXIT_PARA;
    }
    memset(&g_at_cb,0,sizeof(g_at_cb));

    if(false == osal_semp_create(&g_at_cb.cmd.cmdsync,1,1))
    {
        printf("%s:cmdsync error\n\r",__FUNCTION__);
        goto EXIT_CMDSYNC;
    }
    if(false == osal_semp_create(&g_at_cb.cmd.respsync,1,0))
    {
        printf("%s:cmdsync error\n\r",__FUNCTION__);
        goto EXIT_RESPSYNC;
    }
    if(false == osal_mutex_create(&g_at_cb.cmd.cmdlock))
    {
        printf("%s:cmdlock error\n\r",__FUNCTION__);
        goto EXIT_CMDLOCK;
    }

    if(-1 == task_create("at_rcv",__rcv_task_entry,0x800,NULL,NULL,10))
    {
        printf("%s:rcvtask create error\n\r",__FUNCTION__);
        goto EXIT_RCVTASK;
    }

    g_at_cb.devname = devname;
    //for the debug
    g_at_cb.rxdebugmode = en_at_debug_ascii;
    g_at_cb.txdebugmode = en_at_debug_ascii;
  
    ret = true;
    return ret;


EXIT_RCVTASK:
    osal_mutex_del(&g_at_cb.cmd.cmdlock);
    g_at_cb.cmd.cmdlock = CN_OBJECT_INVALID;
EXIT_CMDLOCK:
    osal_semp_del(&g_at_cb.cmd.respsync);
    g_at_cb.cmd.respsync = CN_OBJECT_INVALID;    
EXIT_RESPSYNC:
    osal_semp_del(&g_at_cb.cmd.cmdsync);
    g_at_cb.cmd.cmdsync = CN_OBJECT_INVALID;
EXIT_CMDSYNC:
EXIT_PARA:
    return ret;
}

//////////////////////////////////DEBUG COMMAND FOLLOWING/////////////////////////////////////////
#include <shell.h>
//use this shell command,you could input at command through the terminal
static int shell_at(int argc, const char *argv[])
{
    #define CN_AT_SHELL_LEN 64
     
    char respbuf[CN_AT_SHELL_LEN];
    char cmdbuf[CN_AT_SHELL_LEN];
    
    const char *index =NULL ;
    int ret = -1;

    if((argc !=2)&&(argc != 3))
    {
        printf("paras error\n\r");
        return ret;
    }
    if(argc == 3)
    {
        index = argv[2];
    }
    

    memset(respbuf,0,CN_AT_SHELL_LEN);
    snprintf((char *)cmdbuf,CN_AT_SHELL_LEN,"%s\r",argv[1]);

    ret = at_command(cmdbuf,strlen((const char *)cmdbuf),index,respbuf,CN_AT_SHELL_LEN,LOSCFG_BASE_CORE_TICK_PER_SECOND); //one second
    if(ret > 0)
    {
        printf("atresponse:%d Bytes:%s\n\r",ret,respbuf);
    }
    else
    {
        printf("atresponse:timeout or error\n\r");
    }
    return ret;
}
OSSHELL_EXPORT_CMD(shell_at,"atcmd","atcmd atcommand atrespindex");

//use this function to set the at command display mode:none/ascii/hex
static int shell_atdebug(int argc,const char *argv[])
{
    int mode = en_at_debug_none;
    if(argc != 3)
    {
        printf("paras error\n\r");
        return -1;
    }

    if(0 == strcmp(argv[2],"none"))
    {
        mode = en_at_debug_none;
    }
    else if(0 == strcmp(argv[2],"ascii"))
    {
        mode = en_at_debug_ascii;
    }
    else if(0 == strcmp(argv[2],"hex"))
    {
        mode = en_at_debug_hex;
    }
    else
    {
        printf("mode set error\n\r");
        return -2;
    }

    if(0 == strcmp(argv[1],"rx"))
    {
        g_at_cb.rxdebugmode = mode;
    }
    else if(0 == strcmp(argv[1],"tx"))
    {
        g_at_cb.txdebugmode = mode;
    }
    else
    {
        printf("type set error\n\r");
        return -3;
    }

    return 0;
}
OSSHELL_EXPORT_CMD(shell_atdebug,"atdebug","atdebug rx/tx none/ascii/hex");




