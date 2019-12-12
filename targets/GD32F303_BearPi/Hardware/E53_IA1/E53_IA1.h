/********************************************************************************
    * 文件名称 ：E53_IA1.h
    * 作     者：物联网俱乐部
    * 版     本：V1.0
    * 编写日期 ：2019-5-31
    * 功     能：E53_IA1扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派-IoT(GD)开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
*********************************************************************************/

#ifndef __E53_IA1_H__
#define __E53_IA1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "gd32f30x.h"

/* 控制设备IO口定义 ------------------------------------------------------------*/

#define IA1_Motor_Pin GPIO_PIN_8
#define IA1_Motor_GPIO_Port GPIOB
#define IA1_Motor_GPIO_CLK RCU_GPIOB
#define IA1_Light_Pin GPIO_PIN_9
#define IA1_Light_GPIO_Port GPIOC
#define IA1_Light_GPIO_CLK RCU_GPIOC

/* E53_IA1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{
    float Lux;         //光照强度
    float Humidity;    //湿度
    float Temperature; //温度
} E53_IA1_Data_TypeDef;

extern E53_IA1_Data_TypeDef E53_IA1_Data;

/***************************************************************
 * 名		称: GasStatus_ENUM
 * 说    明：枚举状态结构体
 ***************************************************************/
typedef enum
{
    OFF = 0,
    ON
} E53_IA1_Status_ENUM;

/* 寄存器宏定义 --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS 0x0A

#define SHT30_Addr 0x44

#define BH1750_Addr 0x46
#define BH1750_ON 0x01
#define BH1750_CON 0x10
#define BH1750_ONE 0x20
#define BH1750_RSET 0x07

void Init_E53_IA1(void);
void E53_IA1_Read_Data(void);
void E53_IA1_Light_StatusSet(E53_IA1_Status_ENUM status);
void E53_IA1_Motor_StatusSet(E53_IA1_Status_ENUM status);

#endif
