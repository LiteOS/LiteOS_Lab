/*************************************************
* �ļ����� ��main.c
* ��     �ߣ����������ֲ�
* ��     ����V1.0
* ��д���� ��2018-4-1
* ��     �ܣ������������񴴽������
*********************************************************************
* ˵    �� ���������������������ֲ�EVB_M1������ʹ��
*
* ��     ����https://shop128001708.taobao.com/
* ��     ̳��bbs.iot-club.cn
*********************************************************************************/
#ifndef __BH1750_H__
#define __BH1750_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS                            0x0A

#define I2C_WR	        0		/* д����bit */
#define I2C_RD	        1		/* ������bit */

#define I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define I2C_GPIO_PORT                       GPIOC   
#define I2C_SCL_PIN                         GPIO_PIN_11
#define I2C_SDA_PIN                         GPIO_PIN_10

/* �ߵ͵�ƽ*/  
#define HIGH    1  
#define LOW     0 


#define I2C_SCL(n) (n?HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SCL_PIN,GPIO_PIN_RESET)) //SCL  
#define I2C_SDA(n) (n?HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(I2C_GPIO_PORT,I2C_SDA_PIN,GPIO_PIN_RESET)) //SDA 
#define I2C_SDA_READ()               HAL_GPIO_ReadPin(I2C_GPIO_PORT,I2C_SDA_PIN)

#define BH1750_Addr 0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_ONE  0x20
#define BH1750_RSET 0x07

#define SDA_IN()  {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=0<<10*2;}       //PH5????  
#define SDA_OUT() {GPIOC->MODER&=~(3<<(10*2));GPIOC->MODER|=1<<10*2;}   //PH5???? 


void    I2C_Start(void);
void    I2C_Stop(void);
uint8_t I2C_WaitAck(void);
void    I2C_Ack(void);
void    I2C_NAck(void);
void    I2C_SendByte(uint8_t Byte);
uint8_t I2C_ReadByte(uint8_t  ack);
uint8_t I2C_CheckDevice(uint8_t _Address);


void Init_BH1750(void);								  //IO��ʼ����
void Start_BH1750(void);			//�ϵ磬����������ݼĴ���
void Read_BH1750(void);                               //�����Ķ�ȡ�ڲ��Ĵ�������
float Convert_BH1750(void);
#endif





