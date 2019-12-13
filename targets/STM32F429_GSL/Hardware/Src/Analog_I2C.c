/*****************************************************************************
* | File      	:   Analog_I2C.c
* | Author      :   Waveshare team
* | Function    :   Analog I2C driver
* | Info        :
*               Analog I2C driver
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
#include "Analog_I2C.h"

/******************************************************************************
function:	Subtle level delay
parameter:
			us	£º Waiting time unit us
******************************************************************************/


/******************************************************************************
function:	I2C protocol start signal
******************************************************************************/
void I2C_Start(void)
{
	DEV_Set_GPIOMode(DEV_SDA_PIN, 0);
	
	DEV_Digital_Write(DEV_SDA_PIN, 1);
	DEV_Delay_us(1);	
	
	DEV_Digital_Write(DEV_SCL_PIN, 1);
	DEV_Delay_us(1);
	
 	DEV_Digital_Write(DEV_SDA_PIN, 0);
	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SCL_PIN, 0);
	DEV_Delay_us(1);
}	  

/******************************************************************************
function:	I2C protocol end signal
******************************************************************************/
void I2C_Stop(void)
{
	DEV_Set_GPIOMode(DEV_SDA_PIN, 0);
	
	DEV_Digital_Write(DEV_SDA_PIN, 0);
 	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SCL_PIN, 1);
	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SDA_PIN, 1);
	DEV_Delay_us(1);							   	
}

/******************************************************************************
function:	I2C protocol sends a byte
parameter:
			data	£º Sent data
******************************************************************************/
void I2C_Send_Byte(UBYTE data)
{                        
	UBYTE t; 
  
	DEV_Set_GPIOMode(DEV_SDA_PIN, 0);
  DEV_Digital_Write(DEV_SCL_PIN, 0);
		
	for(t = 0; t < 8; t++){              
		if(data & 0x80){
			DEV_Digital_Write(DEV_SDA_PIN, 1);
		}else{
			DEV_Digital_Write(DEV_SDA_PIN, 0);
		}
		data <<= 1; 	  
		DEV_Delay_us(1); 

		DEV_Digital_Write(DEV_SCL_PIN, 1);
		DEV_Delay_us(1); 
		DEV_Digital_Write(DEV_SCL_PIN, 0);
		DEV_Delay_us(1);
	}	 

	DEV_Digital_Write(DEV_SDA_PIN, 1);
	DEV_Delay_us(1);
	DEV_Digital_Write(DEV_SCL_PIN, 0);
	DEV_Delay_us(1);
} 

/******************************************************************************
function:	I2C protocol reads one byte
parameter:
			ack	£º Whether to answer 1 Answer ,0 Not answering
return:
			Read data
******************************************************************************/
UBYTE I2C_Read_Byte(unsigned char ack)
{
	UBYTE i,receive=0;

 	DEV_Set_GPIOMode(DEV_SDA_PIN, 1);

  for(i = 0; i < 8; i++ ){
    DEV_Digital_Write(DEV_SCL_PIN, 0);
		DEV_Delay_us(1);
		DEV_Digital_Write(DEV_SCL_PIN, 1);
    receive <<= 1;
    if(DEV_Digital_Read(DEV_SDA_PIN))receive++;
		DEV_Delay_us(1);
	}	

	DEV_Digital_Write(DEV_SCL_PIN, 0);
	if (!ack){
		I2C_NAck();
	}else {
		I2C_Ack(); 
	}
 
	return receive;
}

/******************************************************************************
function:	I2C protocol waits for a response
return:
			1£ºNo answer
			0£ºHave a response
******************************************************************************/
UBYTE I2C_Wait_Ack(void)
{
	UBYTE ucErrTime=0;

	DEV_Set_GPIOMode(DEV_SDA_PIN, 1);
	DEV_Digital_Write(DEV_SDA_PIN, 1);
	DEV_Delay_us(1);	   
	DEV_Digital_Write(DEV_SCL_PIN, 1);
	DEV_Delay_us(1);	
	
	while(DEV_Digital_Read(DEV_SDA_PIN)){
		ucErrTime++;
		if(ucErrTime > 250){
			I2C_Stop();
			return 1;
		}
	}
	DEV_Digital_Write(DEV_SCL_PIN, 0);
	return 0;  
} 

/******************************************************************************
function:	I2C response signal
******************************************************************************/
void I2C_Ack(void)
{
	DEV_Set_GPIOMode(DEV_SDA_PIN, 0);
	DEV_Digital_Write(DEV_SCL_PIN, 0);
	
	DEV_Digital_Write(DEV_SDA_PIN, 0);
	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SCL_PIN, 1);
	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SCL_PIN, 0);
	DEV_Delay_us(1);
}

/******************************************************************************
function:	I2C no response signal
******************************************************************************/
void I2C_NAck(void)
{
	DEV_Set_GPIOMode(DEV_SDA_PIN, 0);

	DEV_Digital_Write(DEV_SDA_PIN, 1);
	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SCL_PIN, 1);
	DEV_Delay_us(1);
	
	DEV_Digital_Write(DEV_SCL_PIN, 0);
	DEV_Delay_us(1);
}					 				     
