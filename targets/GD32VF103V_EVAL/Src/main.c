#include <stdio.h>
#include "gd32vf103.h"
#include "gd32v103v_eval.h"

#include "los_base.h"
#include "los_typedef.h"

#include "mem.h"

#if defined (__CC_ARM)
extern char __heap_start__ [];
#elif defined (__GNUC__)
extern char __los_heap_addr_start__ [];
extern char __los_heap_addr_end__ [];
#else
#error "fix me"
#endif

extern VOID LOS_MemInfo (bool bShowDetail);

const struct phys_mem system_phys_mem [] =
    {
#if defined (__CC_ARM)
        { __heap_start__, (char *) 0x2002FC00, },
#elif defined (__GNUC__)
        { __los_heap_addr_start__, __los_heap_addr_end__, },
#else
#error "fix me"
#endif
        { 0, 0 }
    };


void eclic_mtip_handler(void)
{
    *(UINT64 *)(TMR_CTRL_ADDR + TMR_MTIME) = 0;
    /* vPortSetupTimerInterrupt() uses LPIT0 to generate the tick interrupt. */
    osTickHandler();
}


static UINT32 g_TaskID1, g_TaskID2, g_TaskID3;
LITE_OS_SEC_TEXT VOID TEST1(VOID)
{
    while (1)
    {
        printf("test1\n");
        LOS_TaskDelay(10);
    }
}

LITE_OS_SEC_TEXT VOID TEST2(VOID)
{
    while (1)
    {
        printf("test2\n");
        LOS_TaskDelay(20);
    }
}

LITE_OS_SEC_TEXT VOID TEST3(VOID)
{
    while (1)
    {
        printf("test3\n");
        LOS_TaskDelay(30);
    }
}

void LOS_BoadExampleEntry(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST1;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test1";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID1, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST2;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test2";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID2, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST3;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test3";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID3, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }

    return;
}


int main() {
	int a = 100;
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);


    /* output a message on hyperterminal using printf function */
    printf("\r\n Enter into LiteOS \r\n");
    printf("0x%x---0x%x\r\n", __los_heap_addr_start__, __los_heap_addr_end__ );

    UINT32 uwRet = LOS_OK;


    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //LOS_MemInfo(1);
    LOS_BoadExampleEntry();

    (void)LOS_Start();
    return 0;

}
