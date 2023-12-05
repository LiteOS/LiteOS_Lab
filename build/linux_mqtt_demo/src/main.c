/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"
#include "dtls_al.h"
#include "mqtt_al.h"
#include "oc_mqtt_al.h"
#include "oc_mqtt_event.h"
#include "oc_mqtt_profile.h"
#include "queue.h"
#include "iot_config.h"
#include "securec.h"

 #define CONFIG_OC_MQTTV5_DEMO_TLS 1

#ifdef CONFIG_OC_MQTTV5_BS
#define CN_SERVER_IPV4         "iot-bs.cn-north-4.myhuaweicloud.com"
//#define CN_SERVER_IPV4         "119.3.251.30"
#define CN_BOOT_MODE            1
#else
//#define CN_SERVER_IPV4     "2407:c080:17ef:ffff::7b3c:d5b1"//"123.60.213.177"// "iot-mqtts.cn-north-4.myhuaweicloud.com"
#define CN_SERVER_IPV4         "121.36.42.100"
#define CN_BOOT_MODE            0
#endif

#ifdef CONFIG_OC_MQTTV5_DEMO_TLS
#define CN_SERVER_PORT "8883"
#define CN_SECURITY_TYPE EN_DTLS_AL_SECURITY_TYPE_CERT

///< server key
static const char s_server_ca[] =
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
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDXzCCAkegAwIBAgILBAAAAAABIVhTCKIwDQYJKoZIhvcNAQELBQAwTDEgMB4G\r\n"
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjMxEzARBgNVBAoTCkdsb2JhbFNp\r\n"
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDkwMzE4MTAwMDAwWhcNMjkwMzE4\r\n"
"MTAwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMzETMBEG\r\n"
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\r\n"
"hvcNAQEBBQADggEPADCCAQoCggEBAMwldpB5BngiFvXAg7aEyiie/QV2EcWtiHL8\r\n"
"RgJDx7KKnQRfJMsuS+FggkbhUqsMgUdwbN1k0ev1LKMPgj0MK66X17YUhhB5uzsT\r\n"
"gHeMCOFJ0mpiLx9e+pZo34knlTifBtc+ycsmWQ1z3rDI6SYOgxXG71uL0gRgykmm\r\n"
"KPZpO/bLyCiR5Z2KYVc3rHQU3HTgOu5yLy6c+9C7v/U9AOEGM+iCK65TpjoWc4zd\r\n"
"QQ4gOsC0p6Hpsk+QLjJg6VfLuQSSaGjlOCZgdbKfd/+RFO+uIEn8rUAVSNECMWEZ\r\n"
"XriX7613t2Saer9fwRPvm2L7DWzgVGkWqQPabumDk3F2xmmFghcCAwEAAaNCMEAw\r\n"
"DgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFI/wS3+o\r\n"
"LkUkrk1Q+mOai97i3Ru8MA0GCSqGSIb3DQEBCwUAA4IBAQBLQNvAUKr+yAzv95ZU\r\n"
"RUm7lgAJQayzE4aGKAczymvmdLm6AC2upArT9fHxD4q/c2dKg8dEe3jgr25sbwMp\r\n"
"jjM5RcOO5LlXbKr8EpbsU8Yt5CRsuZRj+9xTaGdWPoO4zzUhw8lo/s7awlOqzJCK\r\n"
"6fBdRoyV3XpYKBovHd7NADdBj+1EbddTKJd+82cEHhXXipa0095MJ6RMG3NzdvQX\r\n"
"mcIfeg7jLQitChws/zyrVQ4PkX4268NXSb7hLi18YIvDQVETI53O9zJrlAGomecs\r\n"
"Mx86OyXShkDOOyyGeMlhLxS67ttVb9+E7gUJTb0o2HLO02JQZR7rkpeDMdmztcpH\r\n"
"WD9f\r\n"
"-----END CERTIFICATE-----\r\n";

#ifdef CONFIG_OC_MQTTV5_TLS_BIDIRECTION

