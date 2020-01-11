/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
/**
 *  DATE                AUTHOR      INSTRUCTION
 *  2019-12-12 09:47  zhangqianfu  The first version  
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <osal.h>
#include "BSP_RGB_LCD.h"   ///< LCD DRIVER
#include "BSP_SDRAM.h"
#include "GT911.h"
#include "GUI_Paint.h"     ///< method to draw
#include "image.h"

#include <ec2x_oc.h>


#define CN_PAIN_W  480
#define CN_PAIN_H  800


typedef struct
{
   int x;
   int y;
   int w;
   int h;
   fn_touch_hook  hook;
}ui_touch_t;

#define CN_TOUCH_HOOK_NUM   4
static ui_touch_t  s_ui_touch[CN_TOUCH_HOOK_NUM];

///< do the touch filter here
static int ui_touch(int x_t,int y_t)
{
    int i = 0;
    int y;
    int x;
    static unsigned long long time_last = 0;
    unsigned long long time_now = 0;

    ///< do the change
    x= CN_PAIN_W - y_t;
    y= x_t;

    time_now = osal_sys_time();
    if((time_now - time_last) <= 500)
    {
        printf("FILTER TOUCH:X_T:%d Y_T:%d X:%d Y:%d %d \n\r",x_t,y_t,x,y,(int)time_now);
        time_last = time_now;
        return 0;
    }
    time_last = time_now;
    printf("TOUCH:X_T:%d Y_T:%d X:%d Y:%d  %d \n\r",x_t,y_t,x,y,(int)time_now);

    ///< find the match one and execute
    for(i = 0;i< CN_TOUCH_HOOK_NUM;i++)
    {
        if((NULL != s_ui_touch[i].hook) && \
           (x> s_ui_touch[i].x) && (x < (s_ui_touch[i].x + s_ui_touch[i].w))&&\
           (y> s_ui_touch[i].y) && (y < (s_ui_touch[i].y + s_ui_touch[i].h)))
        {
            s_ui_touch[i].hook(x,y);
        }
    }

    return 0;
}

static int ui_touch_register(ui_touch_t *touch)
{
    int i = 0;
    int ret = -1;
    for(i = 0;i< CN_TOUCH_HOOK_NUM;i++)
    {
        if(NULL == s_ui_touch[i].hook)
        {
            s_ui_touch[i] = *touch;
            ret = 0;
            break;
        }
    }

    return ret;
}



static void ui_clean_window(int x, int y, int w,int h)
{

    int x_s,x_e,y_s,y_e;

    x_s = x;
    x_e = x_s + w;
    y_s = y;
    y_e = y_s + h;

    Paint_ClearWindows(x_s,y_s,x_e,y_e,WHITE);
}


#define CN_UI_TIME_X_S   20
#define CN_UI_TIME_Y_S   650
#define CN_UI_TIME_W     460
#define CN_UI_TIME_H     50
#define CN_UI_TIME_FRESH (1000*20)


static int ui_time_entry(void *arg)
{
    char time_now[128] ;

    while(1)
    {
        ///< time fresh
        memset(time_now,0,128);
        if(0 == ec2x_get_time(time_now))
        {
            ui_clean_window(CN_UI_TIME_X_S,CN_UI_TIME_Y_S,CN_UI_TIME_W,CN_UI_TIME_H);
            Paint_DrawString_EN(CN_UI_TIME_X_S, CN_UI_TIME_Y_S, time_now, &Font24, WHITE, BLACK);
            printf("%s:time_fresh OK\n\r",__FUNCTION__);
        }
        else
        {
            printf("%s:time_fresh ERR\n\r",__FUNCTION__);

        }

        osal_task_sleep(CN_UI_TIME_FRESH);
    }

    return 0;
}

#define CN_LINE_H                3
#define CN_UI_CSQ_IMG_X_S        20
#define CN_UI_CSQ_IMG_Y_S        160
#define CN_UI_CSQ_IMG_W          80
#define CN_UI_CSQ_IMG_H          60

#define CN_UI_RSSI_X_S      CN_UI_CSQ_IMG_X_S
#define CN_UI_RSSI_Y_S      (CN_UI_CSQ_IMG_Y_S + CN_UI_CSQ_IMG_H)
#define CN_UI_RSSI_W        240
#define CN_UI_RSSI_H        30

#define CN_UI_OPRT_X_S      CN_UI_RSSI_X_S
#define CN_UI_OPRT_Y_S      (CN_UI_RSSI_Y_S + CN_UI_RSSI_H)
#define CN_UI_OPRT_W        240
#define CN_UI_OPRT_H        30


#define CN_UI_SIM_T_X_S    0
#define CN_UI_SIM_T_Y_S    (CN_UI_OPRT_Y_S + CN_UI_OPRT_H)
#define CN_UI_SIM_T_W      400
#define CN_UI_SIM_T_H      50

#define CN_UI_SIGNAL_FRESH   (20*1000)   ///< this fresh could be more frequency,we fresh it until it changed



///< csq:0-31
static int ui_signal_entry(void *arg)
{
    int csq,rssi;
    int csq_bak = 0;
    int rssi_bak = 0;
    char rssi_str[32] = "RSSI:" ;

    char oprt_str[32] = "OPRT:";
    char oprt_str_bak[32] = {0};

    Paint_DrawImage(g_img_csq_0 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
    Paint_DrawString_EN(CN_UI_RSSI_X_S, CN_UI_RSSI_Y_S, rssi_str, &Font24, WHITE, BLACK);
    Paint_DrawString_EN(CN_UI_OPRT_X_S, CN_UI_OPRT_Y_S, oprt_str, &Font24, WHITE, BLACK);

    while(1)
    {
        ///< csq fresh
        if(0 == ec2x_get_csq(&csq))
        {
            csq = (csq+6)/7;
            if(csq != csq_bak)
            {
                switch(csq)
                {
                    case 0:
                        Paint_DrawImage(g_img_csq_0 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                    case 1:
                        Paint_DrawImage(g_img_csq_1 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                    case 2:
                        Paint_DrawImage(g_img_csq_2 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                    case 3:
                        Paint_DrawImage(g_img_csq_3 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                    case 4:
                        Paint_DrawImage(g_img_csq_4 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                    case 5:
                        Paint_DrawImage(g_img_csq_5 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                    default:
                        Paint_DrawImage(g_img_csq_0 ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
                        break;
                }
                csq_bak = csq;

            }

            printf("%s:csq_fresh OK\n\r",__FUNCTION__);
        }
        else
        {
            printf("%s:csq_fresh ERR\n\r",__FUNCTION__);

        }

        ///< rssi fresh
        if(0 == ec2x_get_rssi(&rssi))
        {
            if(rssi != rssi_bak)  ///<fresh the different data
            {
                snprintf(rssi_str,32,"RSSI:%ddbm",rssi);
                ui_clean_window(CN_UI_RSSI_X_S,CN_UI_RSSI_Y_S,CN_UI_RSSI_W,CN_UI_RSSI_H);
                Paint_DrawString_EN(CN_UI_RSSI_X_S, CN_UI_RSSI_Y_S, rssi_str, &Font24, WHITE, BLACK);
                rssi_bak = rssi;
            }
            printf("%s:rssi_fresh OK\n\r",__FUNCTION__);
        }
        else
        {
            printf("%s:rssi_fresh ERR\n\r",__FUNCTION__);
        }


        ///< rssi fresh
        if(0 == ec2x_get_operator(oprt_str,26))
        {
            if(0 != strcmp(oprt_str,oprt_str_bak))  ///<fresh the different data
            {
                ui_clean_window(CN_UI_OPRT_X_S,CN_UI_OPRT_Y_S,CN_UI_OPRT_W,CN_UI_OPRT_H);
                Paint_DrawString_EN(CN_UI_OPRT_X_S, CN_UI_OPRT_Y_S, oprt_str, &Font24, WHITE, BLACK);
                strcpy(oprt_str_bak,oprt_str);

            }
            printf("%s:oprt_fresh OK\n\r",__FUNCTION__);
        }
        else
        {
            printf("%s:oprt_fresh ERR\n\r",__FUNCTION__);
        }

        osal_task_sleep(CN_UI_SIGNAL_FRESH);
    }

    return 0;
}

///< here we need the touch support

typedef struct
{
    int         iccid_get:1;
    int         eid_get:1;
    int         iccid_change:1;
    int         iccid_select:3;
    eid_t       eid;
    iccid_tab_t tab;
}ui_iccid_cb_t;
static ui_iccid_cb_t  s_ui_iccid_cb;

///< VIEW FOR THE ICCID UI
/*****************************************
 -----------------------------------------  LINE S_1
 EID:XXXX                                   LINE EID
 -----------------------------------------  LINE S_2
 T:Type S:Seed B:Bussiness  I:inuse         LINE DESCRIPTION
 -----------------------------------------  LINE S_3
 No   ICCID         T       I               LINE TABLE
 -----------------------------------------  LINE S_4
 0    xxxxxx        S       FULLFIL         LINE I_0
 1    xxxxxx        B       EMPTY           LINE_I_1
 2    xxxxxx        B       EMPTY           LINE_I_2
 3
 -----------------------------------------  LINE S_5
******************************************/

