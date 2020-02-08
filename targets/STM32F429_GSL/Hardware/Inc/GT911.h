/*****************************************************************************
* | File      	:   GT911.H
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
#ifndef __GT911_H_
#define __GT911_H_


#include "Analog_I2C.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "gpio.h"

#define GT911_MAX_WIDTH		1024    	//Touchscreen pad max width
#define GT911_MAX_HEIGHT    600			//Touchscreen pad max height

#define CT_CMD_WR			0XBA		//Write data command
#define CT_CMD_RD   	    0XBB		//Read data command

//The maximum number of points supported by the capacitive touch screen
#define CT_MAX_TOUCH    5						

#define GT911_COMMAND_REG   				0x8040	/* Real-time command */
#define GT911_CONFIG_REG					0x8047	/* Configuration parameter register */
#define GT911_PRODUCT_ID_REG 				0x8140 	/* Product ID */
#define GT911_FIRMWARE_VERSION_REG          0x8144  /* Firmware version number */
#define GT911_READ_XY_REG 					0x814E	/* Coordinate register */

typedef struct
{
	uint8_t Touch;
	uint8_t TouchpointFlag;
	uint8_t TouchCount;
 
	uint8_t Touchkeytrackid[CT_MAX_TOUCH];
	uint16_t X[CT_MAX_TOUCH];
	uint16_t Y[CT_MAX_TOUCH];
	uint16_t S[CT_MAX_TOUCH];
}GT911_Dev;

void GT911_Init(void);
void GT911_Scan(void);
typedef int (*fn_touch_hook)(int x, int y);

int GT911_InstallHook(fn_touch_hook hook);



#endif /*__GT911_H_*/