///< the client use the cert mode  SHA-256
//72DC0E75D88CEC38A025E9C48C79D222F608B039D080BCEFC0007DAD1AFFAD00
//72dc0e7
///< bidirection  mqtt mode
static const char s_client_ca[] = \
"-----BEGIN CERTIFICATE-----\r\n"
"MIICuDCCAaACCQDAnCoP3W+otTANBgkqhkiG9w0BAQsFADAdMQswCQYDVQQGEwJD\r\n"
"QTEOMAwGA1UEAwwFTVkgQ0EwHhcNMjAwMjE3MTEyNTA2WhcNMjEwMjExMTEyNTA2\r\n"
"WjAfMQswCQYDVQQGEwJDTjEQMA4GA1UEAwwHQ0xJRU5UMjCCASIwDQYJKoZIhvcN\r\n"
"AQEBBQADggEPADCCAQoCggEBALKOOfeq1QQnwLNsMboqr47ClO24sTd6oLiSjGYe\r\n"
"zb7IECnjiEg+sILEH5avi6uKdCR8QZHskIWdQbNqi2h0E01MTxuneTCnFaAdYh2z\r\n"
"B8n4IUsGr1gyCayfKCQG8ElDElBkB2JMce/IEuxJvnFdhXy6c9zhj8iU+fusD9hm\r\n"
"jvx34zd6Sn2uRl8wdI89pVwkESKyx6J5za/dBWCS0hxvOtDEY2gJA9av8BNYjTxQ\r\n"
"tRlwnxGzgaza8aA9LMbnRmv1bML98w4NgNHkVGUtw1eQso0unHnyEF1WhqEu0pLk\r\n"
"BJQbdO4s4ajQ1rOr7niVnCT99KtzJuWm3oymNdDT6BHsnn0CAwEAATANBgkqhkiG\r\n"
"9w0BAQsFAAOCAQEAPayFtS8du8D+mPdRU96vFqVpDx6oksGhrUIO+3GZ4ruuHjtf\r\n"
"A764y6zD1/LxQVw15xN44G+AqUwEi/BZszQ+5rSitwqoZFciKMblaqd0ZJSG6G2d\r\n"
"Cqyl87cTL6Tlgpa5cBka4ksaMs2OVU9x1aaNnejrTbX2PvZO7o/pVVimQE8Yh+1J\r\n"
"k2D26hg9UoWN4Z3/9LdGC3y4dsmtuQS+Cye61ZfHtT3/XjTRjl9/c+xKtVsQpE6g\r\n"
"dpaGVxWpFUCpGe4POrNHjOOAXaFiCHP7o9Kab1UnxuFVKHCtNT9wdJ7v8puCliwB\r\n"
"t40h9oegMyUYL/VG+14wOYgEPuCeGqqilht+4w==\r\n"
"-----END CERTIFICATE-----\r\n";
static const char s_client_pk[]= \
"-----BEGIN PRIVATE KEY-----\r\n"
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCyjjn3qtUEJ8Cz\r\n"
"bDG6Kq+OwpTtuLE3eqC4koxmHs2+yBAp44hIPrCCxB+Wr4urinQkfEGR7JCFnUGz\r\n"
"aotodBNNTE8bp3kwpxWgHWIdswfJ+CFLBq9YMgmsnygkBvBJQxJQZAdiTHHvyBLs\r\n"
"Sb5xXYV8unPc4Y/IlPn7rA/YZo78d+M3ekp9rkZfMHSPPaVcJBEisseiec2v3QVg\r\n"
"ktIcbzrQxGNoCQPWr/ATWI08ULUZcJ8Rs4Gs2vGgPSzG50Zr9WzC/fMODYDR5FRl\r\n"
"LcNXkLKNLpx58hBdVoahLtKS5ASUG3TuLOGo0Nazq+54lZwk/fSrcyblpt6MpjXQ\r\n"
"0+gR7J59AgMBAAECggEAFbjx2myhD6laazmdbx2kUATiI22Vub8kcG/7vqy+sRgb\r\n"
"x0zUFp3x0WbUxPtETbFBYfLv8zXJe0ffZ1lt7l44vu2qrh2Yw89/9QO+HQjbyLl0\r\n"
"K3F7wYoM3UBbnM5vSypqvvb8YvwT3G0FyFNMZPRQuhTg5Z1XZbqiEvki0Fvhx1sG\r\n"
"bksCKK85dPaH/7kXxMTPZ67/2Z3TlwUERo1YSPRnrQXGnDowudLqQjn8ptiPfO39\r\n"
"bIMQhM/HI1bTw4F5EevL7sS4rcnhpmQNU6ZeINPvYuel+hP36uPwTHMm1hkZbMgP\r\n"
"a69Yk8/meENffl9fQ+T8woYMGgKq0DM+O5ip7QzrLQKBgQDdvwI/9npuwpYkdjWT\r\n"
"tLj5BKFNINjn+TjyvN1QQmOhXqzTy1y9HVm7EYh1l0H5PY/WgYadBWNYabgNSIe0\r\n"
"vxl7tdDxDX4DBQqDIIEY45sPa3glq6flCgtPzkZxcYTvZpqmdX8/iMYGXHpdOMdm\r\n"
"S8jngobFDEgLxFS1lLLs+LtTTwKBgQDOIz08d2zMRX5AW0SkQ+VbdwcO7G5qlTJV\r\n"
"42Rf51Dmr0as/bX5eIWAAfcsEi/kG2e5Ozupe4BVZXarw7AL1bzE4mLvfcwZEUiJ\r\n"
"Qj56H6QTOiVm/DTZdBz415z60zjCui+59luev6txZBL51ZhoifzfdAfxd8ztucV+\r\n"
"mj3kytIucwKBgBSdporl0norsyPOrap+2dek+44dKK1geU/wCS9mDLc7AxxpwsR+\r\n"
"TxJaTfKIzoRqmc3amvZMQvE8WvU5Mgy225qpy7QtifKj7FdGlVoIBymG11PBQsnD\r\n"
"hku0JM51PGO5iVVChUMV5zs+K84w8jDriNElTHxTaPtXFH0RRxVLBrd3AoGAIYX1\r\n"
"lXnOhcHfcFPna9dm22MWEMU+h7LPxsv+LOKPc5SZp9Ry3alN/K1VJyOmCjCyw5rw\r\n"
"bvWYv0qK7BAc9y/BTTuL32rT4FfBNiFBy9aNdlsEbiUh6yNI+ucFrnzfRDvHOzTn\r\n"
"XX/KP9yIgXzNFmMqbqSODiTvfNGzXo1y91xMpiUCgYAfO+Xb5msOMABiBNGD85qr\r\n"
"C+0xpuWoPe4pB5ccgniv1zhPlUJ+8l1vWz/ZWnH/6oVxGGrq5xLtoBgFWM4uSseI\r\n"
"Je0fSqZp0XAbaFI7wkEV/uP6VMjjFT5TfJz4idgI+EaE72va2jbFBgg1MyfR3gx4\r\n"
"deaXc4ZpE64iV+eAtZ8VVQ==\r\n"
"-----END PRIVATE KEY-----\r\n";
static const char *s_client_pk_pwd = "123456";
#endif

