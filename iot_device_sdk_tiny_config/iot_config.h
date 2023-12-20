#ifndef IOT_CONFIG_H
#define IOT_CONFIG_H
//enable LinkLog 
#define CONFIG_LINKLOG_ENABLE 1

// enable MQTT link_main
 #define CONFIG_MQTT_AL_ENABLE 1 

 //paho_mqtt_port.c use them
 #define CONFIG_PAHO_CONNECT_TIMEOUT 10000
 #define CONFIG_PAHO_CMD_TIMEOUT 10000
 #define CONFIG_PAHO_LOOPTIMEOUT 1000
 #define CONFIG_PAHO_SNDBUF_SIZE 2048
 #define CONFIG_PAHO_RCVBUF_SIZE 2048

//enable OC_MQTT link_main 
#define CONFIG_OCMQTT_ENABLE 1

#define CONFIG_QUEUE_TIMEOUT (5 * 1000)

#define CONFIG_IOT_LINK_CONFIGFILE "iot_config.h"

// enable backoff reconnect on the bussiness side
//#define CONFIG_BACKOFF_RECNONNECT 1

//enable mqtts
#define CONFIG_DTLS_AL_ENABLE 1
#define CONFIG_OC_MQTTV5_DEMO_TLS 1

#endif