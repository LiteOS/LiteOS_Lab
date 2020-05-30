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
 *  2020-05-30 14:19  zhangqianfu  The first version
 *
 */

#include <stdlib.h>
#include <string.h>
#include <link_log.h>
#include <ota_img.h>
#include <ota_flag.h>


/**
 *
 * HERE WE MAKE THE FUNCTION TO DO THE IMG TEST
 *
 * AND THIS FUNCTION COULD ONLY RUN IN LOADER, FOR WE WILL ERASE ALL THE IMG
 *
 * TEST FOLLOWING:
 *
 * 1, WE ERASE THE SPECIFYED IMG
 * 2, WE GENERATE RANDOM DATA IN g_otaimg_rodata
 * 3, WE WRITE DATA FROM g_otaimg_rodata TO THE SPECIFIED IMG BY RAMDOM LENGTH
 *    UNTIL THE IMG IS FULL
 * 4, WE FLUSH THE IMG
 * 5, WE READ THE DATA FROM THE IMG AND COMPARE IT WITH THE READONLY DATA UNTIL
 *    WE READ ALL THE DATA
 * 6, IF NO ERROR HAPPENED, THEN WE THINK THE IMG IS OK
 *
 * */

#include <stdlib.h>
#include <string.h>
#include <link_log.h>
#include <ota_img.h>
#include <ota_flag.h>
#include <osal.h>

#ifndef CONFIG_OTAIMG_TESTBLOCKSIZE
#define CONFIG_OTAIMG_TESTBLOCKSIZE  1024
#endif

#ifndef CONFIG_OTA_IMGCHECKSOTA
#define CN_OTAIMG_TESTMODE  EN_OTA_TYPE_FOTA
#else
#define CN_OTAIMG_TESTMODE  EN_OTA_TYPE_SOTA
#endif


static uint8_t  g_otaimg_rodata[CONFIG_OTAIMG_TESTBLOCKSIZE];
static uint8_t  g_otaimg_rwdata[CONFIG_OTAIMG_TESTBLOCKSIZE];
static uint8_t  g_otaimg_cachedata[CONFIG_OTAIMG_TESTBLOCKSIZE];


#define minione_intwo(a,b)    ((a) >(b) ?(b):(a))
#define minione_inthree(a, b, c)   minione_intwo(minione_intwo(a,b),c)

static int img_rand(void)
{
    int ret = rand()%CONFIG_OTAIMG_TESTBLOCKSIZE;
    if(ret == 0)
    {
        ret = 1;
    }
    return ret;
}

///<initialize the raw data to the buffer
static void init_rodata(void)
{
    uint8_t data;
    int i;

    for(i = 0;i< CONFIG_OTAIMG_TESTBLOCKSIZE; i++)
    {
        data = (uint8_t)img_rand();
        g_otaimg_rodata[i] = data;
    }
    return;
}

#define CN_PRINT_WIDTH  16
static void memcheck_print(const char *index,uint8_t *mem, int len)
{
    int i = 0;
    link_printf("%s:\r\n",index);
    for(i = 0;i< len;i++)
    {
        if(0 == (i%CN_PRINT_WIDTH))
        {
            link_printf("\r\n%08x:",(unsigned int)i);
        }
        printf("%02x ",mem[i]);
    }
    link_printf("\r\n");
}