#else
#define CN_SERVER_PORT "1883"
#define CN_SECURITY_TYPE EN_DTLS_AL_SECURITY_TYPE_NONE
#endif

#define CN_APP_DEVICEID  "60bf519cb86d7b02bc518aa9_softbusTiny26" // 替换为注册设备后生成的deviceid

#define CN_APP_DEVICEPWD "13101518025" // 替换为注册设备后生成的密钥

#define CN_APP_LIFETIME 60 // seconds
typedef enum {
    en_msg_report = 8,
    en_msg_conn,
    en_msg_disconn,
} en_msg_type_t; 

typedef struct {
    char* request_id;
    char* payload;
} cmd_t;

typedef struct {
    int lum;
    int temp;
    int hum;
} report_t;

typedef struct {
    en_msg_type_t msg_type;
    union {
        cmd_t cmd;
        report_t report;
    } msg;
} app_msg_t;

typedef struct {
    queue_t *app_msg;
    int connected;
    int led;
    int motor;
} app_cb_t;
static app_cb_t g_app_cb;

extern int link_main(void *args);

static void deal_report_msg(report_t* report)
{
    oc_mqtt_profile_service_t service;
    oc_mqtt_profile_kv_t temperature;
    oc_mqtt_profile_kv_t humidity;
    oc_mqtt_profile_kv_t luminance;
    oc_mqtt_profile_kv_t led;
    oc_mqtt_profile_kv_t motor;

    if (g_app_cb.connected != 1) {
        return;
    }

    service.event_time = NULL;
    service.service_id = "Agriculture";
    service.service_property = &temperature;
    service.nxt = NULL;

    temperature.key = "Temperature";
    temperature.value = &report->temp;
    temperature.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    temperature.nxt = &humidity;

    humidity.key = "Humidity";
    humidity.value = &report->hum;
    humidity.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    humidity.nxt = &luminance;

    luminance.key = "Luminance";
    luminance.value = &report->lum;
    luminance.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    luminance.nxt = &led;

    led.key = "LightStatus";
    led.value = g_app_cb.led ? "ON" : "OFF";
    led.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    led.nxt = &motor;

    motor.key = "MotorStatus";
    motor.value = g_app_cb.motor ? "ON" : "OFF";
    motor.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    motor.nxt = NULL;

    oc_mqtt_profile_propertyreport(NULL, &service);
    return;
}

