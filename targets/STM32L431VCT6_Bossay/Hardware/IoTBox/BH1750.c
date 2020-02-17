/********************************************************************************
* 文件名称 ：BH1750.c
* 作 者：Bossay IoT 开发组
* 版 本：V1.0
* 编写日期 ：2019-11-01
* 功 能：IoTBox 光感驱动
*********************************************************************************
* 说 明 ：本案例配套Bossay IoT Box开发板使用
* 网 站 ：edu.ibossay.com
* 电 话 ：4006916913 
*********************************************************************************/

#include "BH1750.h"
#include "stm32l4xx.h"
#include "i2c.h"
#include "gpio.h"



/***************************************************************
* 函数名称: Init_BH1750
* 说    明: 写命令初始化BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_BH1750(void)
{
    uint8_t t_Data = 0x01;
    HAL_I2C_Master_Transmit(&hi2c1,BH1750_Addr,&t_Data,1,0xff);
}

/***************************************************************
* 函数名称: Start_BH1750
* 说    明: 启动BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Start_BH1750(void)
{
    uint8_t t_Data = 0x10;
    HAL_I2C_Master_Transmit(&hi2c1,BH1750_Addr,&t_Data,1,0xff); 
}

/***************************************************************
* 函数名称: Convert_BH1750
* 说    明: 数值转换
* 参    数: 无
* 返 回 值: 光强值
***************************************************************/
float Convert_BH1750(void)
{
    float result_lx;
    uint8_t BUF[2];
    int result;
    Start_BH1750();
    HAL_Delay(180);
    HAL_I2C_Master_Receive(&hi2c1, BH1750_Addr+1,BUF,2,0xff); 
    result=BUF[0];
    result=(result<<8)+BUF[1];  //Synthetic Digital Illumination Intensity Data
    result_lx=(float)(result/1.2);
    return result_lx;
}

/***************************************************************
* 函数名称: IoTBox_Lux_Read_Data
* 说    明: 测量光照强度、温度、湿度
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void IoTBox_Lux_Read_Data(void)
{	
    IoTBox_Lux_Data.Lux=Convert_BH1750();													//Read bh1750 sensor data  
}
