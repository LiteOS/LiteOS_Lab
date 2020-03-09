/********************************************************************************
* 文件名称 ：BH1750.h
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 光感驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/
#ifndef __BH1750_H__
#define __BH1750_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


/* 寄存器宏定义 --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS                            0x0A
#define SHT30_Addr 0x44
#define BH1750_Addr 0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_RSET 0x07


/* 光强传感器数据类型定义 ------------------------------------------------------------*/
typedef struct
{							
	float    Lux;       //光强
}IoTBox_Lux_Data_TypeDef;

extern IoTBox_Lux_Data_TypeDef IoTBox_Lux_Data;
void Init_BH1750(void);
void IoTBox_Lux_Read_Data(void);


#endif

