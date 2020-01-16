/*****************************************************************************
* | File      	:   GT911.c
* | Author      :   Waveshare team
* | Function    :   GT911 driver
* | Info        :
*               GT911 driver and data processing
*----------------
* |	This version:   V1.0
* | Date        :   2019-3-5
* | Info        :

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "GT911.h"

#include <stm32f4xx_hal_gpio.h>
#include <osal.h>
#include <los_hwi.h>

UWORD x[CT_MAX_TOUCH] = {0}, y[CT_MAX_TOUCH] = {0}, z=0;
UWORD Color[CT_MAX_TOUCH] = {BLACK, BRED, BLUE, RED, BROWN};
UWORD temp=0;
GT911_Dev Dev_Now,Dev_Backup;

/******************************************************************************
function:	GT911 reset
******************************************************************************/
static void GT911_Reset()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	DEV_Set_GPIOMode(DEV_INT_PIN, 0);
	
	HAL_GPIO_WritePin(DEV_RST_PIN, GPIO_PIN_RESET);
	DEV_Delay_ms(100);
	HAL_GPIO_WritePin(DEV_INT_PIN, GPIO_PIN_RESET);
	DEV_Delay_ms(100);
	HAL_GPIO_WritePin(DEV_RST_PIN, GPIO_PIN_SET);
	DEV_Delay_ms(200);
	
	GPIO_InitStruct.Pin = INT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(INT_GPIO_Port, &GPIO_InitStruct);
	
	DEV_Delay_ms(100);
}

/******************************************************************************
function:	Write data to the GT911 register
parameter:
    reg   :   Register address
		buf		��	Data to send
		len		��	Length of data sent
******************************************************************************/
static uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	uint8_t ret=0;
	I2C_Start();	
  I2C_Send_Byte(CT_CMD_WR);   
	I2C_Wait_Ack();
	I2C_Send_Byte(reg>>8);   	
	I2C_Wait_Ack(); 	 										  		   
	I2C_Send_Byte(reg&0XFF);   	
	I2C_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    I2C_Send_Byte(buf[i]);  
		ret=I2C_Wait_Ack();
		if(ret)break;  
	}
  I2C_Stop();					
	return ret; 
}

/******************************************************************************
function:	Read data from the GT911 register
parameter:
    reg   :   Register address
		buf		��	Read data
		len		��	Read the length of the data
******************************************************************************/
static void GT911_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
 	I2C_Start();	
 	I2C_Send_Byte(CT_CMD_WR);  
	I2C_Wait_Ack();
 	I2C_Send_Byte(reg>>8);   
	I2C_Wait_Ack(); 	 										  		   
 	I2C_Send_Byte(reg&0XFF);   	
	I2C_Wait_Ack();  
 	I2C_Stop();  
	
 	I2C_Start();  	 	   
	I2C_Send_Byte(CT_CMD_RD);     
	I2C_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
		buf[i]=I2C_Read_Byte(i==(len-1)?0:1); 
	} 
  I2C_Stop();   
}

/******************************************************************************
function:	Read the ID of the GT911
******************************************************************************/
static uint8_t GT911_Read_ID(void)
{
	uint8_t buf[4];
	GT911_RD_Reg(GT911_PRODUCT_ID_REG, (uint8_t *)&buf[0], 3);
	GT911_RD_Reg(GT911_CONFIG_REG, (uint8_t *)&buf[3], 1);
	printf("TouchPad_ID:%c,%c,%c\r\nTouchPad_Config_Version:%2x\r\n",buf[0],buf[1],buf[2],buf[3]);
	return buf[3];
}

/******************************************************************************
function:	Read the firmware version of the GT911
******************************************************************************/
static uint16_t GT911_Read_FirmwareVersion(void)
{
	uint8_t buf[2];

	GT911_RD_Reg(GT911_FIRMWARE_VERSION_REG, buf, 2);

	printf("FirmwareVersion:%2x\r\n",(((uint16_t)buf[1] << 8) + buf[0]));
	return ((uint16_t)buf[1] << 8) + buf[0];
}


static fn_touch_hook  s_fn_touch_hook = NULL;

int GT911_InstallHook(fn_touch_hook hook)
{
    s_fn_touch_hook = hook;

    return 0;
}



