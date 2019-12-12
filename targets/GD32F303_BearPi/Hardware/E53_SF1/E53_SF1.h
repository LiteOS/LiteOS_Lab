/********************************************************************************
    * 文件名称 ：E53_SF1.h
    * 作     者：物联网俱乐部
    * 版     本：V1.0
    * 编写日期 ：2019-5-31
    * 功     能：E53_SF1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派-IoT(GD)开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
*********************************************************************************/

#ifndef __E53_SF1_H__
#define __E53_SF1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "gd32f30x.h"


/* 控制设备IO口定义 ------------------------------------------------------------*/

#define SF1_Beep_Pin GPIO_PIN_8
#define SF1_Beep_GPIO_Port GPIOB
#define SF1_Beep_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SF1_LED_Pin GPIO_PIN_0
#define SF1_LED_GPIO_Port GPIOA
#define SF1_LED_GPIO_CLK  RCU_GPIOA



/* E53_SF1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
		int Smoke_Value;
} E53_SF1_Data_TypeDef;

extern E53_SF1_Data_TypeDef E53_SF1_Data;

/***************************************************************
* 名		称: GasStatus_ENUM
* 说    明：枚举状态结构体
***************************************************************/
typedef enum
{
	OFF = 0,
	ON
} E53_SF1_Status_ENUM;

void Init_E53_SF1(void);
void E53_SF1_Read_Data(void);
void E53_SF1_LED_StatusSet(E53_SF1_Status_ENUM status);
void E53_SF1_Beep_StatusSet(E53_SF1_Status_ENUM status);

#endif





