
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

static int g_ioswitch = -1;

typedef struct oc_mqtt_paras_s{
    char *id;
    char *passwd;
    char *server_ip4;
    char *server_port;
    char *cbname;
    en_oc_boot_strap_mode_t  boot_mode;
    unsigned short life_time; 
    unsigned short code_mode;
    unsigned short sign_type;
    unsigned short device_type;
    unsigned short auth_type; 
    unsigned short sec_type;
    unsigned short ca_is_valid;
}oc_mqtt_paras;

oc_mqtt_paras g_mqtt_paras = {0};

extern char g_acRecvBuf[TS_RECV_BUFFER_LEN+128];
/*OC MQTT*/
static int ts_oc_mqtt_init(char *message, int len);
static int ts_oc_mqtt_register(char *message, int len);
static int ts_oc_mqtt_config(char *message, int len);
static int ts_oc_mqtt_json_fmt_req(char *message, int len);
static int ts_oc_mqtt_json_fmt_res(char *message, int len);
static int ts_oc_mqtt_report(char *message, int len);
static int ts_oc_mqtt_deconfig(char *message, int len);
static int ts_oc_mqtt_deinit(char *message, int len);
static int ts_oc_mqtt_getvalue(char *message, int len);


test_entry ts_oc_mqtt_entry_flist[]= {
    ts_oc_mqtt_init,
    ts_oc_mqtt_register,
    ts_oc_mqtt_config,
    ts_oc_mqtt_json_fmt_req,
    ts_oc_mqtt_json_fmt_res,
    ts_oc_mqtt_report,
    ts_oc_mqtt_deconfig,
    ts_oc_mqtt_deinit,
    ts_oc_mqtt_getvalue,
};

int ts_sort_oc_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id >= TEST_OC_MQTT_INVALID)
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
static int pp_oc_app_msg_deal(void *handle,mqtt_al_msgrcv_t *msg)
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

