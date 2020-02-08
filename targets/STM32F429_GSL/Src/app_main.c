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
 *  2020-01-14 21:03  zhangqianfu  The first version
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


#include <queue.h>
#include <ec2x_oc.h>

static char *machine_status(unsigned int status);


///< VIEW FOR THE LOGO UI
#define CN_UI_W                 470
#define CN_UI_H                 800
#define CN_UI_FONT_H            40
#define CN_UI_LINE_H            10
#define CN_UI_STRLINE_LENMAX    64

///< THE UI VIEW FOR THIS DEMO

/**************************************************

----------------------------------------------------
HUA WEI GSL DEMO

---------------------------------------------------

CSQPICTURE                        IOTPICTURE
RSSI:
OPERATORE:

EID:XXXX                                   LINE EID
-----------------------------------------  LINE S_1
T:Type S:Seed B:Bussiness  I:inuse         LINE DESCRIPTION
-----------------------------------------  LINE S_2
No   ICCID         T       I               LINE TABLE
-----------------------------------------  LINE S_3
0    xxxxxx        S       FULLFIL         LINE I_0
1    xxxxxx        B       EMPTY           LINE_I_1
2    xxxxxx        B       EMPTY           LINE_I_2
3

-----------------------------------------  LINE
TIME FRESH

-----------------------------------------
MS   :
CGATT:                HWLOGO   HOLDLOG
CGREG:
******************************************/

#define CN_UI_CSQ_IMG_X_S        10
#define CN_UI_CSQ_IMG_Y_S        160
#define CN_UI_CSQ_IMG_W          80
#define CN_UI_CSQ_IMG_H          60

#define CN_UI_RSSI_X_S           CN_UI_CSQ_IMG_X_S
#define CN_UI_RSSI_Y_S           (CN_UI_CSQ_IMG_Y_S + CN_UI_CSQ_IMG_H)
#define CN_UI_RSSI_W             240
#define CN_UI_RSSI_H             30

#define CN_UI_OPRT_X_S           CN_UI_RSSI_X_S
#define CN_UI_OPRT_Y_S           (CN_UI_RSSI_Y_S + CN_UI_RSSI_H)
#define CN_UI_OPRT_W             240
#define CN_UI_OPRT_H             30



///< DEFINE FOR THE EID VIEW
#define CN_UI_EID_BASE_Y             280
#define CN_UI_EID_TOPLINE_Y          (CN_UI_EID_BASE_Y)
#define CN_UI_EID_TEXT_Y             (CN_UI_EID_TOPLINE_Y + CN_UI_LINE_H)
#define CN_UI_EID_END_Y              (CN_UI_EID_TEXT_Y + CN_UI_FONT_H + 10)
#define CN_UI_EID_BASE_X             10
#define CN_UI_EID_W                  (CN_UI_W - CN_UI_EID_BASE_X)
#define CN_UI_EID_H                  (CN_UI_EID_END_Y - CN_UI_EID_BASE_Y)


///< DEFINE FOR THE ICCID TABLE VIEW
#define CN_UI_ICCID_ITEM_MAX  4
#define CN_UI_ICCID_BASE_Y           (CN_UI_EID_END_Y)
#define CN_UI_ICCID_LINE1_Y          (CN_UI_ICCID_BASE_Y)
#define CN_UI_ICCID_INSTEST_Y        (CN_UI_ICCID_BASE_Y + CN_UI_LINE_H)
#define CN_UI_ICCID_LINE2_Y          (CN_UI_ICCID_INSTEST_Y + CN_UI_FONT_H)
#define CN_UI_ICCID_HEADER_Y         (CN_UI_ICCID_LINE2_Y + CN_UI_LINE_H)
#define CN_UI_ICCID_LINE3_Y          (CN_UI_ICCID_HEADER_Y + CN_UI_FONT_H)
#define CN_UI_ICCID_ITEMBASE_Y       (CN_UI_ICCID_LINE3_Y + CN_UI_LINE_H)
#define CN_UI_ICCID_END_Y            (CN_UI_ICCID_ITEMBASE_Y + CN_UI_ICCID_ITEM_MAX*CN_UI_FONT_H)
#define CN_UI_ICCID_BASE_X           (10)
#define CN_UI_ICCID_STATUS_X         (450)
#define CN_UI_ICCID_W                (CN_UI_W - CN_UI_ICCID_BASE_X)
#define CN_UI_ICCID_H                (CN_UI_ICCID_END_Y - CN_UI_ICCID_BASE_Y)

