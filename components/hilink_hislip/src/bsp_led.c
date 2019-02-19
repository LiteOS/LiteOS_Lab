#include "bsp_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOI_CLK_ENABLE();
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_InitStruct.Alternate = 
	HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
		
    /*关闭RGB灯*/
    LED_RGBOFF;		
}
/*********************************************END OF FILE**********************/
