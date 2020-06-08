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
 *  2020-05-07 09:21  zhangqianfu  The first version
 *
 */

#include <stdlib.h>
#include <string.h>
#include <link_log.h>
#include <osal.h>
#include <dtls_al.h>
#include <ota_img.h>
#include <ota_flag.h>
#include <ota_https.h>

#include "sha256_check.h"


///< server key
static const char g_https_serverca[] =
" -----BEGIN CERTIFICATE-----\r\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\r\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\r\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\r\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\r\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\r\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\r\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\r\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\r\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\r\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\r\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\r\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\r\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\r\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\r\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\r\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\r\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\r\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\r\n"
"-----END CERTIFICATE-----\r\n";


///< build the http request
#define CN_OTA_HTTPS_REQ_FMT  "GET %s HTTP/1.1\r\nAuthorization: Bearer %s\r\nHost: %s:%s\r\nConnection: keep-alive\r\nRange: bytes=%d-%d\r\n\r\n"

#ifndef CONFIG_HTTPOTA_BUFLEN
#define CONFIG_HTTPOTA_BUFLEN  1024
#endif

#ifndef CONFIG_OCMQTT_HTTPS_TIMEOUT
#define CONFIG_OCMQTT_HTTPS_TIMEOUT   (10*1000)
#endif

typedef struct
{
    const char *host;
    const char *port;
    const char *url_re;
    const char *bearer;
    int         offset;
    int         len;
    uint8_t     cache[CONFIG_HTTPOTA_BUFLEN];
}http_section_t;

///< build the request header
static int http_makerequest(http_section_t *section)
{
    int ret;
    ret = snprintf((char *)section->cache,CONFIG_HTTPOTA_BUFLEN,CN_OTA_HTTPS_REQ_FMT,section->url_re,\
            section->bearer,section->host,section->port,section->offset + 1, section->offset + section->len); ///< start from 1
    return ret;
}

///< this is the url test format
///< https://121.36.42.100:8943/iodm/dev/v2.0/upgradefile/applications/qOrU58z7TMg2fao5xceWanjjQSAa/devices/5e12ea0a334dd4f337902dc3_iotlink005/packages/b2731c6b96178488a8862ace
#define CN_URL_HOST_INDEX "https://"
#define CN_URL_PORT_INDEX ":"
#define CN_URL_PORT_DEFAULT "443"
static http_section_t *http_decodeurl(ota_https_para_t *cb)
{
    http_section_t *ret = NULL;
    const char *host_start;
    const char *host_end;
    const char *port_start;
    const char *port_end;
    const char *urlre_start;
    const char *urlre_end;
    char *tmp,*tmp_ret;
    int  len;

    tmp = (char *)cb->url;
    tmp_ret = strstr(tmp,CN_URL_HOST_INDEX);
    if(NULL == tmp_ret)
    {
        goto EXIT_HOST;
    }
    host_start = tmp_ret +strlen(CN_URL_HOST_INDEX);

    tmp = (char *)host_start;
    tmp_ret = strstr(tmp,CN_URL_PORT_INDEX);
    if(NULL == tmp_ret)
    {
        port_start = CN_URL_PORT_DEFAULT;
        port_end = port_start + strlen(CN_URL_PORT_DEFAULT) -1;

        tmp_ret = strstr(tmp,"/");
        if(NULL == tmp_ret)
        {
            goto EXIT_PORT;
        }
        host_end = tmp_ret-1;
        urlre_start = tmp_ret;
        urlre_end = cb->url + strlen(cb->url) -1;
    }
    else
    {
        host_end = tmp_ret-1;
        port_start = tmp_ret + 1;
        tmp = tmp_ret;

        tmp_ret = strstr(tmp,"/");
        if(NULL == tmp_ret)
        {
            goto EXIT_PORT;
        }
        port_end = tmp_ret -1;
        urlre_start = tmp_ret;
        urlre_end = cb->url + strlen(cb->url) -1;
    }
    len = sizeof(http_section_t) + (host_end - host_start + 1 + 1) + (port_end- port_start + 1 + 1) + (urlre_end - urlre_start + 1 + 1);
    ret = (http_section_t *)osal_malloc(len);
    if(NULL == ret)
    {
        goto EXIT_MEM;
    }
    tmp = (char *)ret;
    tmp += sizeof(http_section_t);
    ///< initialize the member in the parameters
    len = host_end - host_start + 1;
    memcpy(tmp, host_start, len);
    tmp[len] = '\0';
    ret->host = tmp;
    tmp += len + 1;

    len = port_end - port_start + 1;
    memcpy(tmp, port_start, len);
    tmp[len] = '\0';
    ret->port = tmp;
    tmp += len + 1;

    len = urlre_end - urlre_start + 1;
    memcpy(tmp, urlre_start, len);
    tmp[len] = '\0';
    ret->url_re = tmp;

    ret->bearer = cb->authorize;
    return ret;

EXIT_MEM:
EXIT_PORT:
EXIT_HOST:
    return ret;
}