#define CN_DRAW_CMD_QUEUE_LEN        10
typedef struct
{
    queue_t *draw_cmd_queue;           ///< queue used for the draw task

    ec2x_ver_t  ver;

    char     time_now[CN_UI_STRLINE_LENMAX]; ///< storage the time now

    int      csq_now;                     ///< the csq we get now
    int      csq_bak;                     ///< the csq we drew last time

    int      rssi_now;                    ///< the rssi we get now
    int      rssi_bak;                    ///< the rssi we drew last time

    char     operator_now[CN_UI_STRLINE_LENMAX] ;///< the operator now
    char     operator_bak[CN_UI_STRLINE_LENMAX] ;///< the operator we drew last time

    int      cgatt_value;
    int      cgreg_n;
    int      cgreg_state;

    int      status_changecard:1;
    int      status_bussiness:1;
    int      status_iotconnected:1;
    int      status_geteid:1;                ///< which means we have get the eid
    int      status_freshiccid:1;            ///< we need fresh the iccid
    int      status_moduleredy:1;            ///< we have detect the module
    int      status_gettime:1;               ///< we have get the time
    int      status_simready:1;              ///< we check if the sim is ready or not
    int      status_cgregready:1;            ///< cgatt ready
    int      status_cgattready:1;            ///< cgreg ready

    eid_t       eid;                         ///< the eid we get
    iccid_tab_t iccid_tab;

    int         machine_status;
    int         machine_status_bak;
    int         card_select;
    osal_semp_t card_select_sync;

}app_main_cb_t;
static app_main_cb_t s_app_main_cb;

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
    x= 480 - y_t;
    y= x_t;

    time_now = osal_sys_time();
    if((time_now - time_last) <= 500)
    {
        time_last = time_now;
        return 0;
    }
    time_last = time_now;
    printf("TOUCH:X:%d Y:%d  %d \n\r",x,y,(int)time_now);

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

