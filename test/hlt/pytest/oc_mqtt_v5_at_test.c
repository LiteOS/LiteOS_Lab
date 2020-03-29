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
 *  2020-02-29 10:35  zhangqianfu  The first version
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <shell.h>
#include <osal.h>
#include <oc_mqtt_at.h>
#include <oc_mqtt_al.h>

#ifndef CONFIG_OCMQTTDEMOS_ENABLE



#define CN_LOOP_TIMES  1000

#define CN_BS_ID       "mqtt_sdk03"
#define CN_BS_PWD      "f62fcf47d62c4ed18913"
#define CN_BS_SERVER   "119.3.251.30"
#define CN_BS_PORT     "8883"

#define CN_HUB_ID       "54f107da-f251-436c-af4c-624f33b7d7b6"
#define CN_HUB_PWD      "f62fcf47d62c4ed18913"
#define CN_HUB_SERVER   "119.3.248.253"
#define CN_HUB_PORT     "8883"


#define CN_OC_LIFE_TIME  60
#define CN_PUB_DATA     "{\"services\":[{\"service_id\":\"DeviceStatus\",\"properties\":{\"radioValue\":2}}]}"
#define CN_PUB_TOPIC    "$oc/devices/54f107da-f251-436c-af4c-624f33b7d7b6/sys/properties/report"

#define CN_SELFSUB_TOPIC "$oc/devices/54f107da-f251-436c-af4c-624f33b7d7b6/user/demo_sub"
#define CN_SELFPUB_TOPIC "$oc/devices/54f107da-f251-436c-af4c-624f33b7d7b6/user/demo_pub"



