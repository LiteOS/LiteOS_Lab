#include <stdio.h>
#include "gd32vf103.h"
#include "gd32v103v_eval.h"
#include "gd32v103v_lcd_eval.h"
#include "touch_panel.h"
#include "picture.h"
#include "systick.h"

#include "los_base.h"
#include "los_typedef.h"

#include "mem.h"

#include "osal.h"

#if defined (__CC_ARM)
extern char __heap_start__ [];
#elif defined (__GNUC__)
extern char __los_heap_addr_start__ [];
extern char __los_heap_addr_end__ [];
#else
#error "fix me"
#endif

extern VOID LOS_MemInfo (bool bShowDetail);
extern int link_main(void *args);


const struct phys_mem system_phys_mem [] =
    {
#if defined (__CC_ARM)
        { __heap_start__, (char *) 0x2002FC00, },
#elif defined (__GNUC__)
        { __los_heap_addr_start__, __los_heap_addr_end__, },
#else
#error "fix me"
#endif
        { 0, 0 }
    };

UINT16 a1=20;
UINT16 a2=120;
UINT16 a3=220;
UINT16 b1=10;
UINT16 b2=100;
UINT16 b3=200;
UINT16 device_code;
UINT8 error_string[]="Please press the button!";
char_format_struct char_format;
#define  SFLASH_ID                     0xC84015


/*!
	\brief		find the position of max value in array
	\param[in]	none
	\param[out] none
	\retval 	the position of max value in num_array array
*/
UINT8 find_max(UINT16* num_array)
{
	int id;
	id=0;
	if(num_array[id]<num_array[1]){
		id=1;
	}
	if(num_array[id]<num_array[2]){
		id=2;
	}
	if(num_array[id]<num_array[3]){
		id=3;
	}
	if(num_array[id]==0){
		id=4;
	}
	return id;
}

/*!
	\brief		get the touch area
	\param[in]	x: the row-coordinate
	\param[in]	y: the column-coordinate
	\param[out] num_array: touch area array pointer
	\retval 	none
*/
void get_touch_area(UINT16 x,UINT16 y,UINT16 *num_array)
{
	  if((x<a1+70)&&(x>a1+30)&&(y>b1+30)&&(y<b1+70)){
		  num_array[0]++;
	  }else if((x<a2+70)&&(x>a2+30)&&(y>b1+30)&&(y<b1+70)){
		  num_array[1]++;
	  }else if((x<a1+70)&&(x>a1+30)&&(y>b2+30)&&(y<b2+70)){
		  num_array[2]++;
	  }else if((x<a2+70)&&(x>a2+30)&&(y>b2+30)&&(y<b2+70)){
		  num_array[3]++;
	  }
}