static int pp_oc_mqtt_cmd_entry(void *args)
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

    printf("pp_oc_mqtt_cmd_entry start now !!\n");
    while(s_cmd_entry_live)
    {
        if(osal_semp_pend(s_rcv_sync,cn_osal_timeout_forever))
        {
            err_int = 1;
            mid_int = 1;
            printf("[pp_oc_mqtt_cmd_entry:]recv msg is %s !!\n", s_rcv_buffer);
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
                        g_ioswitch = ioswitch->valueint;
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

static int pp_oc_report(void)
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


static void *pp_oc_config_test(tag_oc_mqtt_config *config)
{
    printf("this is at_oc_config\n");
    return NULL;
}

static int pp_oc_deconfig_test(void *handle)
{
    printf("this is at_oc_deconfig\n");
    return 0;
}

static int pp_oc_report_test(void *handle,char *msg, int msg_len,en_mqtt_al_qos_t qos)
{
    printf("this is at_oc_report_test\n");
    return 0;
}

int ts_oc_mqtt_init(char *message, int len)
{
    osal_semp_create(&s_rcv_sync,1,0);
    task_handle = osal_task_create("at_ocmqtt_cmd",pp_oc_mqtt_cmd_entry,NULL,0x1000,NULL,10);
    return oc_mqtt_init();
}

/*ts_oc_mqtt_config must be before ts_oc_mqtt_config*/
static int ts_oc_mqtt_register(char *message, int len)
{
    int ret = 0;
    int retCode;  
    tag_oc_mqtt_ops s_oc_mqtt_ops=
    {
        .config = pp_oc_config_test,
        .deconfig = pp_oc_deconfig_test,
        .report = pp_oc_report_test,
    };
    retCode = oc_mqtt_register(&s_oc_mqtt_ops);
    ret -= (!(retCode == 0));
    /*register again, will sucess*/
    retCode = oc_mqtt_register(&s_oc_mqtt_ops);
    ret -= (!(retCode == 0));

    retCode = oc_mqtt_register(NULL);
    ret -= (!(retCode == -1));

    //install atiny mqtt
    #include <atiny_mqtt.h>
    oc_mqtt_install_atiny_mqtt();

    osal_task_sleep(500);
    
    

    printf("exit from %s\n", __FUNCTION__);
    
    return ret;
}

static int ts_oc_mqtt_config(char *message, int len)
{

    tag_oc_mqtt_config config;

    char *pchTmp, *pchStrTmpIn;
    oc_mqtt_paras *pparas = &g_mqtt_paras;
    void *handle = NULL;

    
    printf("[%s] g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]ep name is %s\n", __FUNCTION__, pchTmp);
    if(pparas->id != NULL) 
    {
        osal_free(pparas->id);
    }
    pparas->id = osal_malloc(strlen(pchTmp)+1);
    memcpy(pparas->id, pchTmp, strlen(pchTmp));
    pparas->id[strlen(pchTmp)] = '\0';
    printf("120--id is %s\n",pparas->id);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
    }
    pparas->passwd = osal_malloc(strlen(pchTmp)+1);
    memcpy(pparas->passwd, pchTmp, strlen(pchTmp));
    pparas->passwd[strlen(pchTmp)] = '\0';

    printf("140--passwd is %s\n",pparas->passwd);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("168--oc server ipv4 is %s\n",pchTmp);
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_ip4 = NULL;
    }
    else
    {
        pparas->server_ip4 = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_ip4, pchTmp, strlen(pchTmp));
        pparas->server_ip4[strlen(pchTmp)] = '\0';
    }
    
    

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("168--oc server port is %s\n",pchTmp);
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->server_port = NULL;
    }
    else
    {
        pparas->server_port = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->server_port, pchTmp, strlen(pchTmp));
        pparas->server_port[strlen(pchTmp)] = '\0';
    }
    

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("168--callback name is %s\n",pchTmp);
    if(pparas->cbname != NULL) 
    {
        osal_free(pparas->cbname);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->cbname = NULL;
    }
    else
    {
        pparas->cbname = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->cbname, pchTmp, strlen(pchTmp));
        pparas->cbname[strlen(pchTmp)] = '\0';
    }

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->boot_mode = atoi(pchTmp);
    printf("165--boot_mode is %d\n",pparas->boot_mode);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->life_time = atoi(pchTmp);
    printf("165--life_time is %d\n",pparas->life_time);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->code_mode = atoi(pchTmp);
    printf("165--code_mode is %d\n",pparas->code_mode);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->sign_type = atoi(pchTmp);
    printf("165--sign_type is %d\n",pparas->sign_type);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->device_type = atoi(pchTmp);
    printf("165--device_type is %d\n",pparas->device_type);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->auth_type = atoi(pchTmp);
    printf("165--auth_type is %d\n",pparas->auth_type);

    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->sec_type = atoi(pchTmp);
    printf("165--sec_type is %d\n",pparas->sec_type);


    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    pparas->ca_is_valid = atoi(pchTmp);
    printf("165--ca_is_valid is %d\n",pparas->ca_is_valid);

    config.boot_mode = pparas->boot_mode;
    config.lifetime = pparas->life_time;
    config.server = pparas->server_ip4;
    config.port = pparas->server_port;
    config.msgdealer = (pparas->cbname ? pp_oc_app_msg_deal : NULL);
    if(pparas->cbname)printf("[ts_oc_mqtt_config]pparas->cbname is %s\n",pparas->cbname);
    printf("[ts_oc_mqtt_config]config.msgdealer is %p\n", config.msgdealer);
    config.code_mode = pparas->code_mode;
    config.sign_type = pparas->sign_type;
    config.device_type = pparas->device_type;
    config.auth_type = pparas->auth_type;
    config.device_info.s_device.deviceid = pparas->id;
    config.device_info.s_device.devicepasswd = pparas->passwd;

    config.security.type = pparas->sec_type;
    if(pparas->ca_is_valid){
        config.security.u.cas.ca_crt.data = s_mqtt_ca_crt;
        config.security.u.cas.ca_crt.len = sizeof(s_mqtt_ca_crt); ///< must including the end '\0'
    }
    else
    {
        config.security.u.cas.ca_crt.data = NULL;
        config.security.u.cas.ca_crt.len = 0; ///< must including the end '\0'
    }

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

