
#include <stdio.h>
#include <stdlib.h>
#include <sal.h>
#include <osal.h>

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

typedef struct mqtt_al_paras_s{
    char *client;
    char *passwd;
    char *clientdata;
    char *server_ip4;
    char *server_port;
    char *cbname;
    unsigned short life_time;
    unsigned short version;
    unsigned short sec_valid;
    /*below maybe not used*/
    char *will_topic;
    char *will_msg;
    unsigned short will_qos_v;
    int will_retain;  
}mqtt_al_paras;

static mqtt_al_paras  g_mqtt_al_paras = {0};

char *g_mqtt_ep_noteid = NULL;
char *g_mqtt_ep_deviceid = NULL;
char *g_mqtt_ep_passwd = NULL;
char *g_mqtt_clientdata = NULL;



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

int ts_sort_mqtt_al(int entry_id, char *message, int len)
{
    if(entry_id > 9)
    {
        printf("invalid entry_id(%d)\n",entry_id);
        return TS_FAILED;
    }
    return (ts_mqtt_al_entry_flist[entry_id])(message, len);
}

/*--------------mqtt_al test---------------------------*/
static int ts_mqtt_al_init(char *message, int len)
{
    char *pchTmp, *pchStrTmpIn;
    mqtt_al_paras *pparas = &g_mqtt_al_paras;
    
    printf("[%s] g_tempbuf is %s\n", __FUNCTION__, message);
    pchTmp      = message;
    pchStrTmpIn = NULL;
    
    pchTmp = strtok_r(pchTmp, "|", &pchStrTmpIn);
    printf("[%s]id is %d\n",__FUNCTION__,atoi(pchTmp));
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]call name is %s\n",__FUNCTION__,pchTmp);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    printf("[%s]ep name is %s\n", __FUNCTION__, pchTmp);
    if(pparas->client != NULL) 
    {
        osal_free(pparas->client);
    }
    pparas->client = osal_malloc(strlen(pchTmp)+1);
    memcpy(pparas->client, pchTmp, strlen(pchTmp));
    pparas->client[strlen(pchTmp)] = '\0';
    printf("120--g_mqtt_ep_noteid is %s\n",pparas->client);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->passwd != NULL) 
    {
        osal_free(pparas->passwd);
    }
    pparas->passwd = osal_malloc(strlen(pchTmp)+1);
    memcpy(pparas->passwd, pchTmp, strlen(pchTmp));
    pparas->passwd[strlen(pchTmp)] = '\0';

    printf("140--g_mqtt_ep_passwd is %s\n",pparas->passwd);
    
    pchTmp = strtok_r(NULL, "|", &pchStrTmpIn);
    if(pparas->clientdata != NULL)
    {
        osal_free(pparas->clientdata);
    }
    if((!memcmp(pchTmp, "NULL", strlen(pchTmp))) ||
        (!memcmp(pchTmp, "null", strlen(pchTmp))))
    {
        pparas->clientdata = NULL;
    }
    else
    {
        pparas->clientdata = osal_malloc(strlen(pchTmp)+1);
        memcpy(pparas->clientdata, pchTmp, strlen(pchTmp));
        pparas->clientdata[strlen(pchTmp)] = '\0';
    }
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

static void at_dmp_msg_dealer(void *arg,mqtt_al_msgrcv_t  *msg)
{
    cJSON  *root = NULL;
    cJSON  *cmd_item = NULL;
    cJSON  *serviceid_item = NULL;
    char   *json_buf;
    int     reboot_servceid_match = 0;
    int     reboot_cmd_match = 0;
    int     mqtt_rebootstrap = 0;

    //for we must add the '/0' to the end to make sure the json parse correct
    if ((msg == NULL) || ( msg->msg.data == NULL))
    {
        return;
    }

    json_buf = osal_malloc(msg->msg.len + 1);
    if(NULL != json_buf)
    {
        memcpy(json_buf,msg->msg.data,msg->msg.len);
        json_buf[msg->msg.len] = '\0';
        root = cJSON_Parse(json_buf);
        if(NULL != root)
        {
            cmd_item = cJSON_GetObjectItem(root,"cmd");
            if((NULL != cmd_item) && (strncmp(cmd_item->valuestring, "BootstrapRequestTrigger", strlen(cmd_item->valuestring)) == 0))
            {
                reboot_cmd_match =1;
            }
            serviceid_item = cJSON_GetObjectItem(root,"serviceid");
            if((NULL != serviceid_item) && (strncmp(serviceid_item->valuestring, "IOTHUB.BS", strlen(serviceid_item->valuestring)) == 0))
            {
                reboot_servceid_match = 1;
            }
            if(reboot_cmd_match && reboot_servceid_match)
            {
                mqtt_rebootstrap = 1;
            }

            cJSON_Delete(root);
        }
        osal_free(json_buf);
    }

    if(mqtt_rebootstrap) ///< we reboot it now
    {
        printf("THIS IS THE OC MQTT REBOOTSTRAP COMMAND\n\r");
        s_oc_agent_mqtt_cb->b_flag_dmp_run = 0;
        return;
    }

    s_oc_agent_mqtt_cb->config.msgdealer(s_oc_agent_mqtt_cb,msg);

    return;
}


static int ts_mqtt_al_subscrible(char *message, int len)
{
    /*建立连接*/
    ts_mqtt_al_connect();
    int ret = -1;
    mqtt_al_subpara_t   subpara;

    memset(&subpara,0,sizeof(subpara));

    subpara.dealer = cb->msg_dealer;
    subpara.arg = cb;
    subpara.qos = en_mqtt_al_qos_1;
    subpara.topic.data = cb->mqtt_subtopic ;
    subpara.topic.len = strlen(cb->mqtt_subtopic);

    ret = mqtt_al_subscribe(cb->mqtt_handle,&subpara);
    
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






