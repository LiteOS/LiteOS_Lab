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

#include "dev_profile.h"
volatile unsigned int g_keyv_chg_mask[DEV_MAX_KEY_COUNT / UNSIGNED_INT_BITS] = {0};
volatile unsigned int g_svc_chg_mask[DEV_MAX_SVC_COUNT / UNSIGNED_INT_BITS] = {0};

static int dev_set_chg_mask(int index, int mskflg)
{
    int mpos = 0;
    int spos = 0;
    unsigned int msk = 0;
    if (index < 0 || index >= DEV_MAX_KEY_COUNT)
    {
        return -1;
    }
    mpos = index / UNSIGNED_INT_BITS;
    spos = index % UNSIGNED_INT_BITS;
    msk = (0x1 << spos);
    if (DEV_KEY_MASK_SET == mskflg)
    {
        g_keyv_chg_mask[mpos] |= msk;
    }
    else
    {
        g_keyv_chg_mask[mpos] &= (~msk);
    }
    return 0;
}

static int dev_get_svc_idx_by_svcid(short svcid)
{
    svc_info_t *svcs = NULL;
    hislip_s32 itemcnt = 0;
    int idx = -1;
    int i = 0;
    
    svcs = ( svc_info_t *)dev_get_profinfo_and_itssize(DEV_PROF_T_SVCS, &itemcnt);
    for (i = 0; i < itemcnt; i++)
    {
        if (svcs[i].svc_inst == (unsigned short)svcid)
        {
            idx = i;
            break;
        }
    }
    return idx;
}
static int dev_set_svcchg_mask(int index, int mskflg)
{
    int mpos = 0;
    int spos = 0;
    unsigned int msk = 0;
    if (index < 0 || index >= DEV_MAX_SVC_COUNT)
    {
        return -1;
    }
    mpos = index / UNSIGNED_INT_BITS;
    spos = index % UNSIGNED_INT_BITS;
    msk = (0x1 << spos);
    if (DEV_SVC_MASK_SET == mskflg)
    {
        g_svc_chg_mask[mpos] |= msk;
    }
    else
    {
        g_svc_chg_mask[mpos] &= (~msk);
    }
    return 0;
}

void dev_set_all_keyv_unmsk(void)
{
    int i = 0;
    for (i = 0; i < DEV_MAX_KEY_COUNT / UNSIGNED_INT_BITS; i++)
    {
        g_keyv_chg_mask[i] = 0x0;
    }
}
void dev_set_all_svc_unmsk(void)
{
    int i = 0;
    for (i = 0; i < DEV_MAX_SVC_COUNT / UNSIGNED_INT_BITS; i++)
    {
        g_svc_chg_mask[i] = 0x0;
    }
}

int dev_chk_keyv_chg_by_index(int idx, int mskflg)
{
    int mpos = 0;
    int spos = 0;
    unsigned int msk = 0;
    
    if (idx < 0 || idx >= DEV_MAX_KEY_COUNT)
    {
        return 0;
    }
    mpos = idx / UNSIGNED_INT_BITS;
    spos = idx % UNSIGNED_INT_BITS;
    msk = g_keyv_chg_mask[mpos];
    msk = msk >> spos;
    if ((msk & 0x1) == 0x1)
    {
        /* the data changed, now if need unmask it we clear the bit */
        if (DEV_KEY_MASK_USET == mskflg)
        {
            msk = (0x1 << spos);
            g_keyv_chg_mask[mpos] &= (~msk);
        }
        return 1;
    }
 
    return 0;
}


int dev_chk_svc_chg_by_index(int idx, int mskflg)
{
    int mpos = 0;
    int spos = 0;
    unsigned int msk = 0;
    
    if (idx < 0 || idx >= DEV_MAX_SVC_COUNT)
    {
        return 0;
    }
    mpos = idx / UNSIGNED_INT_BITS;
    spos = idx % UNSIGNED_INT_BITS;
    msk = g_svc_chg_mask[mpos];
    msk = msk >> spos;
    if ((msk & 0x1) == 0x1)
    {
        /* the data changed, now if need unmask it we clear the bit */
        if (DEV_SVC_MASK_USET == mskflg)
        {
            msk = (0x1 << spos);
            g_svc_chg_mask[mpos] &= (~msk);
        }
        return 1;
    }
 
    return 0;
}


int dev_get_keyv_chg_flag(void)
{
    int ret = 0;
    int i = 0;
    for (i = 0; i < (DEV_MAX_KEY_COUNT / UNSIGNED_INT_BITS); i++)
    {
        if (0 != g_keyv_chg_mask[i])
        {
            ret = 1;
            break;
        }
    }
    return ret;
}
int dev_get_svc_chg_flag(void)
{
    int ret = 0;
    int i = 0;
    for (i = 0; i < (DEV_MAX_SVC_COUNT / UNSIGNED_INT_BITS); i++)
    {
        if (0 != g_svc_chg_mask[i])
        {
            ret = 1;
            break;
        }
    }
    return ret;
}

void* dev_get_profinfo_and_itssize(int type, int *size)
{
    int cnt = 0;
    void *retaddr = NULL; 
    switch(type) 
    {
        case DEV_PROF_T_SVCS:
            cnt = sizeof(DEV_PROFILE_SVCS)/sizeof(DEV_PROFILE_SVCS[0]);
            retaddr = (void *)DEV_PROFILE_SVCS;
            break;
        case DEV_PROF_T_KEYS:
            cnt = sizeof(DEV_PROFILE_KEYS)/sizeof(DEV_PROFILE_KEYS[0]);
            retaddr = (void *)DEV_PROFILE_KEYS;
            break;
        case DEV_PROF_T_VALS:
            cnt = sizeof(DEV_PROFILE_VALS)/sizeof(DEV_PROFILE_VALS[0]);
            retaddr = (void *)DEV_PROFILE_VALS;
            break;
        default:
            return NULL;
    }
    if (size)
    {
        *size = cnt;
    }
    return retaddr;
}

void *dev_get_dev_info(void)
{
    return (void *)&DEV_PROFILE_DEVINF;
}

void *dev_get_dev_birsa(void)
{
    return (void *)DEV_PROFILE_BIRSA;
}

void *dev_get_dev_ac(void)
{
    return (void *)DEV_PROFILE_ACVAL;
}

void *dev_get_dev_enname(void)
{
    return (void *)&DEV_PROFILE_ENNAME;
}

void *dev_get_dev_profilever(void)
{
    return (void *)DEV_PROFILE_VER;
}

int hlk_update_local_val(short svcid, short keyid, 
                                int val,char *str)
{
    int keyidx = -1;
    
    keyidx = hlk_update_one_key_val(svcid, keyid, val, str);
    if (keyidx >= 0)
    {
        (void)dev_set_chg_mask(keyidx, DEV_KEY_MASK_SET);
    }
    return 0;
}

int hlk_update_local_svcvals(short svcid, short keyid, 
                                int val,char *str, char reportnow)
{
    int svcidx = -1;
    
    (void)hlk_update_one_key_val(svcid, keyid, val, str);
    if (DEV_REPORT_NOW == reportnow)
    {
        svcidx = dev_get_svc_idx_by_svcid(svcid);
        (void)dev_set_svcchg_mask(svcidx, DEV_SVC_MASK_SET);
    }

    return 0;
}

