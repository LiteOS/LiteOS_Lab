/********************************************************************************
* 文件名称 ：BS_IA_DEMO.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IA_Demo驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/

#include "DEMO.h"
#include "stm32l4xx.h"
#include "i2c.h"
#include "gpio.h"
#include "BH1750.h"
#include "SHT30.h"
#include "Actuators.h"

void Init_BS_IA_DEMO(void)
{
    MX_I2C1_Init();
    MX_I2C2_Init();
    Init_BH1750();
    Init_SHT30();
    Init_Motor();
    Init_Light();
}


