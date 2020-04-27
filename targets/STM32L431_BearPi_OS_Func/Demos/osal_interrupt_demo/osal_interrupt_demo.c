#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <osal.h>
#include "stm32l4xx_hal.h"
uint32_t sum1;
uint32_t sum2;
static int app_hello_world_entry()
{
    while (1)
    {
        printf("Hello World! This is Bearpi!\r\n");
        osal_task_sleep(4*1000);
    }
}
static Key1_interrupt_entry()
{
   
    printf("KEY1 Interrupt entry OK!,sum1:%d\r\n",sum1++);
    __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_2);
}
static Key2_interrupt_entry()
{
   
    printf("KEY2 Interrupt entry OK!,sum2:%d\r\n",sum2++);
    __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_3);
}
int standard_app_demo_main()
{
    osal_task_create("helloworld",app_hello_world_entry,NULL,0x400,NULL,2);
    osal_int_connect( EXTI2_IRQn, 3, NULL, Key1_interrupt_entry, NULL);
    osal_int_connect( EXTI3_IRQn, 4, NULL, Key2_interrupt_entry, NULL);
    return 0;
}