#define CN_ICCID_ITEM_MAX  4
#define CN_ICCID_STR_MAXEN 64

#define CN_ICCID_LINE_H   10
#define CN_ICCID_FONT_H   40

#define CN_ICCID_BASE_Y           280
#define CN_ICCID_LINE_S_1_Y       (CN_ICCID_BASE_Y)
#define CN_ICCID_LINE_EID_Y       (CN_ICCID_LINE_S_1_Y + CN_ICCID_LINE_H)
#define CN_ICCID_LINE_S_2_Y       (CN_ICCID_LINE_EID_Y + CN_ICCID_FONT_H + 10)
#define CN_ICCID_LINE_D_Y         (CN_ICCID_LINE_S_2_Y + CN_ICCID_LINE_H)
#define CN_ICCID_LINE_S_3_Y       (CN_ICCID_LINE_D_Y + CN_ICCID_FONT_H)
#define CN_ICCID_LINE_T_Y         (CN_ICCID_LINE_S_3_Y + CN_ICCID_LINE_H)
#define CN_ICCID_LINE_S_4_Y       (CN_ICCID_LINE_T_Y + CN_ICCID_FONT_H)
#define CN_ICCID_lINE_I_0_Y       (CN_ICCID_LINE_S_4_Y + CN_ICCID_LINE_H)
#define CN_ICCID_lINE_I_1_Y       (CN_ICCID_lINE_I_0_Y + CN_ICCID_FONT_H)
#define CN_ICCID_lINE_I_2_Y       (CN_ICCID_lINE_I_1_Y + CN_ICCID_FONT_H)
#define CN_ICCID_lINE_I_3_Y       (CN_ICCID_lINE_I_2_Y + CN_ICCID_FONT_H)
#define CN_ICCID_lINE_S_5_Y       (CN_ICCID_lINE_I_3_Y + CN_ICCID_FONT_H)
#define CN_ICCID_END_Y            (CN_ICCID_lINE_S_5_Y)