int GetShadowByServiceId(char *serviceId)
{
    int ret;
    oc_mqtt_profile_shadowget_t payload;
    payload.object_device_id = NULL;
    payload.request_id = "$get_device_shadow";

    // if service_id is NULL can get all service shadow
    payload.service_id = serviceId;
    ret = oc_mqtt_profile_getshadow(NULL, &payload);
    return ret;
}

void ReportSDKInfo()
{
    printf("hello xingli\n");
    oc_mqtt_profile_event_t service;
    oc_mqtt_profile_kv_t sdk;
    oc_mqtt_profile_kv_t sw;
    oc_mqtt_profile_kv_t fw;
    oc_mqtt_profile_kv_t device_ip;

    service.event_time = NULL;
    service.service_id = "$sdk_info";
    service.event_type = "sdk_info_report";
    service.paras = &sdk;

    sdk.key = "device_sdk_version";
    sdk.value = "C_v2.2.2";
    sdk.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    sdk.nxt = &sw;

    sw.key = "sw_version";
    sw.value = "v1.0";
    sw.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    sw.nxt = &fw;

    fw.key = "fw_version";
    fw.value = "v1.0";
    fw.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    fw.nxt = &device_ip;

    device_ip.key = "device_ip";
    device_ip.value = "127.0.0.1";
    device_ip.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    device_ip.nxt = NULL;

    oc_mqtt_profile_reportevent(NULL, &service);
    printf("=======hello this is aimin=====");
    return;
}



// use this function to push all the message to the buffer
static int msg_rcv_callback(oc_mqtt_profile_msgrcv_t* msg)
{
    int ret = 0;
    char* buf;
    int buf_len;
    app_msg_t* app_msg;

    if (msg == NULL) {
        return ret;
    }

    if (msg->request_id == NULL) {
        buf_len = sizeof(app_msg_t) + msg->msg_len + 1;
    } else {
        buf_len = sizeof(app_msg_t) + strlen(msg->request_id) + 1 + msg->msg_len + 1;
    }
    
    buf = malloc(buf_len);
    if (buf == NULL) {
        return ret;
    }
    app_msg = (app_msg_t*)buf;
    buf += sizeof(app_msg_t);

    app_msg->msg_type = msg->type;
    app_msg->msg.cmd.request_id = buf;
    if (msg->request_id != NULL) {
        buf_len = strlen(msg->request_id);
        buf += (buf_len + 1);
        memcpy_s(app_msg->msg.cmd.request_id, buf_len, msg->request_id, buf_len);
        app_msg->msg.cmd.request_id[buf_len] = '\0';
    }

    buf_len = msg->msg_len;
    app_msg->msg.cmd.payload = buf;
    memcpy_s(app_msg->msg.cmd.payload, buf_len, msg->msg, buf_len);
    app_msg->msg.cmd.payload[buf_len] = '\0';

    ret = queue_push(g_app_cb.app_msg, app_msg, CONFIG_QUEUE_TIMEOUT);
    if (ret != 0) {
        free(app_msg);
    }

    return ret;
}

static void oc_cmdresp(cmd_t* cmd, int cmdret)
{
    oc_mqtt_profile_cmdresp_t cmdresp;
    ///< do the response
    cmdresp.paras = NULL;
    cmdresp.request_id = cmd->request_id;
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = NULL;
    (void)oc_mqtt_profile_cmdresp(NULL, &cmdresp);
}

static void oc_setresp(char *request_id, int ret_code)
{
    oc_mqtt_profile_propertysetresp_t setresp;
    setresp.request_id = request_id;
    setresp.ret_code = ret_code;
    setresp.ret_description = NULL;
    (void)oc_mqtt_profile_propertysetresp(NULL, &setresp);
}

static void deal_cmd_msg(cmd_t* cmd)
{
    cJSON* obj_root;
    cJSON* obj_cmdname;

    int cmdret = 1;
    obj_root = cJSON_Parse(cmd->payload);

    if (obj_root == NULL) {
        goto EXIT_JSONPRASE;
    }

    obj_cmdname = cJSON_GetObjectItem(obj_root, "command_name");
    if (obj_cmdname == NULL) {
        goto EXIT_CMDOBJ;
    }
    
    if (strcmp(cJSON_GetStringValue(obj_cmdname), "Agriculture_Control_light") == 0) {
        //cmdret = deal_light_cmd(cmd, obj_root);
    } else if (strcmp(cJSON_GetStringValue(obj_cmdname), "Agriculture_Control_Motor") == 0) {
        //cmdret = deal_motor_cmd(cmd, obj_root);
    }
    
EXIT_CMDOBJ:
    cJSON_Delete(obj_root);
EXIT_JSONPRASE:
    oc_cmdresp(cmd, cmdret);
    return;
}