///< return the len write, try to send all the data in the timeout
static int loop_netwrite(void *handle,const uint8_t *buffer, int len,uint32_t timeout)
{
    int sndret;
    const uint8_t *data = buffer;
    while(len > 0)
    {
        sndret = dtls_al_write(handle,(uint8_t *)data,len,timeout);
        if(sndret > 0)
        {
            len -= sndret;
            data += sndret;
        }
        else if(sndret == 0)
        {
            break;
        }
    }
    return (len==0)?0:-1;
}

static int loop_netread(void *handle,uint8_t *buffer, int len,uint32_t timeout)
{
    int rcvret;
    const uint8_t *data = buffer;
    while(len > 0)
    {
        rcvret = dtls_al_read(handle,(uint8_t *)data,len,timeout);
        if(rcvret > 0)
        {
            len -= rcvret;
            data += rcvret;
        }
        else if(rcvret == 0)
        {
            break;
        }
    }

    return (len==0)?0:-1;
}

///< use the state machine to check the header
typedef enum
{
    EN_HEADER_STATUS_NORMAL = 0,
    EN_HEADER_STATUS_R,          ///< which means receive the \r
    EN_HEADER_STATUS_RN,         ///< which means receive the \r\n
    EN_HEADER_STATUS_RNR,        ///< which means receive the \r\n\r
    EN_HEADER_STATUS_RNRN,       ///< which means receive the \r\n\r\n
}en_header_status;

static int loop_dealheader(http_section_t *section,void *handle)
{
    int ret = -1;
    int offset = 0;
    char cur;
    en_header_status status = EN_HEADER_STATUS_NORMAL;
    while(status != EN_HEADER_STATUS_RNRN)
    {
        ret = loop_netread(handle,(uint8_t *) &cur, 1, CONFIG_OCMQTT_HTTPS_TIMEOUT);
        if(ret != 0)
        {
            break;
        }
        if(offset < (CONFIG_HTTPOTA_BUFLEN-1))
        {
            section->cache[offset ++] = (uint8_t)cur;   ///< we should check if overblow
        }

        switch (status)
        {
            case EN_HEADER_STATUS_NORMAL:
                if(cur == '\r')
                {
                    status = EN_HEADER_STATUS_R;
                }
                break;
            case EN_HEADER_STATUS_R:
                if(cur == '\n')
                {
                    status = EN_HEADER_STATUS_RN;
                }
                else
                {
                    status = EN_HEADER_STATUS_NORMAL;
                }
                break;
            case EN_HEADER_STATUS_RN:
                if(cur == '\r')
                {
                    status = EN_HEADER_STATUS_RNR;
                }
                else
                {
                    status = EN_HEADER_STATUS_NORMAL;
                }
                break;
            case EN_HEADER_STATUS_RNR:
                if(cur == '\n')
                {
                    status = EN_HEADER_STATUS_RNRN;
                }
                else
                {
                    status = EN_HEADER_STATUS_NORMAL;
                }
                break;
            default:
                break;
        }
    }

    if(offset > 0)
    {
        section->cache[offset] = '\0';
        LINK_LOG_DEBUG("HEADER:%s",section->cache);
    }


    return ret;
}


///< use this function to do the request
static int http_getandrecv(http_section_t *section,void *handle)
{
    int ret;
    int len;
    ///< send the request
    len = http_makerequest(section);
    LINK_LOG_DEBUG("HTTPREQ:%d Bytes REQ:%s\r\n",len,section->cache);

    ret = loop_netwrite(handle, section->cache,len,CONFIG_OCMQTT_HTTPS_TIMEOUT);
    if(ret != 0)
    {
        LINK_LOG_ERROR("TLS REQUEST ERR");
        goto EXIT_HANDLE;
    }
    ///< WE SHOULD DEAL THE HTTP HEADER
    ret = loop_dealheader(section,handle);
    if(ret != 0)
    {
        LINK_LOG_ERROR("HTTP HEADER ERR");
        goto EXIT_HANDLE;
    }

    len = section->len; ///< should check if the response matched the length--TODO
    ret = loop_netread(handle, section->cache,len,CONFIG_OCMQTT_HTTPS_TIMEOUT);

EXIT_HANDLE:
    return ret;
}

///< use this function to do the request and do the storage
static int http_dealsection(http_section_t *section,void *handle,en_ota_type_t otatype)
{
    int ret;

    ret = http_getandrecv(section, handle);
    if(ret == 0)
    {
        LINK_LOG_DEBUG("DOWNLOAD OFFSET:%d LEN:%d SUCCESS",section->offset, section->len);

        ret = ota_img_write(otatype, EN_OTA_IMG_DOWNLOAD, section->offset, section->cache, section->len);
        if( ret == 0)
        {
            LINK_LOG_DEBUG("SAVE OFFSET:%d LEN:%d SUCCESS",section->offset, section->len);
        }
        else
        {
            LINK_LOG_ERROR("SAVE OFFSET:%d LEN:%d FAILED",section->offset, section->len);
        }
    }
    else
    {
        LINK_LOG_ERROR("DOWNLOAD OFFSET:%d LEN:%d FAILED",section->offset, section->len);
    }
    return ret;
}

