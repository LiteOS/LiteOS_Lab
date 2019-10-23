
#include <stdio.h>
#include <stdlib.h>
#include <sal.h>
#include <osal.h>
#if CONFIG_OC_MQTT_ENABLE
#include <oc_mqtt_al.h>
#endif

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


char *g_mqtt_ep_noteid = NULL;
char *g_mqtt_ep_deviceid = NULL;
char *g_mqtt_ep_passwd = NULL;
char *g_mqtt_clientdata = NULL;



/*OC MQTT*/
#if CONFIG_OC_MQTT_ENABLE
static int ts_oc_mqtt_init(char *message, int len);
static int ts_oc_mqtt_connect(char *message, int len);
static int ts_oc_mqtt_subcmd(char *message, int len);
static int ts_oc_mqtt_report(char *message, int len);
static int ts_oc_mqtt_disconnect(char *message, int len);
static int ts_oc_mqtt_deinit(char *message, int len);
#endif
/*MQTT AL*/
static int ts_mqtt_al_init(char *message, int len);
static int ts_mqtt_al_install(char *message, int len);
static int ts_mqtt_al_uninstall(char *message, int len);
static int ts_mqtt_al_connect(char *message, int len);
static int ts_mqtt_al_disconnect(char *message, int len);
static int ts_mqtt_al_subscrible(char *message, int len);
static int ts_mqtt_al_unsubscrible(char *message, int len);
static int ts_mqtt_al_publish(char *message, int len);
static int ts_mqtt_al_checkstatus(char *message, int len);
static int ts_mqtt_al_deinit(char *message, int len);


#if CONFIG_OC_MQTT_ENABLE
test_entry ts_oc_mqtt_entry_flist[]= {
    ts_oc_mqtt_init,
    ts_oc_mqtt_connect,
    ts_oc_mqtt_subcmd,
    ts_oc_mqtt_report,
    ts_oc_mqtt_disconnect,
    ts_oc_mqtt_deinit,
};
#endif

test_entry ts_mqtt_al_entry_flist[]= {
    ts_mqtt_al_init,
    ts_mqtt_al_install,
    ts_mqtt_al_uninstall,
    ts_mqtt_al_connect,
    ts_mqtt_al_disconnect,
    ts_mqtt_al_subscrible,
    ts_mqtt_al_publish,
    ts_mqtt_al_checkstatus,
    ts_mqtt_al_deinit,
};


int ts_sort_oc_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id > 5)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }
#if CONFIG_OC_MQTT_ENABLE
    return (ts_oc_mqtt_entry_flist[entry_id])(message, len);
#else
    return TS_FAILED;
#endif
}


int ts_sort_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id > 9)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }
    return (ts_mqtt_al_entry_flist[entry_id])(message, len);
}

/*--------------oc mqtt test---------------------------*/
#if CONFIG_OC_MQTT_ENABLE

#define cn_app_rcv_buf_len 256
static char            s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;
static void           *s_mqtt_handle;
static int app_msg_deal(void *handle,mqtt_al_msgrcv_t *msg)
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
    printf("[%s]ep name is %s, len is %u\n", __FUNCTION__, pchTmp, strlen(pchTmp));
    if(g_mqtt_ep_noteid != NULL) 
    {
        osal_free(g_mqtt_ep_noteid);
        g_mqtt_ep_noteid = NULL;
    }
    g_mqtt_ep_noteid = osal_malloc(strlen(pchTmp)+1);
    memcpy(g_mqtt_ep_noteid, pchTmp, strlen(pchTmp));
    g_mqtt_ep_noteid[strlen(pchTmp)] = '\0';
    printf("120--g_mqtt_ep_noteid is %s,len is %u\n",g_mqtt_ep_noteid,strlen(pchTmp));
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(g_mqtt_ep_passwd != NULL) 
    {
        osal_free(g_mqtt_ep_passwd);
        g_mqtt_ep_passwd = NULL;
    }
    g_mqtt_ep_passwd = osal_malloc(strlen(pchTmp)+1);
    memcpy(g_mqtt_ep_passwd, pchTmp, strlen(pchTmp));
    g_mqtt_ep_passwd[strlen(pchTmp)] = '\0';

    printf("140--g_mqtt_ep_passwd is %s,len is %u\n",g_mqtt_ep_passwd,strlen(pchTmp));
    
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
    
    return oc_mqtt_init();
}