static int CloudMainTaskEntry(void *args)
{
    app_msg_t* app_msg;
    uint32_t ret;
    g_app_cb.app_msg = queue_create("queue_rcvmsg", 10, 1);
    if (g_app_cb.app_msg == NULL) {
        printf("Create receive msg queue failed");
    }
    oc_mqtt_profile_connect_t connect_para;
    (void)memset(&connect_para, 0, sizeof(connect_para));

    connect_para.boostrap = CN_BOOT_MODE;
    connect_para.device_id = CN_APP_DEVICEID;
    connect_para.device_passwd = CN_APP_DEVICEPWD;
    connect_para.server_addr = CN_SERVER_IPV4;
    connect_para.server_port = CN_SERVER_PORT;
    connect_para.life_time = CN_APP_LIFETIME;
    connect_para.rcvfunc = msg_rcv_callback;

    connect_para.security.type = CN_SECURITY_TYPE;

#ifdef CONFIG_OC_MQTTV5_DEMO_TLS
    connect_para.security.u.cert.server_ca = (uint8_t *)s_server_ca;
    connect_para.security.u.cert.server_ca_len = sizeof(s_server_ca);
#ifdef CONFIG_OC_MQTTV5_TLS_BIDIRECTION
    connect_para.device_passwd = NULL;
    connect_para.security.u.cert.client_ca = (uint8_t *)s_client_ca;
    connect_para.security.u.cert.client_ca_len = sizeof(s_client_ca);
    connect_para.security.u.cert.client_pk = (uint8_t *)s_client_pk;
    connect_para.security.u.cert.client_pk_len = sizeof(s_client_pk);
    connect_para.security.u.cert.client_pk_pwd = (uint8_t *)s_client_pk_pwd;
    connect_para.security.u.cert.client_pk_pwd_len = strlen(s_client_pk_pwd);
#endif   ///<endfor bidirection
#endif   ///<endfor tls

    ret = oc_mqtt_profile_connect(&connect_para);
    printf("ret = %u\n", ret);
    if ((int)en_oc_mqtt_err_ok == ret) {
        g_app_cb.connected = 1;
        printf("oc_mqtt_profile_connect succed!\r\n");
    } else {
        printf("oc_mqtt_profile_connect faild!\r\n");
    }

    GetShadowByServiceId(NULL);

    while (1) {
        app_msg = NULL;
        (void)queue_pop(g_app_cb.app_msg, (void**)&app_msg, (int)cn_osal_timeout_forever);
        if (app_msg != NULL) {
            switch (app_msg->msg_type) {
                case (en_msg_type_t)EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_MSGDOWN:
                    deal_cmd_msg(&app_msg->msg.cmd);
                    break;
                case en_msg_report:
                    deal_report_msg(&app_msg->msg.report);
                    break;
                default:
                    break;
            }
            free(app_msg);
        }
    }
    return 0;
}

static int SensorTaskEntry(void *args)
{
    app_msg_t* app_msg;

    printf("SensorTaskEntry start!\r\n");
        
    while (1) {
        printf("SensorTaskEntry loop start!\r\n");
            
        app_msg = malloc(sizeof(app_msg_t));
        if (app_msg != NULL) {
            app_msg->msg_type = en_msg_report;
            app_msg->msg.report.hum = 52;
            app_msg->msg.report.lum = 227;
            app_msg->msg.report.temp = 27;
            if (queue_push(g_app_cb.app_msg, app_msg, CONFIG_QUEUE_TIMEOUT) != 0) {
                free(app_msg);
            }
        }
        osal_task_sleep(5000);
    }
    return 0;
}

int main(void)
{
    link_main(NULL);
    (void) osal_task_create("CloudMainTaskEntry", CloudMainTaskEntry, NULL, 0x2000, NULL, 24);
    (void) osal_task_create("SensorTaskEntry", SensorTaskEntry, NULL, 0x1000, NULL, 23);
    while (1)
    {
        osal_task_sleep(2000);
    }
    return 0;
}