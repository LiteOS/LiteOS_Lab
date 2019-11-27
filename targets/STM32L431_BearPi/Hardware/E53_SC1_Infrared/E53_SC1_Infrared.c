/********************************************************************************
    * 文件名称 ：E53_SC1_Infrared.c
    * 作     者：LGS
    * 版     本：V1.0
    * 编写日期 ：2019-11-27
    * 功     能：E53_CS1扩展板增加人体红外传感器驱动
*********************************************************************************
    * 说    明 ：本例程配套物联网俱乐部开发板使用
    *
    * 淘     宝：https://shop128001708.taobao.com/
    * 论     坛：bbs.iot-club.cn
*********************************************************************************/

#include "stm32l4xx.h"
#include "E53_SC1_Infrared.h"


/***************************************************************
* 函数名称: Init_BH1750
* 说    明: 写命令初始化BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_E53_SC1_Infrared(void)
{

	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  SC1_Infrared_GPIO_CLK_ENABLE();

	  /*Configure GPIO pin : Ifrared_Pin */
	  GPIO_InitStruct.Pin = SC1_Infrared_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	  HAL_GPIO_Init(SC1_Infrared_GPIO_Port, &GPIO_InitStruct);

	  //HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
	  //HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	

}
