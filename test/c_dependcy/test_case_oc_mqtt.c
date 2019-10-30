
#include <stdio.h>
#include <stdlib.h>
#include <sal.h>
#include <osal.h>
#include <oc_mqtt_al.h>
#include <mqtt_al.h>

#include "test_case.h"

static char s_mqtt_ca_crt[] =
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

#define DEFAULT_SERVER_IPV4         "49.4.93.24"     ///<  server ip address
#define DEFAULT_SERVER_PORT         "8883"           ///<  server mqtt service port
#define CN_OC_MQTT_CLIENTID_STATIC_FMT     "%s_%d_%d_%s"
#define cn_check_time_value                 "2018111517"
void *g_mqtt_al_handle = NULL;
#define DEFAULT_LIFETIME            10  ///<  server mqtt service port


char *g_mqtt_ep_noteid = NULL;
char *g_mqtt_ep_deviceid = NULL;
char *g_mqtt_ep_passwd = NULL;
char *g_mqtt_clientdata = NULL;

#define TEST_OC_MQTT_INIT             ((TEST_SORT_OC_MQTT_AL << 16) | 0)
#define TEST_OC_MQTT_REGISTER         ((TEST_SORT_OC_MQTT_AL << 16) | 1)
#define TEST_OC_MQTT_CONFIG           ((TEST_SORT_OC_MQTT_AL << 16) | 2)
#define TEST_OC_MQTT_JSON_FMT_REQ     ((TEST_SORT_OC_MQTT_AL << 16) | 3)
#define TEST_OC_MQTT_JSON_FMT_RES     ((TEST_SORT_OC_MQTT_AL << 16) | 4)
#define TEST_OC_MQTT_REPORT           ((TEST_SORT_OC_MQTT_AL << 16) | 5)
#define TEST_OC_MQTT_DECONFIG         ((TEST_SORT_OC_MQTT_AL << 16) | 6)
#define TEST_OC_MQTT_DEINIT           ((TEST_SORT_OC_MQTT_AL << 16) | 7)


/*OC MQTT*/
static int ts_oc_mqtt_init(char *message, int len);
static int ts_oc_mqtt_register(char *message, int len);
static int ts_oc_mqtt_config(char *message, int len);
static int ts_oc_mqtt_json_fmt_req(char *message, int len);
static int ts_oc_mqtt_json_fmt_res(char *message, int len);
static int ts_oc_mqtt_report(char *message, int len);
static int ts_oc_mqtt_deconfig(char *message, int len);
static int ts_oc_mqtt_deinit(char *message, int len);

test_entry ts_oc_mqtt_entry_flist[]= {
    ts_oc_mqtt_init,
    ts_oc_mqtt_register,
    ts_oc_mqtt_config,
    ts_oc_mqtt_json_fmt_req,
    ts_oc_mqtt_json_fmt_res,
    ts_oc_mqtt_report,
    ts_oc_mqtt_deconfig,
    ts_oc_mqtt_deinit,
};

int ts_sort_oc_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id > 7)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }

    return (ts_oc_mqtt_entry_flist[entry_id])(message, len);
}

/*--------------oc mqtt test---------------------------*/

#define cn_app_rcv_buf_len 256
static char            s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;
static void           *s_mqtt_handle;
static int            s_cmd_entry_live = 1;
static void           *task_handle = NULL;
static int at_oc_app_msg_deal(void *handle,mqtt_al_msgrcv_t *msg)
{
    int ret = -1;
    printf("topic:%s qos:%d\n\r",msg->topic.data,msg->qos);

    if(msg->msg.len < cn_app_rcv_buf_len)
    {
        memcpy(s_rcv_buffer,msg->msg.data,msg->msg.len );
        s_rcv_buffer[msg->msg.len] = '\0'; ///< the json need it
        s_rcv_datalen = msg->msg.len;

        printf("msg:%s\n\r",s_rcv_buffer);

        osal_semp_post(s_rcv_sync);
        ret = 0;

    }
    return ret;
}

