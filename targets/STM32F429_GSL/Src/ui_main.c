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
#define CN_UI_TIME_FRESH (1000*10)


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

#define CN_UI_SIGNAL_FRESH   (5*1000)   ///< this fresh could be more frequency,we fresh it until it changed



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
                snprintf(rssi_str,32,"RSSI:%d dbm",rssi);
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
        if(0 == ec2x_get_operator(oprt_str+5,26))
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
#define CN_UI_ICCID_FRESH   (10*1000)
#define CN_ICCID_NUM         4
typedef struct
{
    int    type;
    int    inuse;
    char   id[22];
}iccid_card_t;


typedef struct
{
    int           card_num;
    iccid_card_t  iccid[CN_ICCID_NUM];
}iccid_cb_t;


static iccid_cb_t  s_iccid_cb =
{
    .card_num =4,
    .iccid[0] = {
      .type = 1,
      .inuse =0,
      .id = "89860317422046189700",
    },
    .iccid[1] = {
      .type = 2,
      .inuse =0,
      .id = "89860317422046189701",
    },
    .iccid[2] = {
      .type = 2,
      .inuse =0,
      .id = "89860317422046189702",
    },
    .iccid[3] = {
      .type = 2,
      .inuse =1,
      .id = "89860317422046189703",
    },
};


#define CN_UI_ICCID_X_S    10
#define CN_UI_ICCID_Y_S    350
#define CN_UI_ICCID_W      470
#define CN_UI_ICCID_H      300
#define CN_UI_ICCID_HLINE  50


static  int iccid_get()
{

    return 0;
}

///< simualtor it
static  int iccid_enable(int card)
{
    int i ;
    printf("%s:enable the %d card\n\r",__FUNCTION__,card);

    for(i= 0;i< s_iccid_cb.card_num;i++)
    {
        if(i == card)
        {
            s_iccid_cb.iccid[i].inuse = 1;
        }
        else
        {
            s_iccid_cb.iccid[i].inuse = 0;

        }
    }
    return 0;
}
static  int iccid_draw()
{
    int i = 0;
    char str[64];

    ui_clean_window(CN_UI_ICCID_X_S,CN_UI_ICCID_Y_S,CN_UI_ICCID_W,CN_UI_ICCID_H);

    sprintf(str,"%2s %-19s %-2s %-2s","No","ICCID","T","E");
    Paint_DrawString_EN(CN_UI_ICCID_X_S, CN_UI_ICCID_Y_S, str, &Font24, WHITE, BLACK);

    for(i = 0;i<s_iccid_cb.card_num;i++)
    {
        sprintf(str,"%d %-20s %d",i,s_iccid_cb.iccid[i].id,\
                 s_iccid_cb.iccid[i].type);

        if(s_iccid_cb.iccid[i].inuse)
        {
            Paint_DrawCircle(CN_UI_ICCID_X_S+CN_UI_ICCID_W -30,CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1)+8,\
                    15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
        }
        else
        {
            Paint_DrawCircle(CN_UI_ICCID_X_S+CN_UI_ICCID_W -30,CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1)+8,\
                    15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            Paint_DrawCircle(CN_UI_ICCID_X_S+CN_UI_ICCID_W -30,CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1)+8,\
                    13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
        }
        Paint_DrawString_EN(CN_UI_ICCID_X_S, CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1) ,\
                str, &Font24, WHITE, BLACK);
    }

    return 0;
}

static  int iccid_fresh()
{
    iccid_cb_t iccid_bak;
    int i = 0;
    char str[64];

    for(i = 0;i<s_iccid_cb.card_num;i++)
    {


        if(s_iccid_cb.iccid[i].inuse)
        {
            Paint_DrawCircle(CN_UI_ICCID_X_S+CN_UI_ICCID_W -30,CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1)+8,\
                    15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
        }
        else
        {
            Paint_DrawCircle(CN_UI_ICCID_X_S+CN_UI_ICCID_W -30,CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1)+8,\
                    15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            Paint_DrawCircle(CN_UI_ICCID_X_S+CN_UI_ICCID_W -30,CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1)+8,\
                    13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
        }

        if(0 != strncmp(s_iccid_cb.iccid[i].id,iccid_bak.iccid[i].id,22))
        {
            strcpy(iccid_bak.iccid[i].id,s_iccid_cb.iccid[i].id);
            sprintf(str,"%d %-20s %d",i,s_iccid_cb.iccid[i].id,\
                     s_iccid_cb.iccid[i].type);
            Paint_DrawString_EN(CN_UI_ICCID_X_S, CN_UI_ICCID_Y_S + CN_UI_ICCID_HLINE*(i+1) ,\
                    str, &Font24, WHITE, BLACK);
        }

    }

    return 0;
}



///< THE TOUCH X Y NOT THE SAME AS THE PAIN:ROTATE 270
static int ui_iccid_touch(int x_t,int y_t)
{
    int card = 0;
    int y;

     y= x_t;

    if((y >= (CN_UI_ICCID_Y_S + 24)) && (y <= (CN_UI_ICCID_Y_S + 24 + CN_UI_ICCID_HLINE*CN_ICCID_NUM)))
    {
        card = (y - CN_UI_ICCID_Y_S -24 )/CN_UI_ICCID_HLINE;

        printf("touch line:%d \n\r",card);


        if(card < CN_ICCID_NUM)
        {
            iccid_enable(card);
            iccid_get();
            iccid_fresh();
        }
    }
    else
    {

    }

    return 0;
}



static int ui_iccid_entry(void *arg)
{

    Paint_DrawLine(10,300,470,300,GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);
    Paint_DrawString_EN(10, 310, "ICCIDLIST", &Font24, WHITE, BLACK);
    Paint_DrawLine(10,340,470,340,GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

    iccid_draw();

    GT911_InstallHook(ui_iccid_touch);
    while(1)
    {
        osal_task_sleep(CN_UI_ICCID_FRESH);
    }
    return 0;
}

#define CN_UI_IOT_FRESH  (5*1000)
static int ui_iot_entry(void *arg)
{

    Paint_DrawImage(g_img_disconnected ,290, 130, 140, 140);

    while(1)
    {
        osal_task_sleep(CN_UI_IOT_FRESH);
    }
    return 0;


}



int standard_app_demo_main()
{
    printf("HC demo main\r\n");

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


    osal_task_create("ui_time",ui_time_entry,NULL,0x600,NULL,10);
    osal_task_create("ui_signal",ui_signal_entry,NULL,0x600,NULL,10);
    osal_task_create("ui_iccid",ui_iccid_entry,NULL,0x600,NULL,10);
    osal_task_create("ui_iot",ui_iot_entry,NULL,0x600,NULL,10);



    return 0;
}


