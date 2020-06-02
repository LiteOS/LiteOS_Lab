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
 *  2019-08-08 09:06  zhangqianfu  The first version
 *
 */
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <link_endian.h>
#include <shell.h>
#include <time.h>
#include <osal.h>
#include <dtls_interface.h>

static char s_mqtt_ca_crt[] =
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

///< return 0 success while -1 failed
static int __test_tls_demo(const char *host,const char *port)
{

    int ret = -1;
    mbedtls_ssl_context *ssl;
    dtls_shakehand_info_s shakehand_info;
    dtls_establish_info_s establish_info;

    memset(&shakehand_info, 0, sizeof(dtls_shakehand_info_s));

    establish_info.psk_or_cert = VERIFY_WITH_CERT;
    establish_info.v.c.ca_cert = (const unsigned char *)s_mqtt_ca_crt;
    establish_info.v.c.cert_len = sizeof(s_mqtt_ca_crt);
    shakehand_info.psk_or_cert = VERIFY_WITH_CERT;
    establish_info.udp_or_tcp = MBEDTLS_NET_PROTO_TCP;

    ssl = (void *)dtls_ssl_new(&establish_info, MBEDTLS_SSL_IS_CLIENT);
    if (NULL == ssl)
    {

        link_printf("dtls_ssl_new failed\n\r");
        goto EXIT_SSLNEW;
    }

    shakehand_info.client_or_server = MBEDTLS_SSL_IS_CLIENT;
    shakehand_info.udp_or_tcp = MBEDTLS_NET_PROTO_TCP;
    shakehand_info.u.c.host = host;
    shakehand_info.u.c.port = port;

    ret = dtls_shakehand(ssl, &shakehand_info);
    if (ret != 0)
    {
        link_printf("dtls_shakehand failed\n\r");
        goto EXIT_HANDSHAKE;
    }
    dtls_ssl_destroy(ssl);
    return 0;

EXIT_HANDSHAKE:
    dtls_ssl_destroy(ssl);
EXIT_SSLNEW:
    return -1;
}

static int tls_test_main(int argc, const char *argv[])
{
    int test_counter = 0;
    int test_ret = 0;
    int test_success = 0;
    const char *host;
    const char *port;
    int         test_times_limit;


    if(argc != 4)
    {
        return -1;
    }

    host = argv[1];
    port = argv[2];
    test_times_limit = atoi(argv[3]);


    for(test_counter = 1; test_counter <= test_times_limit; test_counter ++)
    {
        test_ret = __test_tls_demo(host,port);
        if(test_ret  == 0)
        {
            test_success++;
        }
        link_printf("TEST:%d SUCCESS:%d\n\r", test_counter,test_success);

        osal_task_sleep(1000);
    }

    return 0;
}
OSSHELL_EXPORT_CMD(tls_test_main,"tlstest","tlstest host port times");