static int at_oc_mqtt_cmd_entry(void *args)
{
    cJSON  *msg = NULL;
    cJSON  *mid = NULL;
    cJSON  *ioswitch = NULL;
    cJSON  *msgType = NULL;
    cJSON  *paras = NULL;
    cJSON  *serviceId = NULL;
    cJSON  *cmd = NULL;
    char   *buf = NULL;

    tag_oc_mqtt_response response;
    tag_key_value_list   list;

    int mid_int;
    int err_int;

    printf("at_oc_mqtt_cmd_entry start now !!\n");
    while(s_cmd_entry_live)
    {
        if(osal_semp_pend(s_rcv_sync,cn_osal_timeout_forever))
        {
            err_int = 1;
            mid_int = 1;
            printf("[at_oc_mqtt_cmd_entry:]recv msg is %s !!\n", s_rcv_buffer);
            msg = cJSON_Parse(s_rcv_buffer);
            
            if(NULL != msg)
            {
                serviceId = cJSON_GetObjectItem(msg,"serviceId");
                if(NULL != serviceId)
                {
                    printf("serviceId:%s\n\r",serviceId->valuestring);
                }

                mid = cJSON_GetObjectItem(msg,"mid");
                if(NULL != mid)
                {
                    mid_int = mid->valueint;
                    printf("mid:%d\n\r",mid_int);
                }
                msgType = cJSON_GetObjectItem(msg,"msgType");
                if(NULL != msgType)
                {
                    printf("msgType:%s\n\r",msgType->valuestring);
                }
                cmd =  cJSON_GetObjectItem(msg,"cmd");
                if(NULL != cmd)
                {
                    printf("cmd:%s\n\r",cmd->valuestring);
                }

                paras = cJSON_GetObjectItem(msg,"paras");
                if(NULL != paras)
                {
                    ioswitch = cJSON_GetObjectItem(paras,"ioswitch");
                    if(NULL != ioswitch)
                    {
                        printf("ioswitch:%d\n\r",ioswitch->valueint);
                        err_int = en_oc_mqtt_err_code_ok;
                    }
                    else
                    {
                        printf("handle the json data as your specific profile\r\n");
                        err_int = en_oc_mqtt_err_code_err;
                    }
                }
                cJSON_Delete(msg);

                list.item.name = "body_para";
                list.item.buf = "body_para";
                list.item.type = en_key_value_type_string;
                list.next = NULL;

                response.hasmore = 0;
                response.errcode = err_int;
                response.mid = mid_int;
                response.bodylst = &list;

                msg = oc_mqtt_json_fmt_response(&response);
                if(NULL != msg)
                {
                    buf = cJSON_Print(msg);
                    if(NULL != buf)
                    {
                        if(0 == oc_mqtt_report(s_mqtt_handle,buf,strlen(buf),en_mqtt_al_qos_1))
                        {
                           // printf("SNDMSG:%s\n\r",buf);
                        }
                        osal_free(buf);
                    }
                    cJSON_Delete(msg);
                }
            }
        }
    }

    return 0;
}


