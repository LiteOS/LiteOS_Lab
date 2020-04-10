#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "los_typedef.h"
#include "los_task.h"
#include <osal.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>

/* LWM2M服务器相关配置 */
#define cn_endpoint_id        "lwm2m_001"
#define cn_app_server         "119.3.250.80"
#define cn_app_port           "5684"

//if your command is very fast,please use a queue here--TODO
#define cn_app_rcv_buf_len 128
static int8_t          s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;



//use this function to push all the message to the buffer
static int app_msg_deal(void *usr_data,en_oc_lwm2m_msg_t type, void *msg, int len)
{
    int ret = -1;

    if(len <= cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg,len);
        s_rcv_datalen = len;

        osal_semp_post(s_rcv_sync);

        ret = 0;

    }
    return ret;
}


int task2(void *args)
{
    int ret = -1;
    oc_config_param_t      oc_param;
    

    memset(&oc_param,0,sizeof(oc_param));

    oc_param.app_server.address = cn_app_server;
    oc_param.app_server.port = cn_app_port;
    oc_param.app_server.ep_id = cn_endpoint_id;
    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
    oc_param.rcv_func = app_msg_deal;

    ret = oc_lwm2m_config(&oc_param);
    if (0 != ret)
    {
        return ret;
    }
}

int Creat_task2()
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S task2_init_param;

    memset (&task2_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task2_init_param.uwArg = (unsigned int)NULL;
    task2_init_param.usTaskPrio = 2;
    task2_init_param.pcName =(char *) "task2";
    task2_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;
    task2_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &task2_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
    printf("Task2 init success!!\r\n");
}