/*!
	\brief		change the picture of LED
	\param[in]	button_id: the id of button you just pressed
	\param[out] none
	\retval 	none
*/
void change_picture(UINT8 button_id)
{
	UINT8 len_s;
	UINT8 *str;
	UINT16 i;
	
	/* draw picture on LCD screen */
	lcd_picture_draw(a1+30,b1+40,a1+30+40-1,b1+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
	lcd_picture_draw(a2+30,b1+40,a2+30+40-1,b1+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
	lcd_picture_draw(a1+30,b2+40,a1+30+40-1,b2+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
	lcd_picture_draw(a2+30,b2+40,a2+30+40-1,b2+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
	/* draw character on LCD screen */
	len_s=sizeof(error_string)-1;
	str = error_string;
	
	char_format.char_color = LCD_COLOR_WHITE;
	for (i = 0; i < len_s; i++){
		lcd_char_display((30+8*i), 190, *str++, char_format);
	}
	
	switch(button_id){
		case 0:
			lcd_picture_draw(a1+30,b1+40,a1+30+40-1,b1+40+40-1,(UINT16 *)(image_on + BMP_HEADSIZE));
			break;
		case 1:
			lcd_picture_draw(a2+30,b1+40,a2+30+40-1,b1+40+40-1,(UINT16 *)(image_on + BMP_HEADSIZE));
			break;
		case 2:
			lcd_picture_draw(a1+30,b2+40,a1+30+40-1,b2+40+40-1,(UINT16 *)(image_on + BMP_HEADSIZE));
			break;
		case 3:
			lcd_picture_draw(a2+30,b2+40,a2+30+40-1,b2+40+40-1,(UINT16 *)(image_on + BMP_HEADSIZE));
			break;
		default:
			str = error_string;
			char_format.char_color = LCD_COLOR_RED;
			for (i = 0; i < len_s; i++){
				lcd_char_display((30+8*i), 190, *str++, char_format);
			}
			break;
	}
}

/*!
	\brief		turn on the LED according the pressed button
	\param[in]	button_id: the id of button you just pressed
	\param[out] none
	\retval 	none
*/
void turn_on_led(UINT8 button_id)
{
	/* firstly,turn off all the LEDs */
	gd_eval_led_off(LED1);
	gd_eval_led_off(LED2);
	gd_eval_led_off(LED3);
	gd_eval_led_off(LED4);
	
	switch(button_id){
		case 0:
			gd_eval_led_on(LED1);  /*!< turn on LED2 only */
			break;
		case 1:
			gd_eval_led_on(LED2);  /*!< turn on LED3 only */
			break;
		case 2:
			gd_eval_led_on(LED3);  /*!< turn on LED4 only */
			break;
		case 3:
			gd_eval_led_on(LED4);  /*!< turn on LED5 only */
			break;
		default:
			/* turn on all the LEDs */
			gd_eval_led_on(LED1);
			gd_eval_led_on(LED2);
			gd_eval_led_on(LED3);
			gd_eval_led_on(LED4);
			break;
	}
}



void eclic_mtip_handler(void)
{
    *(UINT64 *)(TMR_CTRL_ADDR + TMR_MTIME) = 0;
    /* vPortSetupTimerInterrupt() uses LPIT0 to generate the tick interrupt. */
    osTickHandler();
}


static UINT32 g_TaskID1, g_TaskID2, g_TaskID3;
LITE_OS_SEC_TEXT VOID TEST1(VOID)
{
    while (1)
    {
        printf("test1\n");
        LOS_TaskDelay(10);
    }
}

LITE_OS_SEC_TEXT VOID TEST2(VOID)
{
    while (1)
    {
        printf("test2\n");
        LOS_TaskDelay(20);
    }
}

LITE_OS_SEC_TEXT VOID TEST3(VOID)
{
    while (1)
    {
        printf("test3\n");
        LOS_TaskDelay(30);
    }
}

void LOS_BoadExampleEntry(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stTaskInitParam;

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST1;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test1";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID1, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST2;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test2";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID2, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TEST3;
    stTaskInitParam.uwStackSize = 0x130;
    stTaskInitParam.pcName = "Test3";
    stTaskInitParam.usTaskPrio = 2;
    uwRet = LOS_TaskCreate(&g_TaskID3, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        return;
    }

    return;
}


static int link_test()
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S task_init_param;

    memset (&task_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task_init_param.uwArg = (unsigned int)NULL;
    task_init_param.usTaskPrio = 2;
    task_init_param.pcName =(char *) "link_main";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)link_main;
    task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &task_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
}

#ifndef CONFIG_HELLO_WORLD_ENABLE

extern VOID lcd_handle(VOID);

static int lcd_demo()
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S task_init_param;

    UINT16 i;
    UINT8 led_string[4][4]={"LED1","LED2","LED3","LED4"} ;
    
    /* initialize LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
    
    /* configure the EXMC access mode */
    exmc_lcd_init();

    /* configure SPI0 GPIO and parameter */
    spi_flash_init();
    if(SFLASH_ID != spi_flash_read_id()){
        while(1);
    }

    delay_1ms(50);  

    /* read the LCD controller device code:(0x8989) */
    device_code = lcd_register_read(0x0000);

    /* initialize the LCD */
    lcd_init();

    /* clear the LCD screen */
    lcd_clear(LCD_COLOR_WHITE);

    /* draw the picture of Gigadevice logo */
    /* if you don't want to draw the picture, you should modify the macro on
       the line 422th of picture.c file and comment the next line */
    //lcd_picture_draw(40,200,40+160-1,200+87-1,(UINT16 *)(picture + BMP_HEADSIZE));
    lcd_picture_draw_ex(40,200,40+160-1,200+87-1,0);

    /* configure the GPIO of SPI touch panel */
    touch_panel_gpio_configure();


    /* draw a rectangle */
    lcd_rectangle_draw(10,10,230,310,LCD_COLOR_BLUE);

    /* configure char format */
    char_format.char_color = LCD_COLOR_BLUE;
    char_format.bk_color = LCD_COLOR_WHITE;
    char_format.direction = CHAR_DIRECTION_VERTICAL;
    char_format.font = CHAR_FONT_8_16;
    
    /* draw character on LCD screen */
    for (i = 0; i < 4; i++){
        lcd_char_display((a1+35+8*i), b1+20, *(led_string[0]+i), char_format);
        lcd_char_display((a2+35+8*i), b1+20, *(led_string[1]+i), char_format);
        lcd_char_display((a1+35+8*i), b2+20, *(led_string[2]+i), char_format);
        lcd_char_display((a2+35+8*i), b2+20, *(led_string[3]+i), char_format);
    }
    
    /* draw picture of button on LCD screen */
    lcd_picture_draw(a1+30,b1+40,a1+30+40-1,b1+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
    lcd_picture_draw(a2+30,b1+40,a2+30+40-1,b1+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
    lcd_picture_draw(a1+30,b2+40,a1+30+40-1,b2+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));
    lcd_picture_draw(a2+30,b2+40,a2+30+40-1,b2+40+40-1,(UINT16 *)(image_off + BMP_HEADSIZE));




    memset (&task_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task_init_param.uwArg = (unsigned int)NULL;
    task_init_param.usTaskPrio = 2;
    task_init_param.pcName =(char *) "link_main";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)lcd_handle;
    task_init_param.uwStackSize = 0x1200;
    uwRet = LOS_TaskCreate(&handle, &task_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;


}
#endif

int main() {
    /* configure EVAL_COM0 */
    gd_eval_com_init(EVAL_COM0);
    /* output a message on hyperterminal using printf function */
    printf("0x%x---0x%x\r\n", __los_heap_addr_start__, __los_heap_addr_end__ );

    UINT32 uwRet = LOS_OK;


    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //LOS_MemInfo(1);
    //LOS_BoadExampleEntry();
#ifndef CONFIG_HELLO_WORLD_ENABLE
    lcd_demo();
#endif
    link_test();

    (void)LOS_Start();
    return 0;

}