/******************************************************************************
function:	Read the firmware version of the GT911
******************************************************************************/
void GT911_Scan(void)
{
	uint8_t buf[41];
    uint8_t Clearbuf = 0;

	//Determine if the interruption is generated
	if (Dev_Now.Touch == 1)
	{
		Dev_Now.Touch = 0;
		GT911_RD_Reg(GT911_READ_XY_REG, buf, 1);		
		
		if ((buf[0]&0x80) == 0x00)
		{
			GT911_WR_Reg(GT911_READ_XY_REG, (uint8_t *)&Clearbuf, 1);
			DEV_Delay_ms(10);
		}
		else
		{
			Dev_Now.TouchpointFlag = buf[0];
			Dev_Now.TouchCount = buf[0]&0x0f;
			if (Dev_Now.TouchCount > 5 || Dev_Now.TouchCount  == 0)
			{
				GT911_WR_Reg(GT911_READ_XY_REG, (uint8_t *)&Clearbuf, 1);
				return ;
			}
			
			if(temp < 5){//Prevent misjudgment
				temp = 1 + temp;
				if(temp == 4)
//					Paint_Clear(WHITE);
				return;
			}
			
			GT911_RD_Reg(GT911_READ_XY_REG+1, &buf[1], Dev_Now.TouchCount*8);
			GT911_WR_Reg(GT911_READ_XY_REG, (uint8_t *)&Clearbuf, 1);
			
			Dev_Now.Touchkeytrackid[0] = buf[1];
			Dev_Now.X[0] = ((uint16_t)buf[3] << 8) + buf[2];
			Dev_Now.Y[0] = ((uint16_t)buf[5] << 8) + buf[4];
			Dev_Now.S[0] = ((uint16_t)buf[7] << 8) + buf[6];

			Dev_Now.Touchkeytrackid[1] = buf[9];
			Dev_Now.X[1] = ((uint16_t)buf[11] << 8) + buf[10];
			Dev_Now.Y[1] = ((uint16_t)buf[13] << 8) + buf[12];
			Dev_Now.S[1] = ((uint16_t)buf[15] << 8) + buf[14];

			Dev_Now.Touchkeytrackid[2] = buf[17];
			Dev_Now.X[2] = ((uint16_t)buf[19] << 8) + buf[18];
			Dev_Now.Y[2] = ((uint16_t)buf[21] << 8) + buf[20];
			Dev_Now.S[2] = ((uint16_t)buf[23] << 8) + buf[22];

			Dev_Now.Touchkeytrackid[3] = buf[25];
			Dev_Now.X[3] = ((uint16_t)buf[27] << 8) + buf[26];
			Dev_Now.Y[3] = ((uint16_t)buf[29] << 8) + buf[28];
			Dev_Now.S[3] = ((uint16_t)buf[31] << 8) + buf[30];

			Dev_Now.Touchkeytrackid[4] = buf[33];
			Dev_Now.X[4] = ((uint16_t)buf[35] << 8) + buf[34];
			Dev_Now.Y[4] = ((uint16_t)buf[37] << 8) + buf[36];
			Dev_Now.S[4] = ((uint16_t)buf[39] << 8) + buf[38];
			
			//Get coordinates and display coordinates
			
//			//Clear the last redundant display coordinates
//			if(z > Dev_Now.TouchCount){
//				for(i = z; i != Dev_Now.TouchCount; i--){
//					Paint_DrawLine(0 , y[i-1]+3, 800, y[i-1]+3, WHITE, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
//					Paint_DrawLine(x[i-1]+3, 0, x[i-1]+3, 480, WHITE, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
//					Paint_DrawCircle( x[i-1]+3 , y[i-1]+3 , 8, WHITE, DRAW_FILL_EMPTY, DOT_PIXEL_1X1);
//				}
//			}
			
			if((Dev_Now.TouchCount > 0) &&(NULL != s_fn_touch_hook))
			{
			    s_fn_touch_hook(Dev_Now.X[0],Dev_Now.Y[0]);
			}

//			for (i=0;i<Dev_Now.TouchCount;i++)
//			{
//				Dev_Backup.X[i] = Dev_Now.X[i];
//				Dev_Backup.Y[i] = Dev_Now.Y[i];
//				Dev_Backup.TouchCount = Dev_Now.TouchCount;
//				printf("x = %d   y=  %d \r\n",Dev_Now.X[i], Dev_Now.Y[i]);
//
////				//Clear the last displayed coordinates
//				if(Dev_Now.X[i]>385&Dev_Now.X[i]<415 )
//					{
//					Paint_DrawCircle(50,400,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//					Paint_DrawCircle(50,440,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//					Paint_DrawCircle(50,480,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//				}
//			  if(Dev_Now.X[i]>425&Dev_Now.X[i]<455 )
//					{
//					Paint_DrawCircle(50,400,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//					Paint_DrawCircle(50,440,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//					Paint_DrawCircle(50,480,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//				}
//			  if(Dev_Now.X[i]>465&Dev_Now.X[i]<495 )
//					{
//					Paint_DrawCircle(50,400,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//					Paint_DrawCircle(50,440,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//					Paint_DrawCircle(50,480,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
//				}
////				//Display coordinates
////				Paint_DrawLine(Dev_Now.X[i]+3, 0, Dev_Now.X[i]+3, 480, Color[i], LINE_STYLE_SOLID, DOT_PIXEL_1X1);
////				Paint_DrawLine(0 , Dev_Now.Y[i]+3, 800, Dev_Now.Y[i]+3, Color[i], LINE_STYLE_SOLID, DOT_PIXEL_1X1);
////				Paint_DrawCircle(Dev_Now.X[i]+3 , Dev_Now.Y[i]+3 , 8, Color[i], DRAW_FILL_EMPTY, DOT_PIXEL_1X1);
//
//				x[i]  = Dev_Now.X[i];
//				y[i]  = Dev_Now.Y[i];
//			}
			z = Dev_Now.TouchCount;
		}
	}
}


static int scan_entry(void *arg)
{
   while(1)
   {
       GT911_Scan();
       osal_task_sleep(1);
   }

   return 0;
}





void EXTI15_10_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10)!=RESET)    //ST2_S1&#214;D&#182;&#207;
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11)!=RESET)    //ST2_S2&#214;D&#182;&#207;
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12)!=RESET)    //ST3_S1&#214;D&#182;&#207;
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13)!=RESET)    //ST3_S2&#214;D&#182;&#207;
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
        Dev_Now.Touch =1;
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14)!=RESET)    //ST4_S1&#214;D&#182;&#207;
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
    }
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15)!=RESET)    //ST4_S2&#214;D&#182;&#207;
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
    }
}






void GT911_Init(void)
{
    printf("GT911_Init\r\n");

//    osal_task_create("touch",GT911_Scan,NULL,0x1000,NULL,10);
    osal_task_create("touch",scan_entry,NULL,0x800,NULL,7);

    LOS_HwiCreate(EXTI15_10_IRQn, 3, 0, EXTI15_10_IRQHandler, 0);
	GT911_Reset();
	GT911_Read_ID();
	GT911_Read_FirmwareVersion();
    printf("GT911_Init end\r\n");

}
