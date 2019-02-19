/*----------------------------------------------------------------------------
 * Copyright (c) <2017-2018>, <Huawei Technologies Co., Ltd>
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
 *---------------------------------------------------------------------------*/
 /* common headers */
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "hislip.h"
#include "dev_com_struct.h"
#include "hilink_module_op.h"
#include "hilink.h"
#include "dev_hw.h"

/* add your include files from your sdk if need */

/*
    DEV_PROFILE_SVCS point to "hilink_svc" 
    DEV_PROFILE_KEYS point to "dev_map_table" 
    DEV_PROFILE_VALS point to "hilink_msg_table" 
    DEV_PROFILE_DEVINF point to "dev_info" 
    DEV_PROFILE_BIRSA point to "g_rsa_cipher" 
    DEV_PROFILE_ACVAL point to "g_ac" 
    DEV_PROFILE_ENNAME point to "dev_ennm" 
    if you change var's name(for example hilink_svc ...) , please make sure that here
    the macro you also changed at the same time .
*/
#define DEV_PROFILE_SVCS hilink_svc 
#define DEV_PROFILE_KEYS dev_map_table
#define DEV_PROFILE_VALS hilink_msg_table

#define DEV_PROFILE_DEVINF  dev_info
#define DEV_PROFILE_BIRSA   g_rsa_cipher
#define DEV_PROFILE_ACVAL   g_ac
#define DEV_PROFILE_ENNAME  dev_ennm
#define DEV_PROFILE_VER  g_profile_ver

/* hilink profile version(please change it if your profile changed) */
const char *g_profile_ver = "VC001.01.01";

const svc_info_t hilink_svc[] = 
{
    {LED_SVC_INSTANCE_ID, "binarySwitch", "switch"}
};

const dev_key_map_t dev_map_table[] = 
{
    {LED_SVC_KEY_SWITCH_ID, "binarySwitch", "on", HLK_DATA_TYPE_BOOL},
    {LED_SVC_KEY_SWITCH_ID_1, "binarySwitch", "indicator", HLK_DATA_TYPE_BOOL}
};


hilink_msg_t  hilink_msg_table[] = 
{
	{LED_SVC_INSTANCE_ID, LED_SVC_KEY_SWITCH_ID, 0, light_ctrl_func},
	{LED_SVC_INSTANCE_ID, LED_SVC_KEY_SWITCH_ID_1, 0, light_ctrl_func_1}
};

/* profile config
具体产品查询HiLink智能家居 Profile */

const dev_info_t dev_info = {
    "",                       /*设备SN*/
    "9004",                   /*设备ID*/
    "HBL-4",                 /*设备型号*/
    "005",                    /*设备类型*/
    "001"                     /*设备制造商*/
};



const char* g_rsa_cipher = "A0FA503EBCB122EA7B49CAFC0C9B6452D9BC2F0F3CDE6E1EB539A784B61D7E89"\
        "714F75DF272F1D2F9B4FCB5340718841141D9E8CC35B400ACC71F914E8552EB5"\
        "1D55928BF73373B83124B0989003EDFC8616D71ACD704436F3515BE7C5D5F0D6"\
        "B567AB024CB3237B044B87B077BFE77D94CB5031C2F250A86A436CA9276D0CF1"\
        "F2FAE556E794A531674197A5E455CB2A5192D82A092DF0C5C8997B526C746CD1"\
        "986F06DEE818FF5AC7774A294BE15347F3B8028B1C6664169A84A0FC7753D828"\
        "372750FD328A06AB7AD9AE387B5B94BDF5C79930568FC94794EC4C119F8F6FBF"\
        "BCA57045D50FA2A02F14C0469105A3E900C9D8D48FE2C7F264B639F991A811B8";

const unsigned char g_ac[HLK_AC_NUM_LEN] = {
	0x29, 0x21, 0x2d, 0x20, 0x74, 0x51, 0x34,0x29, 
	0x46, 0x74, 0x49, 0x66, 0x6a, 0x61, 0x4a, 0x66, 
	0xe2, 0x95, 0x3c, 0x6c, 0xcb, 0xdb, 0x19, 0x01, 
	0xfe, 0x8f, 0xa5, 0xfc, 0xe2, 0x2b, 0x1b, 0x61, 
    0x38, 0x92, 0x1c, 0x6b, 0x93, 0x0b, 0x64, 0x1f, 
	0x2b, 0x00, 0x46, 0x16, 0x55, 0x1e, 0x79, 0x1a};

const dev_nmen_t dev_ennm  = {
	"Smart Switch",
	"HUAWEI"
};

