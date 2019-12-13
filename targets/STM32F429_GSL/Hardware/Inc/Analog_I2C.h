/*****************************************************************************
* | File      	:   Analog_I2C.H
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
#ifndef __CT_I2C_H_
#define __CT_I2C_H_


#include "../../../STM32F429_GSL/Hardware/Inc/DEV_Config.h"



void 		I2C_Init(void);		 
void 		I2C_Start(void);				
void 		I2C_Stop(void);	  			
void 		I2C_Send_Byte(uint8_t data);			
uint8_t I2C_Read_Byte(unsigned char ack);	
uint8_t I2C_Wait_Ack(void); 			
void 		I2C_Ack(void);					
void 		I2C_NAck(void);				

#endif /*__CAP_I2C_H_*/
