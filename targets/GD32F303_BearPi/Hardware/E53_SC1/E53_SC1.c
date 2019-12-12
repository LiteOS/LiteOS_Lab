/********************************************************************************
    * 文件名称 ：E53_SC1.c
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

#include "E53_SC1.h"
#include "gd32f30x.h"
#include "i2c.h"


uint8_t BUF[2];
int result;

void delay_us(uint32_t n)
{
	uint8_t j;
	while(n--)
	for(j=0;j<10;j++);
}
void delay_ms(uint32_t n)
{
	while(n--)
	delay_us(1000);
}

void BH1750_I2C_Master_Transmit(uint8_t slaveAddr,uint8_t *pBuffer,uint8_t NumByteToRead) 
{
	  /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, slaveAddr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	
	  for(int i = 0;i < NumByteToRead;i++) 
    {
      /* data transmission */
      i2c_data_transmit(I2C0, pBuffer[i]);
      /* wait until the TBE bit is set */
      while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	  }
	  /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while(I2C_CTL0(I2C0)&0x0200);
}

void BH1750_I2C_Master_Receive(uint8_t slaveAddr, uint8_t* pBuffer, uint8_t NumByteToRead)
{
  	/* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, slaveAddr, I2C_TRANSMITTER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    /* wait until the transmit data buffer is empty */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
	  /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);
    while(I2C_CTL0(I2C0)&0x0200);
	
	  /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
	  /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, slaveAddr, I2C_RECEIVER);
	  /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	
	  /* While there is data to be read */
	  while(NumByteToRead) {
		  if(NumByteToRead == 1) {
			/* Disable Acknowledgement */
			i2c_ack_config(I2C0, I2C_ACK_DISABLE);	

			/* Send STOP Condition */
			i2c_stop_on_bus(I2C0);
		}
		/* wait until the RBNE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
		/* Read a byte from the MPU6050 */
		*pBuffer = i2c_data_receive(I2C0);
		/* Point to the next location where the byte read will be saved */
		pBuffer++;
		/* Decrement the read bytes counter */
		NumByteToRead--;
	}
	i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

/***************************************************************
* 函数名称: Init_BH1750
* 说    明: 写命令初始化BH1750
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_BH1750(void)
{
	uint8_t t_Data = 0x01;
  /* configure GPIO */
  gpio_config();
  /* configure I2C */
  i2c_config();

	BH1750_I2C_Master_Transmit(BH1750_Addr,&t_Data,1);
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
	BH1750_I2C_Master_Transmit(BH1750_Addr,&t_Data,1); 
}

void Init_Light(void)
{
  /* enable the led clock */
  rcu_periph_clock_enable(SC1_Light_GPIO_CLK);
  /* configure led GPIO port */
  gpio_init(SC1_Light_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SC1_Light_Pin);
	/*configure GPIO pin Output Level */
  gpio_bit_reset(SC1_Light_GPIO_Port, SC1_Light_Pin);

}

void Init_E53_SC1(void)
{
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
	delay_ms(180);
	BH1750_I2C_Master_Receive(BH1750_Addr+1,BUF,2); 
	result=BUF[0];
	result=(result<<8)+BUF[1];  //合成数据，即光照数据	
	E53_SC1_Data.Lux=(float)(result/1.2);
    
}
/***************************************************************
* 函数名称: E53_SC1_LED_StatusSet
* 说    明: E53_SC1开发板上的LED灯的亮灭控制
* 参    数: status,LED灯的状态
*									OFF,关灯
*									ON,开灯
* 返 回 值: 无
***************************************************************/
void E53_SC1_LED_StatusSet(E53_SC1_Status_ENUM status)
{
  if(status == ON)
    gpio_bit_set(SC1_Light_GPIO_Port, SC1_Light_Pin);
  else
    gpio_bit_reset(SC1_Light_GPIO_Port, SC1_Light_Pin);
}