static int at_oc_connect_invalid_server(int argc,const char *argv[])
{
    void *handle = NULL;

    tag_oc_mqtt_config config;

    config.boot_mode = en_oc_boot_strap_mode_factory;
    config.lifetime = DEFAULT_LIFETIME;
    config.server = NULL;
    config.port = DEFAULT_SERVER_PORT;
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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
    config.msgdealer = app_msg_deal;
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



static int ts_oc_mqtt_connect(char *message, int len)
{
    int ret = 0;
    int retCode;

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

    printf("exit from %s, ret is %d, retCode is %d ************\n\r",__FUNCTION__, ret, retCode);
    
    return ret;
}



static int ts_oc_mqtt_subcmd(char *message, int len)
{
    return TS_OK;
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
    if(0 == retcode)
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

static int ts_oc_mqtt_disconnect(char *message, int len)
{
    int ret = 0;
    int retCode = 0;
    
    if(!s_mqtt_handle)
        at_oc_connect(0,NULL);
    retCode = at_oc_disconnect(0,NULL);
    ret -= (!(retCode == 0));
    printf("in %s, retcode is %d\n", __FUNCTION__, retCode);

    retCode = at_oc_disconnect(0,NULL);
    ret -= (!(retCode == 0));

    printf("exit from %s, ret is %d, retcode is %d\n", __FUNCTION__, ret, retCode);

    return ret;
}
static int ts_oc_mqtt_deinit()
{
    osal_semp_del(&s_rcv_sync);
    return TS_OK;
}
#endif
/*--------------mqtt_al test---------------------------*/
static int ts_mqtt_al_init(char *message, int len)
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
    
    return mqtt_init();
}

static int ts_mqtt_al_install(char *message, int len)
{
    int ret = 0;
    int retCode;
    
    mqtt_al_uninstall();
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    retCode = mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    retCode = mqtt_install_sinnmqtt();
#endif

    ret -= (!(retCode == 0));
    /*again install ,will failed*/
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    retCode = mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    retCode = mqtt_install_sinnmqtt();
#endif

    ret -= (!(retCode == -1));

    return ret;

}
static int ts_mqtt_al_uninstall(char *message, int len)
{
    int ret = 0;
    int retCode;
    
    mqtt_al_uninstall();
#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    mqtt_install_sinnmqtt();
#endif

    retCode = mqtt_al_uninstall();
    ret -= (!(retCode == 0));

    retCode = mqtt_al_uninstall();
    ret -= (!(retCode == -1));

#if CONFIG_MQTT_PAHO_ENABLE
    #include <paho_mqtt_port.h>
    mqtt_install_pahomqtt();
#elif CONFIG_MQTT_SINN_ENABLE
    #include <mqtt_sinn_port.h>
    mqtt_install_sinnmqtt();
#endif

    return ret;
    
}
static int at_mqtt_al_connect(char *clientdata, char *ep, char *passwd, char *ca, en_mqtt_al_security_t type, char *serverip)
{
    mqtt_al_conpara_t conpara;
    mqtt_al_security_para_t security;

    memset(&conpara,0,sizeof(conpara));
    
    conpara.clientid.data = clientdata;
    conpara.clientid.len = strlen(conpara.clientid.data);

    conpara.user.data =  ep;
    conpara.user.len = strlen(conpara.user.data);

    conpara.passwd.data = passwd;
    conpara.passwd.len = strlen(conpara.passwd.data);

    conpara.cleansession = 1;
    conpara.keepalivetime = 10;
    security.type = type;
    security.u.cas.ca_crt.data = ca;
    security.u.cas.ca_crt.len = (strlen(ca)+1); ///< must including the end '\0'
    conpara.security = &security;

    conpara.serveraddr.data = serverip;
    conpara.serveraddr.len = strlen(conpara.serveraddr.data);
    conpara.serverport = atoi(DEFAULT_SERVER_PORT);
    conpara.timeout = 10000;
    conpara.version = en_mqtt_al_version_3_1_1;
    conpara.willmsg = NULL;

    printf("SERVER:%s \r\n PORT:%d \r\n",\
            conpara.serveraddr.data,conpara.serverport);
    printf("CLIENTID:%s \r\n USER:%s \r\n PASSWD:%s \r\n",\
            conpara.clientid.data,conpara.user.data,conpara.passwd.data);

    g_mqtt_al_handle = mqtt_al_connect(&conpara);
    return conpara.conret;
    
}


static int ts_mqtt_al_connect(char *message, int len)
{
    int ret = 0;
    int retCode;
    int tlen;
    en_mqtt_al_connect_state state;
    
    tlen = strlen(g_mqtt_ep_noteid) +
        strlen(cn_check_time_value) + strlen(CN_OC_MQTT_CLIENTID_STATIC_FMT) + 1;
    
    if(g_mqtt_clientdata)
        osal_free(g_mqtt_clientdata);
    g_mqtt_clientdata = osal_malloc(tlen);
    if(g_mqtt_clientdata == NULL)
    {
        return TS_FAILED;
    }
    
    snprintf(g_mqtt_clientdata, tlen, CN_OC_MQTT_CLIENTID_STATIC_FMT, g_mqtt_ep_noteid,\
        2/*en_mqtt_auth_type_nodeid*/, 0/*en_mqtt_sign_type_hmacsha256_check_time_no*/, cn_check_time_value);
    retCode = at_mqtt_al_connect(g_mqtt_clientdata, g_mqtt_ep_noteid, g_mqtt_ep_passwd, 
        s_mqtt_ca_crt, en_mqtt_al_security_cas, DEFAULT_SERVER_IPV4);
    ret -= (!(retCode == cn_mqtt_al_con_code_ok));

    
    state = mqtt_al_check_status(g_mqtt_al_handle);

    ret -= (!(state == en_mqtt_al_connect_ok));
    
    return ret;
    
}

static int ts_mqtt_al_disconnect(char *message, int len)
{
    int ret = 0;
    int retCode;
    retCode = mqtt_al_disconnect(g_mqtt_al_handle);
    if(retCode == 0)
    {
        g_mqtt_al_handle = NULL;
    }
    ret -= (!(retCode==0));

    /*disconnect again*/
    retCode = mqtt_al_disconnect(g_mqtt_al_handle);
    if(retCode == 0)
    {
        g_mqtt_al_handle = NULL;
    }
    ret -= (!(retCode==-1));
    
    return ret;
}

static int ts_mqtt_al_subscrible(char *message, int len)
{
    return TS_OK;
}

static int ts_mqtt_al_unsubscrible(char *message, int len)
{
    return TS_OK;
}

static int ts_mqtt_al_publish(char *message, int len)
{
    return TS_OK;
}

static int ts_mqtt_al_checkstatus(char *message, int len)
{
    int ret = 0;
    en_mqtt_al_connect_state state;
    
    state = mqtt_al_check_status(g_mqtt_al_handle);

    ret -= (!(state != en_mqtt_al_connect_ok));

    
    
    return TS_OK;
}


static int ts_mqtt_al_deinit(char *message, int len)
{

    if(g_mqtt_ep_noteid)
        osal_free(g_mqtt_ep_noteid);
    if(g_mqtt_ep_passwd)
        osal_free(g_mqtt_ep_passwd);

    if(g_mqtt_ep_deviceid)
        osal_free(g_mqtt_ep_deviceid);
    
    if(g_mqtt_clientdata)
        osal_free(g_mqtt_clientdata);

    return TS_OK;
}






