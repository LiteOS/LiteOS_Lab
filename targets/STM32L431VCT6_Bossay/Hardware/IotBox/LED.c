/********************************************************************************
* 文件名称 ：LED.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox LED驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/

#include "Actuators.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "gpio.h"

/***************************************************************
* 函数名称: Init_LED
* 说    明: 初始化IoTBox的LED灯
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_LED(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  LED_GPIO_CLK_ENABLE();

	 /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

	
	 /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

/***************************************************************
* 函数名称: IoTBox_LED_StatusSet
* 说    明: IoTBox开发板上的LED灯的亮灭控制
* 参    数: status,LED灯的状态
*									OFF,关灯
*									ON,开灯
* 返 回 值: 无
***************************************************************/
void IoTBox_LED_StatusSet(Actuators_Status_ENUM status)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, status != ON ? GPIO_PIN_RESET : GPIO_PIN_SET );	
}

void Led_blink(void)
{
    while(1)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_Delay(1000); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_Delay(1000);
    }

}

