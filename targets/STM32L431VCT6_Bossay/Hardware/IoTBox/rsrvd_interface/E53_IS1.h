/********************************************************************************
* 文件名称 ：E53_IS1.h
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2020-02-12
* 功 能：人体红外检测扩展板驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/
#ifndef __E53_IS1_H__
#define __E53_IS1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define IS1_Beep_Pin GPIO_PIN_8
#define IS1_Beep_GPIO_Port GPIOB
#define IS1_Beep_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define IS1_IN_Pin GPIO_PIN_12
#define IS1_IN_GPIO_Port GPIOD
#define IS1_IN_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

/***************************************************************
* 名		称: GasStatus_ENUM
* 说    明：枚举状态结构体
***************************************************************/
typedef enum
{
	OFF = 0,
	ON
} E53_IS1_Status_ENUM;

extern TIM_HandleTypeDef htim16;
void Init_E53_IS1(void);
void E53_IS1_Beep_StatusSet(E53_IS1_Status_ENUM status);

#endif