static const char s_server_ca[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIID4DCCAsigAwIBAgIJAK97nNS67HRvMA0GCSqGSIb3DQEBCwUAMFMxCzAJBgNV\r\n"
"BAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UEBxMCU1oxDzANBgNVBAoTBkh1YXdl\r\n"
"aTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lPVDAeFw0xNjA1MDQxMjE3MjdaFw0y\r\n"
"NjA1MDIxMjE3MjdaMFMxCzAJBgNVBAYTAkNOMQswCQYDVQQIEwJHRDELMAkGA1UE\r\n"
"BxMCU1oxDzANBgNVBAoTBkh1YXdlaTELMAkGA1UECxMCQ04xDDAKBgNVBAMTA0lP\r\n"
"VDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJxM9fwkwvxeILpkvoAM\r\n"
"Gdqq3x0G9o445F6Shg3I0xmmzu9Of8wYuW3c4jtQ/6zscuIGyWf06ke1z//AVZ/o\r\n"
"dp8LkuFbBbDXR5swjUJ6z15b6yaYH614Ty/d6DrCM+RaU+FWmxmOon9W/VELu2BB\r\n"
"NXDQHJBSbWrLNGnZA2erk4JSMp7RhHrZ0QaNtT4HhIczFYtQ2lYF+sQJpQMrjoRn\r\n"
"dSV9WB872Ja4DgcISU1+wuWLmS/NKjIvOWW1upS79yu2I4Rxos2mFy9xxz311rGC\r\n"
"Z3X65ejFNzCUrNgf6NEP1N7wB9hUu7u50aA+/56D7EgjeI0gpFytC+a4f6JCPVWI\r\n"
"Lr0CAwEAAaOBtjCBszAdBgNVHQ4EFgQUcGqy59oawLEgMl21//7F5RyABpwwgYMG\r\n"
"A1UdIwR8MHqAFHBqsufaGsCxIDJdtf/+xeUcgAacoVekVTBTMQswCQYDVQQGEwJD\r\n"
"TjELMAkGA1UECBMCR0QxCzAJBgNVBAcTAlNaMQ8wDQYDVQQKEwZIdWF3ZWkxCzAJ\r\n"
"BgNVBAsTAkNOMQwwCgYDVQQDEwNJT1SCCQCve5zUuux0bzAMBgNVHRMEBTADAQH/\r\n"
"MA0GCSqGSIb3DQEBCwUAA4IBAQBgv2PQn66gRMbGJMSYS48GIFqpCo783TUTePNS\r\n"
"tV8G1MIiQCpYNdk2wNw/iFjoLRkdx4va6jgceht5iX6SdjpoQF7y5qVDVrScQmsP\r\n"
"U95IFcOkZJCNtOpUXdT+a3N+NlpxiScyIOtSrQnDFixWMCJQwEfg8j74qO96UvDA\r\n"
"FuTCocOouER3ZZjQ8MEsMMquNEvMHJkMRX11L5Rxo1pc6J/EMWW5scK2rC0Hg91a\r\n"
"Lod6aezh2K7KleC0V5ZlIuEvFoBc7bCwcBSAKA3BnQveJ8nEu9pbuBsVAjHOroVb\r\n"
"8/bL5retJigmAN2GIyFv39TFXIySw+lW0wlp+iSPxO9s9J+t\r\n"
"-----END CERTIFICATE-----\r\n";
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



static int demo_hubdefaultconnect()
{
    int ret =0;
    int loop = 0;

    hwoc_mqtt_disconnect();
    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        link_printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);

        if(0 != hwoc_mqtt_connect(0,CN_OC_LIFE_TIME,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
        {
            break;
        }
        hwoc_mqtt_disconnect();
        osal_task_sleep(100);

    }
    if(loop == CN_LOOP_TIMES)
    {
        link_printf("OC MQTT HUB CONNECT TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        link_printf("OC MQTT HUB CONNECT TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;

}

static int demo_hubpkconnect()
{
    int ret =0;
    int loop = 0;

    hwoc_mqtt_disconnect();

    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        link_printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);
        if( 0 != hwoc_mqtt_serverca(s_server_ca))
        {
            break;
        }

        if(0 !=  hwoc_mqtt_clientca(s_client_ca))
        {
            break;
        }
        if( 0 != hwoc_mqtt_clientpk(s_client_pk,s_client_pk_pwd))
        {
            break;
        }
        if(0 != hwoc_mqtt_connect(0,CN_OC_LIFE_TIME,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
        {
            break;
        }
        hwoc_mqtt_disconnect();
        osal_task_sleep(100);

    }
    if(loop == CN_LOOP_TIMES)
    {
        link_printf("OC MQTT HUB CONNECT TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        link_printf("OC MQTT HUB CONNECT TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;

}


static int demo_selfsub()
{

    int ret = -1;
    int loop = 0;


    hwoc_mqtt_disconnect();
    if(0 != hwoc_mqtt_connect(0,CN_OC_LIFE_TIME,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
    {
        return ret;
    }

    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {

        link_printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);
        if(0 != hwoc_mqtt_subscribe(loop%3,CN_SELFSUB_TOPIC))
        {
            break;
        }

        if(0 != hwoc_mqtt_publish(loop%3,CN_SELFPUB_TOPIC,(uint8_t *)CN_PUB_DATA,strlen(CN_PUB_DATA)))
        {
            break;
        }

        if(0 != hwoc_mqtt_unsubscribe(CN_SELFSUB_TOPIC))
        {
            break;
        }
        osal_task_sleep(100);
    }
    hwoc_mqtt_disconnect();

    if(loop == CN_LOOP_TIMES)
    {
        link_printf("OC MQTT HUB PUBLISH  TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        link_printf("OC MQTT HUB PUBLISH  TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;
}


static int demo_hubpublish()
{
    int ret = -1;
    int loop = 0;


    hwoc_mqtt_disconnect();
    if(0 != hwoc_mqtt_connect(0,CN_OC_LIFE_TIME,CN_HUB_SERVER,CN_HUB_PORT,CN_HUB_ID,CN_HUB_PWD))
    {
        return ret;
    }

    for(loop = 0; loop<CN_LOOP_TIMES;loop++ )
    {
        link_printf("***************%s:LOOP:%d************************\n\r",__FUNCTION__,loop);
        if(0 != hwoc_mqtt_publish(loop%3,CN_PUB_TOPIC,(uint8_t *)CN_PUB_DATA,strlen(CN_PUB_DATA)))
        {
            break;
        }
        osal_task_sleep(100);
    }
    hwoc_mqtt_disconnect();

    if(loop == CN_LOOP_TIMES)
    {
        link_printf("OC MQTT HUB PUBLISH  TEST SUCCESS------------------\n\r");
        ret = 0;
    }
    else
    {
        link_printf("OC MQTT HUB PUBLISH  TEST FAILED ++++++++++++++++++++\n\r");
        ret =-1;
    }
    return ret;
}


typedef  int (*fn_demo_fun)(void);

static fn_demo_fun s_demo_tab[]=
{
//    demo_bshubconnect,
//    demo_bspublish,
//    demo_bsconnect,
    demo_hubpkconnect,
    demo_selfsub,
    demo_hubpublish,
    demo_hubdefaultconnect,
    NULL,
};



static int entry_ocmqttloop(void *arg)
{
    int ret = 0;
    int i = 0;
    fn_demo_fun demo_fun;

    link_printf("LOOP TEST START........\n\r");
    for(i =0;;i++)
    {
        demo_fun = s_demo_tab[i];
        if(demo_fun != NULL)
        {
            ret = demo_fun();

            if(0 != ret)
            {
                link_printf("demo:%d failed, and test exit\n\r ",i);
            }
        }
        else
        {
            break;
        }
    }
    link_printf("LOOP TEST END..........\n\r");

    return 0;
}

int standard_app_demo_main(int argc, const char *argv[])
{
    osal_task_create("ocmqtttest",entry_ocmqttloop,NULL,0x1000,NULL,10);

    return 0;
}


OSSHELL_EXPORT_CMD(standard_app_demo_main,"ocmqttloop","ocmqttloop test");


#endif