///< THE TOUCH X Y NOT THE SAME AS THE PAIN:ROTATE 270
static int ui_iccid_touch(int x,int y)
{
    unsigned int card = 0;

    card = (y - CN_UI_ICCID_ITEMBASE_Y)/CN_UI_FONT_H;
    if(card < s_app_main_cb.iccid_tab.num)
    {
        printf("CARD SELECT:%d \n\r",card);
        s_app_main_cb.card_select = card;
        osal_semp_post(s_app_main_cb.card_select_sync);
    }

    return card;
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



static int draw_cmd_push(int cmd)
{
    int ret = 0;

    ret = queue_push( s_app_main_cb.draw_cmd_queue,(void *)cmd,2*1000);

    if(0 != ret)
    {
        printf("PUSH DRAW COMMAND %d failed\n\r",cmd);
    }
    return ret;
}

typedef enum
{
    en_app_cmd_draw_csq = 0x10,
    en_app_cmd_draw_rssi,
    en_app_cmd_draw_operator,
    en_app_cmd_draw_dmp,
    en_app_cmd_draw_eid,
    en_app_cmd_draw_iccid,
    en_app_cmd_draw_time,
    en_app_cmd_draw_machinestatus,
    en_app_cmd_draw_cpin,
    en_app_cmd_draw_network,
    en_app_cmd_draw_last,
}en_app_cmd_t;

#define CN_UI_THEME_BASE_Y  0
#define CN_UI_THEME_BASE_X  0
#define CN_UI_THEME_IMG_Y   (CN_UI_THEME_BASE_Y)
#define CN_UI_THEME_IMG_H   114
#define CN_UI_THEME_LINE_Y  (CN_UI_THEME_IMG_Y + CN_UI_THEME_IMG_H)
#define CN_UI_THEME_END_Y   (CN_UI_THEME_LINE_Y + CN_UI_LINE_H)
#define CN_UI_THEME_W       480
#define CN_UI_THEME_H       (CN_UI_THEME_END_Y - CN_UI_THEME_BASE_Y)

///< draw the log
static int draw_theme(int fresh)
{
    //Paint_DrawString_EN(50, 20, "HUAWEI OceanLink Demo", &Font24, LIGHTBLUE, BLACK);
    Paint_DrawImage(g_img_oc, CN_UI_THEME_BASE_X, CN_UI_THEME_BASE_Y, CN_UI_THEME_W, CN_UI_THEME_IMG_H);
    Paint_DrawLine(CN_UI_THEME_BASE_Y,CN_UI_THEME_LINE_Y,\
                   CN_UI_W,CN_UI_THEME_LINE_Y,GRAY,LINE_STYLE_SOLID,DOT_PIXEL_3X3);

    return 0;
}


#define CN_UI_CSQ_CLASS_MAX    6
static const unsigned char *s_csq_img_array[CN_UI_CSQ_CLASS_MAX] = \
{
    g_img_csq_0,
    g_img_csq_1,
    g_img_csq_2,
    g_img_csq_3,
    g_img_csq_4,
    g_img_csq_5,
};

static int draw_csq(int fresh)
{
    int class_l = 0;
    if(0 == fresh)
    {
        Paint_DrawImage(s_csq_img_array[class_l] ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
    }
    else if(s_app_main_cb.csq_now != s_app_main_cb.csq_bak)
    {
        class_l = (s_app_main_cb.csq_now >31)?0:((s_app_main_cb.csq_now +CN_UI_CSQ_CLASS_MAX)/(CN_UI_CSQ_CLASS_MAX +1));
        Paint_DrawImage(s_csq_img_array[class_l] ,CN_UI_CSQ_IMG_X_S, CN_UI_CSQ_IMG_Y_S, CN_UI_CSQ_IMG_W, CN_UI_CSQ_IMG_H);
        s_app_main_cb.csq_now = s_app_main_cb.csq_bak;
    }

    return 0;
}


static int draw_rssi(int fresh)
{
    char draw_str[CN_UI_STRLINE_LENMAX] = {0};

    if(0 == fresh)
    {
        snprintf(draw_str,CN_UI_STRLINE_LENMAX,"RSSI:%ddbm",0);
        ui_clean_window(CN_UI_RSSI_X_S,CN_UI_RSSI_Y_S,CN_UI_RSSI_W,CN_UI_RSSI_H);
        Paint_DrawString_EN(CN_UI_RSSI_X_S, CN_UI_RSSI_Y_S, draw_str, &Font24, WHITE, BLACK);
    }
    else if(s_app_main_cb.rssi_now != s_app_main_cb.rssi_bak)
    {
        snprintf(draw_str,CN_UI_STRLINE_LENMAX,"RSSI:%ddbm",s_app_main_cb.rssi_now);
        ui_clean_window(CN_UI_RSSI_X_S,CN_UI_RSSI_Y_S,CN_UI_RSSI_W,CN_UI_RSSI_H);
        Paint_DrawString_EN(CN_UI_RSSI_X_S, CN_UI_RSSI_Y_S, draw_str, &Font24, WHITE, BLACK);
        s_app_main_cb.rssi_bak = s_app_main_cb.rssi_now;
    }
    return 0;
}

static int draw_operator(int fresh)
{

    char draw_str[CN_UI_STRLINE_LENMAX] = {0};

    if(0 == fresh)
    {
        snprintf(draw_str,CN_UI_STRLINE_LENMAX,"OPER:%s","UNKNOWN");

        ui_clean_window(CN_UI_OPRT_X_S,CN_UI_OPRT_Y_S,CN_UI_OPRT_W,CN_UI_OPRT_H);
        Paint_DrawString_EN(CN_UI_OPRT_X_S, CN_UI_OPRT_Y_S, draw_str, &Font24, WHITE, BLACK);
    }
    else if(0 != strcmp(s_app_main_cb.operator_now, s_app_main_cb.operator_bak))
    {
        snprintf(draw_str,CN_UI_STRLINE_LENMAX,"%s",(char *)&s_app_main_cb.operator_now[0]);

        ui_clean_window(CN_UI_OPRT_X_S,CN_UI_OPRT_Y_S,CN_UI_OPRT_W,CN_UI_OPRT_H);
        Paint_DrawString_EN(CN_UI_OPRT_X_S, CN_UI_OPRT_Y_S, draw_str, &Font24, WHITE, BLACK);

        memset(s_app_main_cb.operator_bak,0,sizeof(s_app_main_cb.operator_bak));
        strcpy(s_app_main_cb.operator_bak,s_app_main_cb.operator_now);
    }

    return 0;
}

static int draw_dmp(int fresh)
{
    if(s_app_main_cb.status_iotconnected)
    {
        Paint_DrawImage(g_img_connected ,290, 130, 140, 140);
    }
    else
    {
        Paint_DrawImage(g_img_disconnected ,290, 130, 140, 140);
    }

    return 0;
}


static int draw_eid(int fresh)
{
    char draw_str[CN_UI_STRLINE_LENMAX] = {0};

    ui_clean_window(CN_UI_EID_BASE_X,CN_UI_EID_BASE_Y,CN_UI_EID_W,CN_UI_EID_H);

    if(s_app_main_cb.status_geteid)
    {
        snprintf(draw_str,CN_UI_STRLINE_LENMAX,"EID:%s",s_app_main_cb.eid.id);
    }
    else
    {
        snprintf(draw_str,CN_UI_STRLINE_LENMAX,"EID:%s","UNKNOWN");
    }

    Paint_DrawString_EN(CN_UI_ICCID_BASE_X, CN_UI_EID_TEXT_Y, draw_str, &Font24, WHITE, BLACK);

    return 0;
}

///< status:0 empty 1 fill blue 2 fill gray
typedef enum
{
    en_iccid_status_inactive = 0,
    en_iccid_status_active,
    en_iccid_status_changing,
}en_draw_iccid_status_t;

static int draw_iccid_status(int x,int y,int status)
{
    switch(status)
    {
        case en_iccid_status_inactive:
            Paint_DrawCircle(x,y,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            Paint_DrawCircle(x,y,13,WHITE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            break;
        case en_iccid_status_active:
            Paint_DrawCircle(x,y,15,LIGHTBLUE,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            break;
        case en_iccid_status_changing:
            Paint_DrawCircle(x,y,15,GRAY,DRAW_FILL_FULL,DOT_PIXEL_2X2);
            break;
        default:
            break;
    }

    return 0;
}

static int draw_iccid(int fresh)
{
    int i = 0;
    int status;
    char draw_str[CN_UI_STRLINE_LENMAX] = {0};

    if(0 == fresh)
    {
        ui_clean_window(CN_UI_ICCID_BASE_X,CN_UI_ICCID_BASE_Y,CN_UI_ICCID_W,CN_UI_ICCID_H);

        Paint_DrawLine(CN_UI_ICCID_BASE_X,CN_UI_ICCID_LINE1_Y,\
                CN_UI_ICCID_BASE_X + CN_UI_ICCID_W,CN_UI_ICCID_LINE1_Y, GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

        Paint_DrawString_EN(CN_UI_ICCID_BASE_X, CN_UI_ICCID_INSTEST_Y,\
                           "T:Type S:Seed B:Bussiness", &Font24, WHITE, BLACK);

        Paint_DrawLine(CN_UI_ICCID_BASE_X,CN_UI_ICCID_LINE2_Y,\
                CN_UI_ICCID_BASE_X + CN_UI_ICCID_W,CN_UI_ICCID_LINE2_Y, GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);

        sprintf(draw_str,"%2s %-19s %-2s %-2s","No","ICCID","T","E");
        Paint_DrawString_EN(CN_UI_ICCID_BASE_X, CN_UI_ICCID_HEADER_Y, draw_str, &Font24, WHITE, BLACK);

        Paint_DrawLine(CN_UI_ICCID_BASE_X,CN_UI_ICCID_LINE3_Y,\
                CN_UI_ICCID_BASE_X + CN_UI_ICCID_W,CN_UI_ICCID_LINE3_Y, GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);
    }
    else
    {
        ui_clean_window(CN_UI_ICCID_BASE_X,CN_UI_ICCID_ITEMBASE_Y,CN_UI_ICCID_W,CN_UI_ICCID_END_Y - CN_UI_ICCID_ITEMBASE_Y);

        for(i = 0;i<s_app_main_cb.iccid_tab.num;i++)
        {
            sprintf(draw_str,"%d %-20s %c",i,s_app_main_cb.iccid_tab.iccid[i].id,\
                    (s_app_main_cb.iccid_tab.iccid[i].type==1)?'S':'B');
            Paint_DrawString_EN(CN_UI_ICCID_BASE_X, CN_UI_ICCID_ITEMBASE_Y + CN_UI_FONT_H*i ,\
                    draw_str, &Font24, WHITE, BLACK);

            if(s_app_main_cb.status_changecard)
            {
                status = en_iccid_status_changing;
            }
            else
            {
                status = s_app_main_cb.iccid_tab.iccid[i].status;
            }
            draw_iccid_status(CN_UI_ICCID_STATUS_X,CN_UI_ICCID_ITEMBASE_Y +CN_UI_FONT_H*i +8 ,status);
        }
    }

    return 0;

}


#define CN_UI_TIME_BASE_X   10
#define CN_UI_TIME_BASE_Y   (CN_UI_ICCID_END_Y)

#define CN_UI_TIME_LINE_Y   (CN_UI_TIME_BASE_Y)
#define CN_UI_TIME_TEXT_Y   (CN_UI_TIME_LINE_Y + CN_UI_LINE_H)
#define CN_UI_TIME_END_Y    (CN_UI_TIME_TEXT_Y + CN_UI_FONT_H)
#define CN_UI_TIME_W        (CN_UI_W - CN_UI_TIME_BASE_X)
#define CN_UI_TIME_H        (CN_UI_TIME_END_Y  - CN_UI_TIME_BASE_Y)

static int draw_time(int fresh)
{
    char draw_str[CN_UI_STRLINE_LENMAX] = {0};

    if(0 == fresh)
    {
        ui_clean_window(CN_UI_TIME_BASE_X,CN_UI_TIME_BASE_Y,CN_UI_TIME_W,CN_UI_TIME_H);

        Paint_DrawLine(CN_UI_TIME_BASE_X,CN_UI_TIME_LINE_Y,\
                CN_UI_TIME_BASE_X + CN_UI_TIME_W,CN_UI_TIME_LINE_Y, GRAY,LINE_STYLE_DOTTED,DOT_PIXEL_3X3);
        strcpy(draw_str,"TIME UNKNOWN");

    }
    else
    {
        ui_clean_window(CN_UI_TIME_BASE_X,CN_UI_TIME_TEXT_Y,CN_UI_TIME_W,CN_UI_TIME_END_Y - CN_UI_TIME_TEXT_Y);
        strcpy(draw_str,s_app_main_cb.time_now);
    }
    Paint_DrawString_EN(CN_UI_TIME_BASE_X, CN_UI_TIME_TEXT_Y, draw_str, &Font24, WHITE, BLACK);
    return 0;
}



#define CN_UI_LOGO_BASE_Y      (CN_UI_TIME_END_Y)
#define CN_UI_LOGO_TOPLINE_Y   (CN_UI_LOGO_BASE_Y)

#define CN_UI_LOGO_IMG1_Y      (CN_UI_LOGO_BASE_Y + CN_UI_LINE_H)
#define CN_UI_LOGO_IMG2_Y      (CN_UI_LOGO_IMG1_Y)
#define CN_UI_LOGO_BASE_X      (250)
#define CN_UI_LOGO_IMG1_X      (CN_UI_LOGO_BASE_X)
#define CN_UI_LOGO_IMG_W       (100)
#define CN_UI_LOGO_IMG_H       (100)
#define CN_UI_LOGO_IMG2_X      (CN_UI_LOGO_IMG1_X + CN_UI_LOGO_IMG_W)
#define CN_UI_LOGO_END_Y       (CN_UI_LOGO_IMG2_Y + CN_UI_LOGO_IMG_H)
#define CN_UI_LOGO_W           (CN_UI_W-CN_UI_LOGO_BASE_X)
#define CN_UI_LOGO_H           (CN_UI_LOGO_END_Y - CN_UI_LOGO_BASE_Y)

static  int draw_logo(int fresh)
{
    ui_clean_window(CN_UI_LOGO_BASE_X,CN_UI_LOGO_BASE_Y,CN_UI_LOGO_W,CN_UI_LOGO_H);
    Paint_DrawLine(0,CN_UI_LOGO_TOPLINE_Y,\
                   CN_UI_W,CN_UI_LOGO_TOPLINE_Y,GRAY,LINE_STYLE_SOLID,DOT_PIXEL_3X3);

    Paint_DrawImage(g_img_huawei_logo, CN_UI_LOGO_IMG1_X, CN_UI_LOGO_IMG1_Y, CN_UI_LOGO_IMG_W, CN_UI_LOGO_IMG_H);
    Paint_DrawImage(g_img_hold_logo, CN_UI_LOGO_IMG2_X, CN_UI_LOGO_IMG1_Y, CN_UI_LOGO_IMG_W, CN_UI_LOGO_IMG_H);

    return 0;
}


#define CN_UI_MACHINESTATUS_X   10
#define CN_UI_MACHINESTATUS_Y   (CN_UI_LOGO_IMG1_Y)
#define CN_UI_MACHINESTATUS_W   240
#define CN_UI_MACHINESTATUS_H   CN_UI_FONT_H
#define CN_UI_MACHINESTATUS_END_Y  (CN_UI_MACHINESTATUS_Y + CN_UI_MACHINESTATUS_H)


static int draw_machinestatus(int fresh)
{
    char draw_str[CN_UI_STRLINE_LENMAX] = {0};
    snprintf(draw_str,CN_UI_STRLINE_LENMAX,"MS:%s",machine_status(s_app_main_cb.machine_status));

    ui_clean_window(CN_UI_MACHINESTATUS_X,CN_UI_MACHINESTATUS_Y,CN_UI_MACHINESTATUS_W,CN_UI_MACHINESTATUS_H);
    Paint_DrawString_EN(CN_UI_MACHINESTATUS_X, CN_UI_MACHINESTATUS_Y,draw_str, &Font24, WHITE, BLACK);

    return 0;
}

#define CN_UI_NETWORKING_BASE_X   10
#define CN_UI_NETWORKING_BASE_Y   CN_UI_MACHINESTATUS_END_Y
#define CN_UI_NETWORKING_CGATT_Y  (CN_UI_MACHINESTATUS_END_Y)
#define CN_UI_NETWORKING_CGREG_Y  (CN_UI_NETWORKING_CGATT_Y + CN_UI_FONT_H)
#define CN_UI_NETWORKING_W        240
#define CN_UI_NETWORKING_H        (CN_UI_FONT_H *2)
#define CN_UI_NETWORKING_END_Y    (CN_UI_NETWORKING_BASE_Y + CN_UI_NETWORKING_H)

static int draw_networking(int fresh)
{
    char draw_str[CN_UI_STRLINE_LENMAX] = {0};
    ui_clean_window(CN_UI_NETWORKING_BASE_X,CN_UI_NETWORKING_BASE_Y,CN_UI_NETWORKING_W,CN_UI_NETWORKING_H);

    snprintf(draw_str,CN_UI_STRLINE_LENMAX,"CGATT:%d",s_app_main_cb.cgatt_value);
    Paint_DrawString_EN(CN_UI_NETWORKING_BASE_X, CN_UI_NETWORKING_CGATT_Y, draw_str, &Font24, WHITE, BLACK);

    snprintf(draw_str,CN_UI_STRLINE_LENMAX,"CGREG:%d,%d",s_app_main_cb.cgreg_n,s_app_main_cb.cgreg_state);
    Paint_DrawString_EN(CN_UI_NETWORKING_BASE_X, CN_UI_NETWORKING_CGREG_Y, draw_str, &Font24, WHITE, BLACK);

    return 0;
}


///< this is the draw task for the ui
static int draw_entry(void *para)
{
    int cmd = 0;
    int ret = -1;
    void *data = NULL;

    ui_touch_t touch;


    BSP_LCD_Init();
    void GT911_Init(void);
    GT911_Init();
    GT911_InstallHook(ui_touch);

    touch.x = CN_UI_ICCID_STATUS_X-10;
    touch.w = CN_UI_W - CN_UI_ICCID_STATUS_X +10;
    touch.y = CN_UI_ICCID_ITEMBASE_Y;
    touch.h = CN_UI_ICCID_END_Y - CN_UI_ICCID_ITEMBASE_Y;
    touch.hook = ui_iccid_touch;
    ui_touch_register(&touch);

    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_270, WHITE);
    Paint_Clear(WHITE);
    Paint_SetRotate(ROTATE_270);

    draw_theme(0);
    draw_csq(0);
    draw_rssi(0);
    draw_operator(0);
    draw_dmp(0);
    draw_eid(0);
    draw_iccid(0);
    draw_time(0);
    draw_logo(0);

    while(1)
    {
        ret = queue_pop(s_app_main_cb.draw_cmd_queue,&data,cn_osal_timeout_forever);
        if(0 == ret)
        {
            cmd = (int )data;
            switch (cmd)
            {
                case en_app_cmd_draw_csq:
                    draw_csq(1);
                    break;
                case en_app_cmd_draw_rssi:
                    draw_rssi(1);
                    break;
                case en_app_cmd_draw_operator:
                    draw_operator(1);
                    break;
                case en_app_cmd_draw_dmp:
                    draw_dmp(1);
                    break;
                case en_app_cmd_draw_eid:
                    draw_eid(1);
                    break;
                case en_app_cmd_draw_iccid:
                    draw_iccid(1);
                    break;
                case en_app_cmd_draw_time:
                    draw_time(1);
                    break;
                case en_app_cmd_draw_machinestatus:
                    draw_machinestatus(1);
                    break;
                case en_app_cmd_draw_network:
                    draw_networking(1);
                    break;

                default:
                    break;
            }
        }

    }

    return 0;

}

/******************************************************************************
 *
 * machine status flow
 *                       -------------------------------------------------
 *                       |                                                |
 *                       |                                                |
 *                       V                                                |
 *moduledetect----->checkcard-------------                                |
 *                  ^     ^              |                                |
 *                  |     |              |                                |
 *                  |  seedonly----------|                                |
 *                  |     ^              |                                |
 *                  |     |              |                                |
 *                  |     |              ------------->netdetect-------->iotmode
 *                  |     |              |                                |
 *                  |     |              |                                |
 *                  |     |              v                                |
 *                  |------——--------- seedmode                           |
 *                                      |                                 |
 *                                      |                                 |
 *                                      |                                 |
 *                                      V                                 |
 *                                    cardchange<---------idle<-----------|
 *
 * **/
/*****************************************************************************
 *
 * status:
 *
 * moduledetect:this is the first status,if we detect the module, then turn to cardcheck
 *
 * cardcheck:if get failed, stay here keeping get;
 *           if only seed, goto seed only
 *           if seedmode, goto seed mode
 *           if netmode, goto netcheck mode
 *
 * seedonly: enable the seed each 30 seconds, and turn to cardcheck
 *
 * seedmode: get the iccid, if changed, goto the correspongding mode;
 *
 * netdetect:check the network,if ok goto the iot, else stay here
 *
 * iotmode:  here we connect to the dmp,if success we goto the idle, else turn back to card check
 *
 * idle   :  fresh the iccid, if changged,goto corresponding status; publish data, if failed, goto iotmode
 *
 *
 * cardchange: here we goto cardcheckmode
 *
 * */


typedef enum
{
    en_app_status_moduledetect = 0,
    en_app_status_carddetect,
    en_app_status_seedonly,
    en_app_status_seedmode,
    en_app_status_netdetect,
    en_app_status_iotconnect,
    en_app_status_idle,
    en_app_status_cardchange,
    en_app_status_last,
}en_app_status_t;

const char *s_machine_status[] =
{
    "MODULEDETECT",
    "CARDDETECT",
    "SEEDONLY",
    "SEEDMODE",
    "NETDETECT",
    "IOTCONNECT",
    "IDLE",
    "CARDCHANGE"
};

static char *machine_status(unsigned int status)
{
    char *ret = "UNKNOWN";
    if(en_app_status_last > status)
    {
        ret = (char *)s_machine_status[status];
    }
    return ret;
}


#define CN_FRESH_MAGIC_CSQ     5
#define CN_FRESH_MAGIC_RSSI    5
#define CN_FRESH_MAGIC_OPER    5
#define CN_FRESH_MAGIC_IOT     10
#define CN_FRESH_MAGIC_ICCID   10
#define CN_FRESH_MAGIC_TIME    5
static int deal_commonfresh(int counter)
{
    int ret = -1;

    if(0 == counter%CN_FRESH_MAGIC_CSQ)
    {
        ret = ec2x_get_csq(&s_app_main_cb.csq_now);
        if(0 == ret)
        {
            draw_cmd_push(en_app_cmd_draw_csq);
        }
    }

    if(0 == counter%CN_FRESH_MAGIC_RSSI)
    {
        ret = ec2x_get_rssi(&s_app_main_cb.rssi_now);
        if(0 == ret)
        {
            draw_cmd_push(en_app_cmd_draw_rssi);
        }
    }
    if(0 == counter%CN_FRESH_MAGIC_OPER)
    {
        ret = ec2x_get_operator((char *)&s_app_main_cb.operator_now);
        if(0 == ret)
        {
            draw_cmd_push(en_app_cmd_draw_operator);
        }
    }

    if(0 == counter%CN_FRESH_MAGIC_TIME)
    {
        ret = ec2x_get_time((char *)&s_app_main_cb.time_now);
        if(0 == ret)
        {
            draw_cmd_push(en_app_cmd_draw_time);
        }
    }

    if(0 == s_app_main_cb.status_geteid)
    {
        ret = ec2x_geteid(&s_app_main_cb.eid);
        if(ret == 0)
        {
            s_app_main_cb.status_geteid = 1;
            draw_cmd_push(en_app_cmd_draw_eid);
        }
    }

    return ret;
}


///< check if any module inserted
static int deal_moduledetect(int counter)
{
    int ret = -1;


    int status = en_app_status_moduledetect;

    ec2x_echoset(0);

    ec2x_hwsimset(1);

    ret = ec2x_getmqttversion(&s_app_main_cb.ver);
    if(0 == ret)
    {
        status = en_app_status_carddetect;
    }

    return status;
}


static int deal_cardcheck(int counter)
{
    int ret = -1;
    int status = en_app_status_carddetect;

    deal_commonfresh(counter);

    s_app_main_cb.status_simready = 0;
    ret = ec2x_cpin();
    if(0 == ret)
    {
        s_app_main_cb.status_simready = 1;
        draw_cmd_push(en_app_cmd_draw_cpin);
    }
    else
    {
        draw_cmd_push(en_app_cmd_draw_cpin);
        return status;
    }

    ///< get the iccid
    ret = ec2x_geticcidtab(&s_app_main_cb.iccid_tab);
    if((0 == ret) &&(s_app_main_cb.iccid_tab.num > 0))
    {
        draw_cmd_push(en_app_cmd_draw_iccid);
        if(1 == s_app_main_cb.iccid_tab.num )
        {
            status = en_app_status_seedonly;
        }
        else if(1 == s_app_main_cb.iccid_tab.iccid[0].status)
        {
            status = en_app_status_seedmode;
        }
        else
        {
            status = en_app_status_netdetect;
        }
    }
    return status;
}

static int deal_seedonly(int counter)
{
    int ret = -1;
    int status = en_app_status_seedonly;

    deal_commonfresh(counter);

    ret = ec2x_geticcidtab(&s_app_main_cb.iccid_tab);

    if((0 == ret) &&(s_app_main_cb.iccid_tab.num > 0))
    {
        draw_cmd_push(en_app_cmd_draw_iccid);
        if(1 == s_app_main_cb.iccid_tab.num )
        {
            status = en_app_status_seedonly;
        }
        else if(1 == s_app_main_cb.iccid_tab.iccid[0].status)
        {
            status = en_app_status_seedmode;
        }
        else
        {
            status = en_app_status_netdetect;
        }
    }
    else
    {
        status = en_app_status_carddetect;
    }
    if((0 == counter%10) && (1 == s_app_main_cb.iccid_tab.num )&&(1 == s_app_main_cb.iccid_tab.iccid[0].status))
    {
        ec2x_eniccid(&s_app_main_cb.iccid_tab.iccid[0]);
        osal_task_sleep(10*1000);
        status = en_app_status_carddetect;

    }
    return status;
}


static int deal_seedmode(int counter)
{
    int ret = -1;
    int status = en_app_status_seedmode;

    deal_commonfresh(counter);

    ///< get the iccid
    ret = ec2x_geticcidtab(&s_app_main_cb.iccid_tab);
    if((0 == ret) &&(s_app_main_cb.iccid_tab.num > 0))
    {
        draw_cmd_push(en_app_cmd_draw_iccid);
        if(1 == s_app_main_cb.iccid_tab.num )
        {
            status = en_app_status_seedonly;
        }
        else if(1 == s_app_main_cb.iccid_tab.iccid[0].status)
        {
            status = en_app_status_seedmode;
        }
        else
        {
            status = en_app_status_netdetect;
        }
    }
    else
    {
        status = en_app_status_carddetect;
    }
    return status;
}


static int deal_netdetect(int counter)
{
    int ret = -1;
    int status = en_app_status_netdetect;

    deal_commonfresh(counter);

    s_app_main_cb.cgatt_value = 0;
    s_app_main_cb.status_cgattready = 0;

    ret = ec2x_cgatt(&s_app_main_cb.cgatt_value);
    if(0 == ret )
    {
        if( s_app_main_cb.cgatt_value)
        {
            s_app_main_cb.status_cgattready = 1;
        }
    }

    s_app_main_cb.cgreg_n = 0;
    s_app_main_cb.cgreg_state = 0;
    s_app_main_cb.status_cgregready = 0;

    ret = ec2x_cgreg(&s_app_main_cb.cgreg_n,&s_app_main_cb.cgreg_state);
    if(0 == ret )
    {
        if((5 ==  s_app_main_cb.cgreg_state) ||(1 ==  s_app_main_cb.cgreg_state))
        {
            s_app_main_cb.status_cgregready = 1;
        }
    }

    draw_cmd_push(en_app_cmd_draw_network);

    if(s_app_main_cb.status_cgattready && s_app_main_cb.status_cgregready )
    {
        status = en_app_status_iotconnect;
    }


    return status;
}

extern int iot_connect(void);
extern int iot_disconnect(void);
extern int iot_send(void);
static int deal_iotconnect(unsigned int counter)
{
    int status =  en_app_status_iotconnect;
    int ret = -1;

    deal_commonfresh(counter);

    ret = iot_connect();
    if(0 == ret)
    {
        status = en_app_status_idle;
        s_app_main_cb.status_iotconnected = 1;
        draw_cmd_push(en_app_cmd_draw_dmp);
    }
    else
    {
        status = en_app_status_carddetect;
        osal_task_sleep(30*1000);

    }

    return status;
}


static int deal_idle(unsigned int counter)
{
    int ret = -1;
    int status = en_app_status_idle;

    deal_commonfresh(counter);

    if(0 == counter%CN_FRESH_MAGIC_IOT)
    {
        ret = iot_send();
        if(0 != ret)
        {
            iot_disconnect();

            s_app_main_cb.status_iotconnected = 0;
            draw_cmd_push(en_app_cmd_draw_dmp);

            status = en_app_status_carddetect;
        }

    }
    if(0 == counter%CN_FRESH_MAGIC_ICCID)
    {
        ret = ec2x_geticcidtab(&s_app_main_cb.iccid_tab);
        if((0 == ret) && (s_app_main_cb.iccid_tab.num <=1))
        {
            status = en_app_status_carddetect;
        }
        draw_cmd_push(en_app_cmd_draw_iccid);
    }


    return status;
}

int standard_app_demo_main()
{
    int ret = -1;
    int status = en_app_status_moduledetect;
    unsigned int loop_counter = 1;
    printf("HC demo main\r\n");

    osal_semp_create(&s_app_main_cb.card_select_sync,1,0);

    s_app_main_cb.draw_cmd_queue= queue_create("queue_rcvmsg",CN_DRAW_CMD_QUEUE_LEN,1);
    if(NULL == s_app_main_cb.draw_cmd_queue)
    {
        return ret;
    }
    osal_task_create("draw_ui",draw_entry,NULL,0x600,NULL,10);
    osal_task_sleep(100);

    while(1)
    {
        if(osal_semp_pend(s_app_main_cb.card_select_sync,1000))
        {
            if((s_app_main_cb.card_select < s_app_main_cb.iccid_tab.num) && \
               ((s_app_main_cb.machine_status == en_app_status_idle) || \
                (s_app_main_cb.machine_status == en_app_status_seedmode) || \
                (s_app_main_cb.machine_status == en_app_status_netdetect) ))
            {
                s_app_main_cb.status_changecard = 1;
                draw_cmd_push(en_app_cmd_draw_iccid);

                status = en_app_status_cardchange ;
                s_app_main_cb.machine_status = status;
                draw_cmd_push(en_app_cmd_draw_machinestatus);

                if(s_app_main_cb.status_iotconnected)
                {
                    iot_disconnect();
                    s_app_main_cb.status_iotconnected = 0;
                    draw_cmd_push(en_app_cmd_draw_dmp);
                }

                ec2x_eniccid(&s_app_main_cb.iccid_tab.iccid[s_app_main_cb.card_select]);

                osal_task_sleep(10*1000);
                s_app_main_cb.card_select = CN_ICCID_MAX_NUM;
            }
        }

        switch (status)
        {
            case  en_app_status_moduledetect:
                status = deal_moduledetect(loop_counter);
                break;
            case en_app_status_carddetect:
                status = deal_cardcheck(loop_counter);
                break;
            case en_app_status_seedmode:
                status = deal_seedmode(loop_counter);
                break;
            case en_app_status_seedonly:
                status = deal_seedonly(loop_counter);
                break;
            case  en_app_status_netdetect:
                status = deal_netdetect(loop_counter);
                break;
            case  en_app_status_iotconnect:
                status = deal_iotconnect(loop_counter);
                break;
            case  en_app_status_idle:
                status = deal_idle(loop_counter);
                break;

            case en_app_status_cardchange:
                s_app_main_cb.status_changecard = 0;
                status = en_app_status_carddetect;
                break;

            default:
                break;
        }
        osal_task_sleep(1000*2);
        s_app_main_cb.machine_status = status;
        draw_cmd_push(en_app_cmd_draw_machinestatus);

        if(0 == s_app_main_cb.status_geteid)
        {
            ret = ec2x_geteid(&s_app_main_cb.eid);
            if(ret == 0)
            {
                s_app_main_cb.status_geteid = 1;
                draw_cmd_push(en_app_cmd_draw_eid);
            }
        }
        loop_counter++;
    }

    return ret;
}