///<use this function to write the data to  the  img full
static int img_randomrw_test(en_ota_img_type_t img_type)
{
    int img_size;
    int img_left;
    int ret;
    int cur_offset;
    int cur_len;

    ret = ota_img_size(CN_OTAIMG_TESTMODE,img_type,&img_size);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT GET THE IMG:TYPE:%d",img_type);
        return ret;
    }
    LINK_LOG_DEBUG("GET IMG %d SIZE:%d SUCCESS", img_type, img_size);


    ret = ota_img_erase(CN_OTAIMG_TESTMODE, img_type);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT ERASE THE IMG:TYPE:%d",img_type);
        return ret;
    }
    LINK_LOG_ERROR("ERASE THE IMG:TYPE:%d SUCCESS",img_type);

    ///< then we will write the random data to the img with random length
    img_left = img_size;
    cur_offset = 0;
    while(img_left > 0)
    {
        cur_len = img_rand();
        cur_len = minione_inthree(cur_len, img_left,\
                CONFIG_OTAIMG_TESTBLOCKSIZE -cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE);
        ret = ota_img_write(CN_OTAIMG_TESTMODE,img_type,cur_offset,\
                g_otaimg_rodata + cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("WRITE:IMG:%d OFFSET:%d LEN:%d FAILED",img_type, cur_offset, cur_len);
            return ret;
        }

        cur_offset += cur_len;
        img_left -= cur_len;
    }
    LINK_LOG_DEBUG("WRITE IMG %d SIZE:%d SUCCESS", img_type, img_size);

    ret = ota_img_flush(CN_OTAIMG_TESTMODE, img_type);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT FLUSH THE IMG:TYPE:%d",img_type);
        return ret;
    }
    LINK_LOG_ERROR("FLUSH THE IMG:TYPE:%d SUCCESS",img_type);

    ///< now we do the read with the random length, and check if it is right
    img_left = img_size;
    cur_offset = 0;
    while(img_left > 0)
    {
        cur_len = img_rand();
        cur_len = minione_inthree(cur_len, img_left,\
                CONFIG_OTAIMG_TESTBLOCKSIZE -cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE);
        ret = ota_img_read(CN_OTAIMG_TESTMODE,img_type,cur_offset,\
                g_otaimg_rwdata + cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("READ:IMG:%d OFFSET:%d LEN:%d FAILED",img_type, cur_offset, cur_len);
            return ret;
        }

        ///< AND CHECK IT NOW
        ret = memcmp(g_otaimg_rwdata + cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE,\
                g_otaimg_rodata + cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("RWCONTENT ERR:OFFSET:%d LEN:%d",cur_offset, cur_len);
            memcheck_print("RODATA",g_otaimg_rodata + cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE,cur_len);
            memcheck_print("RWDATA",g_otaimg_rwdata + cur_offset%CONFIG_OTAIMG_TESTBLOCKSIZE,cur_len);

            return ret;
        }
        cur_offset += cur_len;
        img_left -= cur_len;
    }
    LINK_LOG_DEBUG("IMG READ %d SIZE:%d SUCCESS", img_type, img_size);
    return ret;
}

static int img_move(en_ota_img_type_t dst_img, en_ota_img_type_t src_img)
{
    int src_img_size;
    int dst_img_size;
    int img_left;
    int ret;
    int cur_offset;
    int cur_len;

    ret = ota_img_size(CN_OTAIMG_TESTMODE,src_img,&src_img_size);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT GET THE IMG:TYPE:%d",src_img);
        return ret;
    }
    LINK_LOG_DEBUG("GET IMG %d SIZE:%d SUCCESS", src_img, src_img_size);

    ret = ota_img_size(CN_OTAIMG_TESTMODE,dst_img,&dst_img_size);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT GET THE IMG:TYPE:%d",dst_img);
        return ret;
    }
    LINK_LOG_DEBUG("GET IMG %d SIZE:%d SUCCESS", dst_img, dst_img_size);

    if(src_img_size > dst_img_size)
    {
        LINK_LOG_ERROR("GET SRCIMG %d SRCIMGSIZE:%d DSTIMG:%d DSTIMGSIZE:%d DOESNOT MATCH", \
                src_img,src_img_size,dst_img, dst_img_size);
        ret = -1;
        return ret;
    }


    ret = ota_img_erase(CN_OTAIMG_TESTMODE, dst_img);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT ERASE THE IMG:TYPE:%d",dst_img);
        return ret;
    }
    LINK_LOG_ERROR("ERASE THE IMG:TYPE:%d SUCCESS",dst_img);

    ///< then we will write the random data to the img with random length
    img_left = src_img_size;
    cur_offset = 0;
    while(img_left > 0)
    {
        cur_len = img_rand();
        cur_len = minione_inthree(cur_len, img_left,CONFIG_OTAIMG_TESTBLOCKSIZE);
        ret = ota_img_read(CN_OTAIMG_TESTMODE,src_img,cur_offset,\
                g_otaimg_rwdata,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("READ:IMG:%d OFFSET:%d LEN:%d FAILED",src_img, cur_offset, cur_len);
            return ret;
        }

        ret = ota_img_write(CN_OTAIMG_TESTMODE,dst_img,cur_offset,\
                g_otaimg_rwdata,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("WRITE:IMG:%d OFFSET:%d LEN:%d FAILED",src_img, cur_offset, cur_len);
            return ret;
        }
        cur_offset += cur_len;
        img_left -= cur_len;
    }
    LINK_LOG_DEBUG("MOVE FROM IMG %d SIZE:%d  TO IMG:%d SUCCESS", src_img, src_img_size,dst_img);
    ret = ota_img_flush(CN_OTAIMG_TESTMODE, dst_img);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT FLUSH THE IMG:TYPE:%d",dst_img);
        return ret;
    }
    LINK_LOG_ERROR("FLUSH THE IMG:TYPE:%d SUCCESS",dst_img);

    return ret;
}

