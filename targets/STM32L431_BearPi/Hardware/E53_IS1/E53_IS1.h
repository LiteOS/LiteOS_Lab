/********************************************************************************
    * 文件名称 ：E53_IS1.h
    * 作     者：小熊派开源社区
    * 版     本：V1.0
    * 编写日期 ：2019-5-27
    * 功     能：E53_IS1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派开源社区开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
    * 微信公众号：小熊派开源社区
*********************************************************************************/
#ifndef __E53_IS1_H__
#define __E53_IS1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define IS1_Beep_Pin GPIO_PIN_8
#define IS1_Beep_GPIO_Port GPIOB
#define IS1_Beep_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define IS1_IN_Pin GPIO_PIN_9
#define IS1_IN_GPIO_Port GPIOB
#define IS1_IN_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void Init_E53_IS1(void);
void E53_IS1_Beep_StatusSet(E53_IS1_Status_ENUM status);

#endif