#define CN_ICCID_BASE_X           (10)
#define CN_ICCID_END_X            (470)
#define CN_ICCID_STATUS_X         (450)
#define CN_UI_ICCID_FRESH         (1*1000)

///< status:0 empty 1 fill blue 2 fill gray
typedef enum
{
    en_draw_status_empty = 0,
    en_draw_status_fillblue,
    en_draw_status_gray,
}en_draw_status_t;

static int draw_status(int x,int y,int status)
{
    switch(status)
    {
        case en_draw_status_empty:
            Paint_DrawCircle(x,y,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            Paint_DrawCircle(x,y,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            break;
        case en_draw_status_fillblue:
            Paint_DrawCircle(x,y,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            break;
        case en_draw_status_gray:
            Paint_DrawCircle(x,y,15,GRAY,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            break;
        default:
            break;
    }

    return 0;
}
static int iccid_disablestatus()
{
    int i  = 0;
    for(i = 0;i<s_ui_iccid_cb.tab.num;i++)
    {
        draw_status(CN_ICCID_STATUS_X,CN_ICCID_lINE_I_0_Y+CN_ICCID_FONT_H*i +8,en_draw_status_gray);
    }

    return 0;
}


static  int iccid_fresh()
{
    int ret = 0;
    int i   = 0;
    static int times = 0;
    char str[CN_ICCID_STR_MAXEN];

    times ++;

    if((0 == s_ui_iccid_cb.iccid_get) ||(times > 5))
    {
        ret = ec2x_geticcidtab(&s_ui_iccid_cb.tab);
        if(ret == 0)
        {
            s_ui_iccid_cb.iccid_get = 1;
            times = 0;
        }
    }


    for(i = 0;i<s_ui_iccid_cb.tab.num;i++)
    {
        if(s_ui_iccid_cb.tab.iccid[i].status)
        {
            draw_status(CN_ICCID_STATUS_X,CN_ICCID_lINE_I_0_Y +CN_ICCID_FONT_H*i +8 ,en_draw_status_fillblue);
        }
        else
        {
            draw_status(CN_ICCID_STATUS_X,CN_ICCID_lINE_I_0_Y+CN_ICCID_FONT_H*i +8,en_draw_status_empty);
        }

        sprintf(str,"%d %-20s %c",i,s_ui_iccid_cb.tab.iccid[i].id,\
                (s_ui_iccid_cb.tab.iccid[i].type==1)?'S':'B');
        Paint_DrawString_EN(CN_ICCID_BASE_X, CN_ICCID_lINE_I_0_Y + CN_ICCID_FONT_H*i ,\
                str, &Font24, WHITE, BLACK);
    }

    if(i < s_ui_iccid_cb.tab.num)
    {
        ui_clean_window(CN_ICCID_BASE_X,CN_ICCID_lINE_I_0_Y+ i*CN_ICCID_FONT_H,\
                        CN_ICCID_END_X-CN_ICCID_BASE_X,CN_ICCID_END_Y-CN_ICCID_lINE_I_0_Y -i*CN_ICCID_FONT_H);
    }



    if(0 == s_ui_iccid_cb.eid_get)
    {
        ret = ec2x_geteid(&s_ui_iccid_cb.eid);
        if(0 == ret)
        {
            snprintf(str,CN_ICCID_STR_MAXEN,"EID:%s",s_ui_iccid_cb.eid.id);
            Paint_DrawString_EN(CN_ICCID_BASE_X, CN_ICCID_LINE_EID_Y, str, &Font24, WHITE, BLACK);
            s_ui_iccid_cb.eid_get =1;
        }
    }



    return 0;
}

///< THE TOUCH X Y NOT THE SAME AS THE PAIN:ROTATE 270
static int ui_iccid_touch(int x,int y)
{
    int card = 0;

    card = (y - CN_ICCID_lINE_I_0_Y)/CN_ICCID_FONT_H;


    if(card < CN_ICCID_MAX_NUM)
    {
        printf("CARD SELECT:%d \n\r",card);
        s_ui_iccid_cb.iccid_change =1;
        s_ui_iccid_cb.iccid_select = card;
    }

    return 0;
}


static int ui_iccid_entry(void *arg)
{

    ui_touch_t touch;
    char str[CN_ICCID_STR_MAXEN];
    ui_clean_window(CN_ICCID_BASE_X,CN_ICCID_BASE_Y,\
                    CN_ICCID_END_X-CN_ICCID_BASE_X,CN_ICCID_END_Y-CN_ICCID_BASE_Y);

    Paint_DrawLine(CN_ICCID_BASE_X,CN_ICCID_LINE_S_1_Y,CN_ICCID_END_X,CN_ICCID_LINE_S_1_Y,\
            GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

    Paint_DrawString_EN(CN_ICCID_BASE_X, CN_ICCID_LINE_EID_Y, "EID:", &Font24, WHITE, BLACK);

    Paint_DrawLine(CN_ICCID_BASE_X,CN_ICCID_LINE_S_2_Y,CN_ICCID_END_X,CN_ICCID_LINE_S_2_Y,\
            GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

    Paint_DrawString_EN(CN_ICCID_BASE_X, CN_ICCID_LINE_D_Y,\
                        "T:Type S:Seed B:Bussiness", &Font24, WHITE, BLACK);

    Paint_DrawLine(CN_ICCID_BASE_X,CN_ICCID_LINE_S_3_Y,CN_ICCID_END_X,CN_ICCID_LINE_S_3_Y,\
             GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

    sprintf(str,"%2s %-19s %-2s %-2s","No","ICCID","T","E");
    Paint_DrawString_EN(CN_ICCID_BASE_X, CN_ICCID_LINE_T_Y, str, &Font24, WHITE, BLACK);

    Paint_DrawLine(CN_ICCID_BASE_X,CN_ICCID_LINE_S_4_Y,CN_ICCID_END_X,CN_ICCID_LINE_S_4_Y,\
              GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

    Paint_DrawLine(CN_ICCID_BASE_X,CN_ICCID_lINE_S_5_Y,CN_ICCID_END_X,CN_ICCID_lINE_S_5_Y,\
              GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

    touch.x = CN_ICCID_STATUS_X-10;
    touch.w = CN_PAIN_W - CN_ICCID_STATUS_X +10;
    touch.y = CN_ICCID_lINE_I_0_Y;
    touch.h = CN_ICCID_lINE_S_5_Y - CN_ICCID_lINE_I_0_Y;
    touch.hook = ui_iccid_touch;
    ui_touch_register(&touch);

    while(1)
    {
        osal_task_sleep(CN_UI_ICCID_FRESH);
        if(s_ui_iccid_cb.iccid_change)
        {
            if((s_ui_iccid_cb.iccid_select <s_ui_iccid_cb.tab.num) &&\
               ((s_ui_iccid_cb.tab.iccid[s_ui_iccid_cb.iccid_select].status == 0)||\
                (s_ui_iccid_cb.tab.iccid[s_ui_iccid_cb.iccid_select].type == 1)))
            {
                extern int iot_reconnect();
                iot_reconnect();

                iccid_disablestatus();
                ec2x_eniccid(&s_ui_iccid_cb.tab.iccid[s_ui_iccid_cb.iccid_select]);
            }
            s_ui_iccid_cb.iccid_change = 0;
            s_ui_iccid_cb.iccid_get = 0;
        }

        iccid_fresh();
    }
    return 0;
}



#define CN_UI_IOT_FRESH  (1*1000)
extern int iot_status();

static int ui_iot_entry(void *arg)
{
    int cur_status = 0;
    Paint_DrawImage(g_img_disconnected ,290, 130, 140, 140);

    while(1)
    {
        osal_task_sleep(CN_UI_IOT_FRESH);
        cur_status = iot_status();

        if(cur_status == 1)
        {
            Paint_DrawImage(g_img_connected ,290, 130, 140, 140);
        }
        else
        {
            Paint_DrawImage(g_img_disconnected ,290, 130, 140, 140);
        }

    }
    return 0;
}

typedef enum
{
    en_app_main_status_initialize = 0,  ///< waiting for the module
    en_app_main_status_seed,            ///< this is the seed card
    en_app_main_status_business,        ///< this is the business card
    en_app_main_status_changing,        ///< this is the changge status
}en_app_main_status_t;

int standard_app_demo_main()
{
    printf("HC demo main\r\n");
    ec2x_ver_t ver;

    BSP_LCD_Init();
    void GT911_Init(void);
    GT911_Init();

    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_270, WHITE);
    Paint_Clear(WHITE);
    Paint_SetRotate(ROTATE_270);
    //Paint_DrawString_EN(50, 20, "HUAWEI OceanLink Demo", &Font24, LIGHTBLUE, BLACK);
    Paint_DrawImage(g_img_oc, 0, 0, 480, 114);
    Paint_DrawLine(10,120,470,120,GRAY,LINE_STYLE_SOLID,DOT_PIXEL_3X3);

    Paint_DrawImage(g_img_huawei_logo, 250, 700, 100, 100);
    Paint_DrawImage(g_img_hold_logo, 370, 700, 100, 100);

    ///< wait until the ec20 done
    Paint_DrawString_EN(20, 450 , "WAIT MODULE READY...", &Font24, WHITE, BLACK);
    while(0 != ec2x_getmqttversion(&ver))
    {
        osal_task_sleep(2 *1000);
    }
    Paint_DrawString_EN(20, 490 , "MODULE READY NOW!", &Font24, WHITE, BLACK);
    ec2x_echoset(0);

    osal_task_create("ui_time",ui_time_entry,NULL,0x600,NULL,10);
    osal_task_create("ui_signal",ui_signal_entry,NULL,0x600,NULL,10);
    osal_task_create("ui_iccid",ui_iccid_entry,NULL,0x600,NULL,10);
    osal_task_create("ui_iot",ui_iot_entry,NULL,0x600,NULL,10);

    extern int iot_dmeo_main();
    iot_dmeo_main();

    GT911_InstallHook(ui_touch);



    return 0;
}