static int img_cmp(en_ota_img_type_t dst_img, en_ota_img_type_t src_img)
{
    int src_img_size;
    int dst_img_size;
    int img_left;
    int ret;
    int cur_offset;
    int cur_len;

    ret = ota_img_size(CN_OTAIMG_TESTMODE,src_img,&src_img_size);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT GET THE IMG:TYPE:%d",src_img);
        return ret;
    }
    LINK_LOG_DEBUG("GET IMG %d SIZE:%d SUCCESS", src_img, src_img_size);

    ret = ota_img_size(CN_OTAIMG_TESTMODE,dst_img,&dst_img_size);
    if(ret != 0)
    {
        LINK_LOG_ERROR("COULDNOT GET THE IMG:TYPE:%d",dst_img);
        return ret;
    }
    LINK_LOG_DEBUG("GET IMG %d SIZE:%d SUCCESS", dst_img, dst_img_size);

    LINK_LOG_ERROR("GET SRCIMG %d SRCIMGSIZE:%d DSTIMG:%d DSTIMGSIZE:%d ", \
            src_img,src_img_size,dst_img, dst_img_size);
    ///< then we will write the random data to the img with random length
    img_left = minione_intwo(src_img_size,dst_img_size);
    cur_offset = 0;
    while(img_left > 0)
    {
        cur_len = img_rand();
        cur_len = minione_inthree(cur_len, img_left,CONFIG_OTAIMG_TESTBLOCKSIZE);
        ret = ota_img_read(CN_OTAIMG_TESTMODE,src_img,cur_offset,\
                g_otaimg_rwdata,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("READ:IMG:%d OFFSET:%d LEN:%d FAILED",src_img, cur_offset, cur_len);
            return ret;
        }

        ret = ota_img_read(CN_OTAIMG_TESTMODE,dst_img,cur_offset,\
                g_otaimg_cachedata,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("READ:IMG:%d OFFSET:%d LEN:%d FAILED",src_img, cur_offset, cur_len);
            return ret;
        }

        ret = memcmp(g_otaimg_rwdata,g_otaimg_cachedata,cur_len);
        if(ret != 0)
        {
            LINK_LOG_ERROR("IMG NOT EQUAL:OFFSET:%d LEN:%d",cur_offset, cur_len);
            memcheck_print("DSTDATA",g_otaimg_rwdata,cur_len);
            memcheck_print("SRCDATA",g_otaimg_cachedata,cur_len);
            return ret;
        }
        cur_offset += cur_len;
        img_left -= cur_len;
    }
    LINK_LOG_DEBUG("IMG %d SIZE:%d IMG:%d EQUAL", src_img, src_img_size,dst_img);
    return ret;
}


static int img_flaggtest(void)
{
    int i;
    int ret;
    ota_flag_t flag_write;
    ota_flag_t flag_read;

    uint8_t *data;
    ////< initialize the flag
    data = (uint8_t *)&flag_write.info;
    for(i = 0;i< sizeof(flag_write.info);i++)
    {
        data[i] = (uint8_t) img_rand();
    }

    ret = ota_flag_save(CN_OTAIMG_TESTMODE,&flag_write);
    if(0 != ret)
    {
        LINK_LOG_ERROR("FLAG SAVE ERROR");
        return ret;
    }

    ret = ota_flag_get(CN_OTAIMG_TESTMODE,&flag_read);
    if(0 != ret)
    {
        LINK_LOG_ERROR("FLAG GET ERROR");
        return ret;
    }

    ret = memcmp(&flag_write,&flag_read,sizeof(ota_flag_t));
    if(ret != 0)
    {
        LINK_LOG_ERROR("FLAG NOT EQUAL:");
        memcheck_print("WRITEFLAG",(uint8_t *)&flag_write,sizeof(ota_flag_t));
        memcheck_print("READFLAG",(uint8_t *)&flag_write,sizeof(ota_flag_t));
        return ret;
    }

    LINK_LOG_DEBUG("FLAG TEST OK");
    return ret;
}

