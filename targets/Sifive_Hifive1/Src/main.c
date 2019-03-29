#include <stdio.h>

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


static UINT32 g_TaskID1, g_TaskID2, g_TaskID3;
LITE_OS_SEC_TEXT VOID TEST1(VOID)
{
    //struct metal_led *led0_green;
    //led0_green = metal_led_get_rgb("LD0", "green");
    while (1)
    {
        //metal_led_enable(led0_green);
        //printf("test1\n");
        metal_tty_putc(0x31);
        metal_tty_putc('\r');
        metal_tty_putc('\n');
        //LOS_Schedule();
        //osTaskSchedule();
        LOS_TaskDelay(0);
    }
}

LITE_OS_SEC_TEXT VOID TEST2(VOID)
{
    //struct metal_led *led0_blue;
    //led0_blue = metal_led_get_rgb("LD0", "blue");
    while (1)
    {
        //metal_led_enable(led0_blue);
        //printf("test2\n");
        metal_tty_putc(0x32);
        metal_tty_putc('\r');
        metal_tty_putc('\n');
        //LOS_Schedule();
        //osTaskSchedule();
        LOS_TaskDelay(0);
    }
}

LITE_OS_SEC_TEXT VOID TEST3(VOID)
{
    //struct metal_led *led0_blue;
    //led0_blue = metal_led_get_rgb("LD0", "blue");
    while (1)
    {
        //metal_led_enable(led0_blue);
        //printf("test3\n");
        metal_tty_putc(0x33);
        metal_tty_putc('\r');
        metal_tty_putc('\n');
        //LOS_Schedule();
        //osTaskSchedule();
        LOS_TaskDelay(0);
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
    //printf("ret:0x%x\n", uwRet);
    if (uwRet != LOS_OK)
    {
        return;
    }

    //printf("task create1\r\n");

//    LOS_MemInfo(1);    

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST2;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test2";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID2, &stTaskInitParam);

    //printf("ret:0x%x\n", uwRet);
    if (uwRet != LOS_OK)
    {
        return;
    }
    //printf("task create2\r\n");

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST3;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test3";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID3, &stTaskInitParam);

    //printf("ret:0x%x\n", uwRet);
    if (uwRet != LOS_OK)
    {
        return;
    }

    //printf("task create3\r\n");
    return;
}


int main() {
    //printf("Hello, World!\n");
    //printf("0x%x---0x%x\r\n", __los_heap_addr_start__, __los_heap_addr_end__ );

    UINT32 uwRet = LOS_OK;

    struct metal_led *led0_red, *led0_green, *led0_blue;
    led0_red = metal_led_get_rgb("LD0", "red");
    led0_green = metal_led_get_rgb("LD0", "green");
    led0_blue = metal_led_get_rgb("LD0", "blue");
    if ((led0_red == NULL) || (led0_green == NULL) || (led0_blue == NULL)) {
        printf("At least one of LEDs is null.\n");
        return 1;
    }

    // Enable each LED
    metal_led_enable(led0_red);
//    metal_led_enable(led0_green);
//    metal_led_enable(led0_blue);

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
