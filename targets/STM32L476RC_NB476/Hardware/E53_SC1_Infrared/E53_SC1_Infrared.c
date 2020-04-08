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

//#include "E53_SC1.h"
#include "stm32l4xx.h"
#include "i2c.h"
#include "E53_SC1_Infrared.h"


uint8_t BUF[2];
int result;


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

void Init_Light(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
	SC1_Light_GPIO_CLK_ENABLE();
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SC1_Light_GPIO_Port, SC1_Light_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SC1_Light_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SC1_Light_GPIO_Port, &GPIO_InitStruct);

}

void Init_E53_SC1(void)
{
	MX_I2C1_Init();
	Init_Light();
	Init_BH1750();
}
/***************************************************************
* 函数名称: E53_SC1_Read_Data
* 说    明: 测量光照强度
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void E53_SC1_Read_Data(void)
{

	Start_BH1750();
	HAL_Delay(180);
	HAL_I2C_Master_Receive(&hi2c1, BH1750_Addr+1,BUF,2,0xff);
	result=BUF[0];
	result=(result<<8)+BUF[1];  //合成数据，即光照数据
	E53_SC1_Data.Lux=(float)(result/1.2);

}

/***************************************************************
* 函数名称: Init_E53_SC1_Infrared
* 说    明: 写命令初始化Infrared IO
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
