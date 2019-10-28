
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sal.h>
#include <osal.h>
#include "test_case.h"

#define LOCAL_PORT  5999

#define RECV_BUFFER_LEN 512

test_sort ts_sort_flist[] = {
    NULL,
    NULL,
    ts_sort_oc_mqtt_al,
};

static char g_acRecvBuf[RECV_BUFFER_LEN];
static char g_tempBuf[RECV_BUFFER_LEN];

int g_apitestfin = 0;


int test_main_entry(void *paras)
{
    int ret;
    int server_fd;
    struct sockaddr_in ser_addr;
    struct sockaddr_in cli_addr;
    socklen_t addr_len = sizeof(cli_addr);
    int recv_len,send_len;
    int test_id;
    int prot_id,entry_id;
    int result;
    char *pchTmp, *pchStrTmpIn;
    struct hostent *entry;
    
    server_fd = sal_socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    printf("come into test_main_entry \n");
    entry = sal_gethostbyname("192.168.1.100");
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    memcpy(&ser_addr.sin_addr.s_addr,entry->h_addr_list[0],sizeof(ser_addr.sin_addr.s_addr));
    ser_addr.sin_port = htons(LOCAL_PORT);
    ret = sal_bind(server_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0){
        printf("socket bind fail!\n");
        return -1;
    }

    printf("will come into test_main_entry \n");

    for(;;)
    {
        
        recv_len = sal_recvfrom(server_fd, g_acRecvBuf, sizeof(g_acRecvBuf), 0, (struct sockaddr *)&cli_addr, &addr_len);
        if (recv_len < 0)
        {
           printf("error(%d) occur while receive\n",recv_len);
           break;
        }
        printf("recve msg: %s,%d\n",g_acRecvBuf,recv_len);
        memcpy(g_tempBuf, g_acRecvBuf, recv_len);

        pchTmp      = g_tempBuf;
        pchStrTmpIn = NULL;
        pchTmp      = strtok_r(pchTmp, "|", &pchStrTmpIn);
        test_id     = atoi(pchTmp);
        prot_id     = test_id >> 16;
        entry_id    = test_id & 0xFFFF;

        printf("prot_id:%d, entry_id: %d\n",prot_id,entry_id);

        result = ts_sort_flist[prot_id](entry_id, g_acRecvBuf, recv_len);
        memset(g_acRecvBuf,0,sizeof(g_acRecvBuf));
        snprintf(g_acRecvBuf,sizeof(g_acRecvBuf),"%d:%d:goodluck",test_id,result);
        printf("====***:%s\n",g_acRecvBuf);
        send_len = sal_sendto(server_fd, g_acRecvBuf, strlen(g_acRecvBuf), 0, (struct sockaddr *)&cli_addr, addr_len);
        if (send_len < 0)
        {
            printf("error(%d) occur while send!!!\n",send_len);     
            continue;
        }
    }

    return 0;
}


void autotest_start(void)
{
    osal_task_create("test_main",test_main_entry,NULL,0x1000,NULL,10);
    /*wait oc register api test over*/
    do {
        osal_task_sleep(500);
    } while(g_apitestfin==0);
}




