/********************************************************************************
    * 文件名称 ：E53_SC1.h
    * 作     者：物联网俱乐部
    * 版     本：V1.0
    * 编写日期 ：2019-5-31
    * 功     能：E53_SC1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派-IoT(GD)开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
*********************************************************************************/

#ifndef __E53_SC1_H__
#define __E53_SC1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "gd32f30x.h"

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define SC1_Light_Pin GPIO_PIN_9
#define SC1_Light_GPIO_Port GPIOB
#define SC1_Light_GPIO_CLK  RCU_GPIOB

/* E53_IA1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
		float    Lux;							//光照强度
} E53_SC1_Data_TypeDef;

extern E53_SC1_Data_TypeDef E53_SC1_Data;

/***************************************************************
* 名		称: GasStatus_ENUM
* 说    明：枚举状态结构体
***************************************************************/
typedef enum
{
	OFF = 0,
	ON
} E53_SC1_Status_ENUM;

/* 寄存器宏定义 --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS                            0x0A

#define BH1750_Addr 0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_RSET 0x07

void Init_E53_SC1(void);
void E53_SC1_Read_Data(void);
void E53_SC1_LED_StatusSet(E53_SC1_Status_ENUM status);

#endif