///< download the file by split it to sections
static int https_filedownload(ota_https_para_t  *param,http_section_t *section)
{
    int ret = 1;
    int file_off;
    int file_lenleft;
    dtls_al_para_t dtls_para;
    void          *handle = NULL;

    ret = ota_img_erase(param->ota_type,EN_OTA_IMG_DOWNLOAD);
    if(ret != 0 )
    {
        LINK_LOG_ERROR("Erase the download img failed");
        goto EXIT_ERASE;
    }
    else
    {
        LINK_LOG_DEBUG("Erase the download img SUCCESS");
    }

    file_off = 0;
    file_lenleft = param->file_size;

    (void) memset( &dtls_para,0, sizeof(dtls_para));
    dtls_para.security.type = EN_DTLS_AL_SECURITY_TYPE_CERT;
    dtls_para.istcp = 1;
    dtls_para.isclient = 1;
    dtls_para.security.u.cert.server_ca = (uint8_t *)g_https_serverca;
    dtls_para.security.u.cert.server_ca_len = sizeof(g_https_serverca);

    if(EN_DTLS_AL_ERR_OK != dtls_al_new(&dtls_para,&handle))
    {
        LINK_LOG_ERROR("TLS HANDLE BUILD ERR");
        goto EXIT_TLSHANDLE;
    }

    ret = dtls_al_connect( handle, section->host, section->port, CONFIG_OCMQTT_HTTPS_TIMEOUT);
    if (ret != EN_DTLS_AL_ERR_OK)
    {
        LINK_LOG_ERROR("TLS CONNECT ERR");
        goto EXIT_TLSCONNECT;
    }

    while(file_lenleft > 0)
    {
        section->offset = file_off;
        section->len = file_lenleft > CONFIG_HTTPOTA_BUFLEN?CONFIG_HTTPOTA_BUFLEN:file_lenleft;

        ret = http_dealsection(section, handle, param->ota_type);
        if(0 == ret)
        {
            file_lenleft -= section->len;
            file_off += section->len;
        }
        else
        {
            break;
        }
    }
    ///< if we download success
    dtls_al_destroy(handle);
    (void)ota_img_flush(param->ota_type, EN_OTA_IMG_DOWNLOAD);
    return ret;


EXIT_TLSCONNECT:
    dtls_al_destroy(handle);
EXIT_TLSHANDLE:
EXIT_ERASE:
    ret = -1;
    return ret;
}



///< return 0 success while -1 failed;
int ota_https_download(ota_https_para_t *param)
{
    int ret = -1;
    http_section_t *section;
    ota_flag_t     otaflag;

    ret = ota_flag_get(param->ota_type, &otaflag);
    if(ret != 0 )
    {
        LINK_LOG_ERROR("GET OTA FLAG FAILED");
        goto EXIT_FLAGGET;
    }

    section = http_decodeurl(param);
    if(NULL == section)
    {
        LINK_LOG_ERROR("DECODE URL FAILED");
        goto EXIT_SECTIONPARAM;
    }

    ret = https_filedownload(param, section);
    if(ret != 0)
    {
        LINK_LOG_ERROR("HTTPS DOWNLOAD FAILED ");
        goto EXIT_DOWNLOAD;
    }

#ifdef CONFIG_OCMQTT_OTASHA256CHECK
    sha256_check_t  sha256_checkpara;

    if(NULL != param->signature)
    {
        sha256_checkpara.data_len = param->file_size;
        sha256_checkpara.data_offset = 0;
        sha256_checkpara.ota_type = param->ota_type;
        sha256_checkpara.sha256 = param->signature;
        ret = sha256_check(&sha256_checkpara);
        if(ret != 0)
        {
            goto EXIT_SIGNATURE;
        }
    }
#endif
    otaflag.info.img_download.file_size = param->file_size;
    otaflag.info.img_download.file_off = param->file_offset;
    strncpy((char *)otaflag.info.img_download.ver,param->version,CONFIG_OTA_VERSIONLEN);
    otaflag.info.curstatus = EN_OTA_STATUS_DOWNLOADED;
    ret = ota_flag_save(param->ota_type,&otaflag);
    if(ret != 0)
    {
        LINK_LOG_ERROR("FLAG SAVE ERR");
        goto EXIT_FLAGSAVE;
    }
    LINK_LOG_DEBUG("DOWNLOAD SUCCESS AND COULD DO UPGRADING");


EXIT_FLAGSAVE:
EXIT_SIGNATURE:
EXIT_DOWNLOAD:
    osal_free(section);
EXIT_SECTIONPARAM:
EXIT_FLAGGET:
   return ret;
}