static int img_checkentry(void)
{
    int ret;

    ///< initializze the source data
    init_rodata();
    ///< test the flag img: this img could be independent, and not need the random read or write
    LINK_LOG_DEBUG("IMG FLAG SAVE/GET TEST BEGIN");
    ret = img_flaggtest();
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG FLAG SAVE/GET TEST ERROR");
        return ret;
    }
    LINK_LOG_DEBUG("IMG FLAG SAVE/GET TEST OK");

    ///< now test the running/backup/downloading for the random read or write
    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(RUNNING/DOWNLOAD/BACKUP)");

    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(RUNNING) BEGIN");
    ret = img_randomrw_test(EN_OTA_IMG_RUNNING);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG  CONTINUEWRITE/RANDOMREAD TEST(RUNNING) FAILED");
        return ret;
    }
    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(RUNNING) SUCCESS");

    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(DOWNLOAD) BEGIN");
    ret = img_randomrw_test(EN_OTA_IMG_DOWNLOAD);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG  CONTINUEWRITE/RANDOMREAD TEST(DOWNLOAD) FAILED");
        return ret;
    }
    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(DOWNLOAD) SUCCESS");

    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(BACKUP) BEGIN");
    ret = img_randomrw_test(EN_OTA_IMG_BACKUP);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG  CONTINUEWRITE/RANDOMREAD TEST(BACKUP) FAILED");
        return ret;
    }
    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST(BACKUP) SUCCESS");
    LINK_LOG_DEBUG("IMG  CONTINUEWRITE/RANDOMREAD TEST SUCCESS");

    ///< NOW DO THE BACKUP TEST
    LINK_LOG_DEBUG("IMG BACKUP TEST BEGIN");
    ret = img_move( EN_OTA_IMG_BACKUP,EN_OTA_IMG_RUNNING);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG BACKUP:MOVE RUNNING TO BACKUP FAILED");
        return ret;
    }

    ret = img_cmp(EN_OTA_IMG_BACKUP,EN_OTA_IMG_RUNNING);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG BACKUP:BACKUP AND RUNNING NOT EQUAL");
        return ret;
    }
    LINK_LOG_DEBUG("IMG  BACKUP TEST SUCCESS");

    ///< NOW DO THE BACKUP TEST
    LINK_LOG_DEBUG("IMG RECOVER TEST BEGIN");
    ret = img_move( EN_OTA_IMG_RUNNING,EN_OTA_IMG_BACKUP);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG RECOVER:MOVE BACKUP TO RUNNING FAILED");
        return ret;
    }

    ret = img_cmp(EN_OTA_IMG_BACKUP,EN_OTA_IMG_RUNNING);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG RECOVER:BACKUP AND RUNNING NOT EQUAL");
        return ret;
    }
    LINK_LOG_DEBUG("IMG  RECOVER TEST SUCCESS");

    ///< NOW DO THE PATCH TEST
    LINK_LOG_DEBUG("IMG PATCH TEST BEGIN");
    ret = img_move( EN_OTA_IMG_RUNNING,EN_OTA_IMG_DOWNLOAD);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG PATCH:MOVE DOWNLOAD TO RUNNING FAILED");
        return ret;
    }

    ret = img_cmp(EN_OTA_IMG_DOWNLOAD,EN_OTA_IMG_RUNNING);
    if(ret != 0)
    {
        LINK_LOG_ERROR("IMG PATCH:DOWNLOAD AND RUNNING NOT EQUAL");
        return ret;
    }
    LINK_LOG_DEBUG("IMG  PATCH TEST SUCCESS");

    LINK_LOG_DEBUG("*********IMG TEST ALL PASSED, CONGRATULATIONS************");


    return ret;
}


int img_check(void)
{
    int ret = -1;

    ret = img_checkentry();
    while(1)
    {
        osal_task_sleep(1000);
    }
    return ret;
}