static int at_oc_connect_invalid_server(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = NULL;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_connect_invalid_port(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = NULL;/*valid is 8883*/
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}


static int at_oc_connect_invalid_code_mode(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_binary;/*only json support now*/
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}


static int at_oc_connect_invalid_sign_type(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_yes;/*only support mqtt_sign_type_hmacsha256_check_time_no*/
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_connect_invalid_authtype(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_model;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_connect_invalid_ep(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = NULL;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_connect_invalid_passwd(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = NULL;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_connect_invalid_sectype(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_psk;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_connect_invalid_ca(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_psk;
    config.security.u.cas.ca_crt.data = NULL;
    config.security.u.cas.ca_crt.len = 0; ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}



static int at_oc_connect(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = DEFAULT_SERVER_IPV4;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = at_oc_app_msg_deal;
    config.code_mode = en_oc_mqtt_code_mode_json;
    config.sign_type = en_mqtt_sign_type_hmacsha256_check_time_no;
    config.device_type = en_oc_mqtt_device_type_static;
    config.auth_type = en_mqtt_auth_type_nodeid;
    config.device_info.s_device.deviceid = g_mqtt_ep_noteid;
    config.device_info.s_device.devicepasswd = g_mqtt_ep_passwd;

    config.security.type = en_mqtt_al_security_cas;
    config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
    config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'

    handle = oc_mqtt_config(&config);
    if(NULL == handle)
    {
        printf("config err \r\n");
        return -1;
    }
    else
    {
        printf("config success\r\n");
    }

    s_mqtt_handle = handle;

    return 0;
}

static int at_oc_disconnect()
{
    int ret = 0;
    ret = oc_mqtt_deconfig(s_mqtt_handle);
    if(0 == ret)
    {
        s_mqtt_handle = NULL;
        printf("deconfig success\r\n");

    }
    else
    {
        printf("deconfig failed\r\n");
    }
    return ret;
}

static int at_oc_report(int argc,const char *argv[])
{
    int ret = -1;
    tag_oc_mqtt_report  report;
    tag_key_value_list  lst;
    cJSON *root = NULL;
    char  *buf = NULL;
    static int leftpower = 0;

    leftpower = (leftpower + 7 )%100;

    lst.item.name = "batteryLevel";
    lst.item.buf = (char *)&leftpower;
    lst.item.len = sizeof(leftpower);
    lst.item.type = en_key_value_type_int;
    lst.next = NULL;

    report.hasmore = en_oc_mqtt_has_more_no;
    report.paralst= &lst;
    report.serviceid = "Battery";
    report.eventtime = NULL;

    root = oc_mqtt_json_fmt_report(&report);
    if(NULL != root)
    {
        buf = cJSON_Print(root);
        if(NULL != buf)
        {
            ret = oc_mqtt_report(s_mqtt_handle,buf,strlen(buf),en_mqtt_al_qos_1);
            osal_free(buf);
        }

        cJSON_Delete(root);
    }

    return ret;
}


static void *at_oc_config_test(tag_oc_mqtt_config *config)
{
    printf("this is at_oc_config\n");
    return NULL;
}

static int at_oc_deconfig_test(void *handle)
{
    printf("this is at_oc_deconfig\n");
    return 0;
}

static int at_oc_report_test(void *handle,char *msg, int msg_len,en_mqtt_al_qos_t qos)
{
    printf("this is at_oc_report_test\n");
    return 0;
}

int ts_oc_mqtt_init(char *message, int len)
{
    char *pchTmp, *pchStrTmpIn;
    
    printf("[%s] g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]ep name is %s\n", __FUNCTION__, pchTmp);
    if(g_mqtt_ep_noteid != NULL) 
    {
        osal_free(g_mqtt_ep_noteid);
        g_mqtt_ep_noteid = NULL;
    }
    g_mqtt_ep_noteid = osal_malloc(strlen(pchTmp)+1);
    memcpy(g_mqtt_ep_noteid, pchTmp, strlen(pchTmp));
    g_mqtt_ep_noteid[strlen(pchTmp)] = '\0';
    printf("120--g_mqtt_ep_noteid is %s\n",g_mqtt_ep_noteid);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(g_mqtt_ep_passwd != NULL) 
    {
        osal_free(g_mqtt_ep_passwd);
        g_mqtt_ep_passwd = NULL;
    }
    g_mqtt_ep_passwd = osal_malloc(strlen(pchTmp)+1);
    memcpy(g_mqtt_ep_passwd, pchTmp, strlen(pchTmp));
    g_mqtt_ep_passwd[strlen(pchTmp)] = '\0';

    printf("140--g_mqtt_ep_passwd is %s\n",g_mqtt_ep_passwd);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(g_mqtt_ep_deviceid != NULL) 
    {
        osal_free(g_mqtt_ep_deviceid);
        g_mqtt_ep_deviceid = NULL;
    }
    g_mqtt_ep_deviceid = osal_malloc(strlen(pchTmp)+1);
    memcpy(g_mqtt_ep_deviceid, pchTmp, strlen(pchTmp));
    g_mqtt_ep_deviceid[strlen(pchTmp)] = '\0';

    printf("160--g_mqtt_ep_deviceid is %s\n",g_mqtt_ep_deviceid);

    osal_semp_create(&s_rcv_sync,1,0);
    task_handle = osal_task_create("at_ocmqtt_cmd",at_oc_mqtt_cmd_entry,NULL,0x1000,NULL,10);
    return oc_mqtt_init();
}

static int ts_oc_mqtt_register(char *message, int len)
{
    int ret = 0;
    int retCode;  
    tag_oc_mqtt_ops s_oc_mqtt_ops=
    {
        .config = at_oc_config_test,
        .deconfig = at_oc_deconfig_test,
        .report = at_oc_report_test,
    };
    retCode = oc_mqtt_register(&s_oc_mqtt_ops);
    ret -= (!(retCode == 0));
    /*register again, will sucess*/
    retCode = oc_mqtt_register(&s_oc_mqtt_ops);
    ret -= (!(retCode == 0));

    retCode = oc_mqtt_register(NULL);
    ret -= (!(retCode == -1));
    
    return ret;
}

static int ts_oc_mqtt_config(char *message, int len)
{
    int ret = 0;
    int retCode;

    #include <atiny_mqtt.h>
    oc_mqtt_install_atiny_mqtt();

    osal_task_sleep(1000);
    
    at_oc_disconnect(0,NULL); ///< make sure we disconnected;
    

    retCode = at_oc_connect_invalid_server(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_server FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_port(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_port FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_code_mode(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_code_mode FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));
    
    retCode = at_oc_connect_invalid_sign_type(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_sign_type FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_authtype(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_authtype FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_ep(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_ep FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_passwd(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_passwd FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_sectype(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_sectype FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));

    retCode = at_oc_connect_invalid_ca(0,NULL);
    if(retCode != -1)
    {
        printf("at_oc_connect_invalid_ca FAILED ************\n\r");
    }
    ret -= (!(retCode == -1));
    
    printf("call at_oc_connect ************\n\r");    
    retCode = at_oc_connect(0,NULL);
    if(0 == retCode)
    {
        printf("EXIT TESTDEMO 1:PASS\n\r");
    }
    else
    {
        printf("EXIT TESTDEMO 1:FAILED ************\n\r");
    }
    
    ret -= (!(retCode == 0));

    printf("%s, call at_oc_connect,ret is %d,retcode is %d ************\n\r",__FUNCTION__, ret, retCode);

    retCode = at_oc_connect(0,NULL);

    ret -= (!(retCode == -1));

    printf("exit from %s, ret is %d, retCode is %d ************\n\r", __FUNCTION__, ret, retCode);
    
    return ret;
}

static int ts_oc_mqtt_json_fmt_req(char *message, int len)
{
    int ret = 0;
    int leftpower;
    tag_oc_mqtt_report  report;
    tag_key_value_list  lst1;
	tag_key_value_list  lst2;
	tag_key_value_list  lst3;
	tag_key_value_list  lst4;
    cJSON *root;
    char  *buf;
    
    leftpower = 1;
    lst1.item.name = "LED1";
    lst1.item.buf = (char *)&leftpower;
    lst1.item.len = sizeof(leftpower);
    lst1.item.type = en_key_value_type_int;
    lst1.next = &lst2;
		
	leftpower = 2;
    lst2.item.name = "LED2";
    lst2.item.buf = (char *)&leftpower;
    lst2.item.len = sizeof(leftpower);
    lst2.item.type = en_key_value_type_int;
    lst2.next = &lst3;
    
    leftpower = 35;	
    lst3.item.name = "LED3";
    lst3.item.buf = (char *)&leftpower;
    lst3.item.len = sizeof(leftpower);
    lst3.item.type = en_key_value_type_int;
    lst3.next = &lst4;

    leftpower = 78;
    lst4.item.name = "LED4";
    lst4.item.buf = (char *)&leftpower;
    lst4.item.len = sizeof(leftpower);
    lst4.item.type = en_key_value_type_int;
    lst4.next = NULL;
		
    report.hasmore = en_oc_mqtt_has_more_no;
    report.paralst= &lst1;
    report.serviceid = "LED";
    report.eventtime = NULL;
    root = oc_mqtt_json_fmt_report(&report);
    ret -= (!(root != NULL));
    buf = cJSON_Print(root);
    ret -= (!(buf != NULL));
    printf("req buf is %s\n", buf);

    
    
    return TS_OK;
}

static int ts_oc_mqtt_json_fmt_res(char *message, int len)
{
    int ret = 0;
    //int retCode;
    tag_oc_mqtt_response response;
    tag_key_value_list   list;

    cJSON  *root;
    char   *buf;
    
    list.item.name = "body_para";
    list.item.buf = "body_para";
    list.item.type = en_key_value_type_string;
    list.next = NULL;

    response.hasmore = 0;
    response.errcode = en_oc_mqtt_err_code_ok;
    response.mid = 1;
    response.bodylst = &list;
    
    root = oc_mqtt_json_fmt_response(&response);
    
    ret -= (!(root != NULL));
    buf = cJSON_Print(root);
    ret -= (!(buf != NULL));
    printf("res buf is %s\n", buf);
    
    return ret;
}



static int ts_oc_mqtt_report(char *message, int len)
{
    int ret = 0;
    int retCode = 0;

    if(!s_mqtt_handle)
        retCode = at_oc_connect(0,NULL);
    
    if(0 == retCode)
    {
        printf("CONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("CONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }
    ret -= (!(retCode == 0));
    
    retCode = at_oc_report(0,NULL);
    ret -= (!(retCode == 0));
    if(0 == retCode)
    {
        printf("REPORT:SUCCESS\n\r");
    }
    else
    {
        printf("REPORT:FAILED\n\r");
        goto DEMO_EXIT;
    }
    
    
    retCode = at_oc_disconnect(0,NULL);
    ret -= (!(retCode == 0));
    if(0 == retCode)
    {
        printf("DISCONNECT:SUCCESS\n\r");
    }
    else
    {
        printf("DISCONNECT:FAILED\n\r");
        goto DEMO_EXIT;
    }

    retCode = at_oc_report(0,NULL);
    ret -= (!(retCode == -1));
    
DEMO_EXIT:
    if(0 == ret)
    {
        printf("EXIT test_oc_mqtt_report_static: PASS\n\r");
    }
    else
    {
        printf("EXIT test_oc_mqtt_report_static: FAILED ************\n\r");
    }
    return ret;
}

static int ts_oc_mqtt_deconfig(char *message, int len)
{
    int ret = 0;
    int retCode = 0;
    
    if(!s_mqtt_handle)
        at_oc_connect(0,NULL);
    retCode = at_oc_disconnect(0,NULL);
    ret -= (!(retCode == 0));
    printf("in %s, retcode is %d\n", __FUNCTION__, retCode);

    retCode = at_oc_disconnect(0,NULL);
    ret -= (!(retCode == -1));

    printf("exit from %s, ret is %d, retcode is %d\n", __FUNCTION__, ret, retCode);

    return ret;
}
static int ts_oc_mqtt_deinit(char *message, int len)
{
    s_cmd_entry_live = 0;
    if(s_rcv_sync && task_handle) 
    {
        osal_semp_post(s_rcv_sync);
        osal_task_sleep(3000);
        osal_task_kill(task_handle);
        osal_semp_del(s_rcv_sync);
        printf("resource released!");
    }
    return TS_OK;
}