static int ts_oc_mqtt_json_fmt_req(char *message, int len)
{
    int ret = 0;
    tag_oc_mqtt_report  report;
    tag_key_value_list  lst1;
	tag_key_value_list  lst2;
	tag_key_value_list  lst3;
	tag_key_value_list  lst4;
    cJSON *root;
    char  *buf;
    char *pchTmp, *pchStrTmpIn = NULL;

    printf("[ts_oc_mqtt_json_fmt_req]recve msg: %s,%d\n",message,len);
    
    pchTmp = message;

    /*id*/
    pchTmp      = strtok_r(pchTmp, "|", &pchStrTmpIn);
    /*func name*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    /*service id*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *serviceId = osal_malloc(strlen(pchTmp)+1);
    memcpy(serviceId, pchTmp, strlen(pchTmp));
    serviceId[strlen(pchTmp)] = 0;
    printf("serviceId is %s\n", serviceId);
    /*hasMore*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int hasMore = atoi(pchTmp);
    printf("hasMore is %d\n", hasMore);
    
    /*item1**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item1 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item1, pchTmp, strlen(pchTmp));
    item1[strlen(pchTmp)] = 0;
    printf("item1 is %s\n", item1);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype1 = atoi(pchTmp);
    printf("valuetype1 is %d\n", valuetype1);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue1 = NULL;
    int value1;
    lst1.item.name = item1;//"LED1";
    lst1.item.type = valuetype1; //en_key_value_type_int;
    
    if(valuetype1 == en_key_value_type_int)
    {
        value1 = atoi(pchTmp);
        lst1.item.buf = (char *)&value1;
        lst1.item.len = sizeof(int);
    }
    else if(valuetype1 == en_key_value_type_string)
    {
        svalue1 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue1, pchTmp, strlen(pchTmp));
        svalue1[strlen(pchTmp)] = 0;
        lst1.item.buf = svalue1;
        lst1.item.len = strlen(pchTmp);
    }
    else if(valuetype1 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst1.next = &lst2;

    /*item2**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item2 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item2, pchTmp, strlen(pchTmp));
    item2[strlen(pchTmp)] = 0;
    printf("item2 is %s\n", item2);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype2 = atoi(pchTmp);
    printf("valuetype2 is %d\n", valuetype2);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue2 = NULL;
    int value2;
    lst2.item.name = item2;//"LED1";
    lst2.item.type = valuetype2; //en_key_value_type_int;
    
    if(valuetype2 == en_key_value_type_int)
    {
        value2 = atoi(pchTmp);
        lst2.item.buf = (char *)&value2;
        lst2.item.len = sizeof(int);
    }
    else if(valuetype2 == en_key_value_type_string)
    {
        svalue2 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue2, pchTmp, strlen(pchTmp));
        svalue2[strlen(pchTmp)] = 0;
        lst2.item.buf = svalue2;
        lst2.item.len = strlen(pchTmp);
    }
    else if(valuetype2 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst2.next = &lst3;

    /*item3**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item3 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item3, pchTmp, strlen(pchTmp));
    item3[strlen(pchTmp)] = 0;
    printf("item3 is %s\n", item3);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype3 = atoi(pchTmp);
    printf("valuetype3 is %d\n", valuetype3);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue3 = NULL;
    int value3;
    lst3.item.name = item3;//"LED1";
    lst3.item.type = valuetype3; //en_key_value_type_int;
    
    if(valuetype3 == en_key_value_type_int)
    {
        value3 = atoi(pchTmp);
        lst3.item.buf = (char *)&value3;
        lst3.item.len = sizeof(int);
    }
    else if(valuetype3 == en_key_value_type_string)
    {
        svalue3 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue3, pchTmp, strlen(pchTmp));
        svalue3[strlen(pchTmp)] = 0;
        lst3.item.buf = svalue3;
        lst3.item.len = strlen(pchTmp);
    }
    else if(valuetype3 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst3.next = &lst4;

    /*item4**/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item4 = osal_malloc(strlen(pchTmp)+1);
    memcpy(item4, pchTmp, strlen(pchTmp));
    item4[strlen(pchTmp)] = 0;
    printf("item4 is %s\n", item4);
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype4 = atoi(pchTmp);
    printf("valuetype4 is %d\n", valuetype1);

    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *svalue4 = NULL;
    int value4;
    lst4.item.name = item4;//"LED1";
    lst4.item.type = valuetype4; //en_key_value_type_int;
    
    if(valuetype4 == en_key_value_type_int)
    {
        value4 = atoi(pchTmp);
        lst4.item.buf = (char *)&value4;
        lst4.item.len = sizeof(int);
    }
    else if(valuetype4 == en_key_value_type_string)
    {
        svalue4 = osal_malloc(strlen(pchTmp)+1);
        memcpy(svalue4, pchTmp, strlen(pchTmp));
        svalue4[strlen(pchTmp)] = 0;
        lst4.item.buf = svalue4;
        lst4.item.len = strlen(pchTmp);
    }
    else if(valuetype4 == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    lst4.next = NULL;


    report.hasmore = (en_oc_mqtt_has_more)hasMore;//en_oc_mqtt_has_more_no;
    report.paralst= &lst1;
    report.serviceid = serviceId;
    report.eventtime = NULL;
    root = oc_mqtt_json_fmt_report(&report);
    ret -= (!(root != NULL));
    if(root != NULL)
    {
        buf = cJSON_Print(root);
        ret -= (!(buf != NULL));

        cJSON_Delete(root);
        if(buf != NULL)
        {
            
            if(strlen(buf) < TS_RECV_BUFFER_LEN)
            {
                memcpy(message,buf,strlen(buf));
                message[strlen(buf)] = '\0';
            }
            printf("req buf(len %d) is %s\n", (int)strlen(message),message);
            osal_free(buf);
        }
        
    }

    if(serviceId != NULL)osal_free(serviceId);
    if(svalue1 != NULL)osal_free(svalue1);
    if(svalue2 != NULL)osal_free(svalue2);
    if(svalue3 != NULL)osal_free(svalue3);
    if(svalue4 != NULL)osal_free(svalue4);
    
    if(ret == 0)
        return TS_OK_HAS_DATA;
    else
        return ret;
}

static int ts_oc_mqtt_json_fmt_res(char *message, int len)
{
    int ret = 0;
    //int retCode;
    tag_oc_mqtt_response response;
    tag_key_value_list   list;

    cJSON  *root;
    char   *buf;
    
    char *pchTmp, *pchStrTmpIn = NULL;

    printf("[ts_oc_mqtt_json_fmt_req]recve msg: %s,%d\n",message,len);
    
    pchTmp = message;
    /*id*/
    pchTmp      = strtok_r(pchTmp, "|", &pchStrTmpIn);
    /*func name*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    /*hasmore*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    en_oc_mqtt_has_more hasMore = (en_oc_mqtt_has_more)atoi(pchTmp);

    /*errorcode*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    en_oc_mqtt_err_code errCode = (en_oc_mqtt_err_code)atoi(pchTmp);
    /*mid*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int mid = (int)atoi(pchTmp);
    /*item*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    char *item = osal_malloc(strlen(pchTmp)+1);
    memcpy(item, pchTmp, strlen(pchTmp));
    item[strlen(pchTmp)] = 0;
    printf("item is %s\n", item);
    /*value type*/
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int valuetype = atoi(pchTmp);
    printf("valuetype is %d\n", valuetype);
    
    list.item.name = item;
    list.item.type = valuetype;
    
    pchTmp      = strtok_r(NULL, "|", &pchStrTmpIn);
    int value;
    char *savlue = NULL;
    
    if(valuetype == en_key_value_type_int)
    {
        value = atoi(pchTmp);
        list.item.buf = (char *)&value;
        list.item.len = sizeof(int);
    }
    else if(valuetype == en_key_value_type_string)
    {
        savlue = osal_malloc(strlen(pchTmp)+1);
        memcpy(savlue, pchTmp, strlen(pchTmp));
        savlue[strlen(pchTmp)] = 0;
        list.item.buf = savlue;
        list.item.len = strlen(pchTmp);
    }
    else if(valuetype == en_key_value_type_array)/* not test now*/
    {
        printf("not test now !!\n");
    }
    else
    {
        printf("unsupport now !!\n");
    }
    list.next = NULL;
    /*
    typedef enum
    {
        en_oc_mqtt_has_more_no = 0,
        en_oc_mqtt_has_more_yes =1,
    }en_oc_mqtt_has_more;
    typedef enum
{
    en_oc_mqtt_err_code_ok = 0,
    en_oc_mqtt_err_code_err =1,
}en_oc_mqtt_err_code;
    typedef enum
    {
        en_key_value_type_int = 0,
        en_key_value_type_string,
        en_key_value_type_array,
    }en_value_type;

    */

    response.hasmore = hasMore;
    response.errcode = errCode;
    response.mid = mid;//1;
    response.bodylst = &list;
    
    root = oc_mqtt_json_fmt_response(&response);
    
    ret -= (!(root != NULL));
    if(root != NULL)
    {
        buf = cJSON_Print(root);
        ret -= (!(buf != NULL));

        cJSON_Delete(root);
        if(buf != NULL)
        {
            
            if(strlen(buf) < TS_RECV_BUFFER_LEN)
            {
                memcpy(message,buf,strlen(buf));
                message[strlen(buf)] = '\0';
            }
            printf("res buf(len %d) is %s\n", (int)strlen(message),message);
            osal_free(buf);
        }
        
    }
    if(ret == 0)
        return TS_OK_HAS_DATA;
    else
        return ret;
}



static int ts_oc_mqtt_report(char *message, int len)
{
    int ret = 0;
    int retCode = 0;
    
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
    
    retCode = pp_oc_report();
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
    
    
    retCode = ts_oc_mqtt_deconfig(NULL, 0);
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

    retCode = pp_oc_report();
    ret -= (!(retCode == -1));
    
    retCode = ts_oc_mqtt_config(NULL,0);
    ret -= (!(retCode == 0));
    
    
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
    oc_mqtt_paras *pparas = &g_mqtt_paras;
    
    retCode = oc_mqtt_deconfig(s_mqtt_handle);
    ret -= (!(retCode == 0));
    
    if(0 == retCode)
    {
        s_mqtt_handle = NULL;
        printf("deconfig success\r\n");

    }
    else
    {
        printf("deconfig failed\r\n");
    }

    retCode = oc_mqtt_deconfig(s_mqtt_handle);
    ret -= (!(retCode == -1));
    
    printf("exit from %s, ret is %d, retcode is %d\n", __FUNCTION__, ret, retCode);
    if(pparas->id != NULL) 
    {
        osal_free(pparas->id);
        pparas->id = NULL;
    }
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
        pparas->passwd = NULL;
    }
    
    if(pparas->server_ip4 != NULL) 
    {
        osal_free(pparas->server_ip4);
        pparas->server_ip4 = NULL;
    }
    if(pparas->server_port != NULL) 
    {
        osal_free(pparas->server_port);
        pparas->server_port = NULL;
    }
    if(pparas->cbname != NULL) 
    {
        osal_free(pparas->cbname);
        pparas->cbname = NULL;
    }
    return ret;
}
static int ts_oc_mqtt_deinit(char *message, int len)
{
    s_cmd_entry_live = 0;
    
    osal_task_sleep(500);
    if(s_rcv_sync && task_handle) 
    {
        osal_semp_post(s_rcv_sync);
        osal_task_sleep(3000);
        osal_task_kill(task_handle);
        osal_semp_del(s_rcv_sync);
        
    }

    
    
    printf("resource released!");
    return TS_OK;
}

static int ts_oc_mqtt_getvalue(char *message, int len)
{
    printf("[ts_oc_mqtt_getvalue]g_ioswitch is %d \n", g_ioswitch);
    memset(message, 0, 32);/*32 is more large than one int32*/
    snprintf(message,len,"%d",g_ioswitch);
    
    return TS_OK_HAS_DATA;
}



