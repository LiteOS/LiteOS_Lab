##############################CONFIGURE INTRODUCTION############################
#configure type      configure value
#------------------:------------------
#CONFIG_OS_TYPE       : linux macos liteos
#CONFIG_SHELL_ENABLE   : yes no
#CONFIG_LIBC_ENABLE   : yes no
#CONFIG_CJSON_ENABLE  : yes no
#CONFIG_TCPIP_TYPE    : lwip linux_socket macos_socket at_esp8266 none
#CONFIG_DTLS_TYPE     : mbedtls none
#CONFIG_EMBEDTLS_MODE : crt psk none
#CONFIG_MQTT_TYPE     : paho none
#CONFIG_LWM2M_TYPE    : wakaama none
#CONFIG_OC_MQTT_TYPE  : soft none
#CONFIG_OC_LWM2M_TYPE : soft bc35 none
#CONFIG_LOADER_ENABLE : yes no
#CONFIG_OC_LWM2M_DEMO_TYPE:none  oc_lwm2m_demo_dtls and oc_lwm2m_demo_nodtls oc_lwm2m_demo_bs_dtls
#CONFIG_OC_MQTT_DEMO_TYPE:none oc_mqtt_demo_static oc_mqtt_demo_bs

######################LOADER     CONFIURE TEMPLATE###############################
#CONFIG_OS_TYPE       := liteos
#CONFIG_SHELL_ENABLE   := yes
#CONFIG_LIBC_ENABLE   := yes
#CONFIG_CJSON_ENABLE  := no
#CONFIG_TCPIP_TYPE    := none
#CONFIG_DTLS_TYPE     := none
#CONFIG_EMBEDTLS_MODE := none
#CONFIG_MQTT_TYPE     := none
#CONFIG_LWM2M_TYPE    := none
#CONFIG_OC_MQTT_TYPE  := none
#CONFIG_OC_LWM2M_TYPE := none
#CONFIG_LOADER_ENABLE := yes
#CONFIG_OC_LWM2M_DEMO_TYPE:= none
#CONFIG_OC_MQTT_DEMO_TYPE:= none
#################################################################################
 
#####################  OC MQTT CONFIURE TEMPLATE################################
#CONFIG_OS_TYPE       := liteos
#CONFIG_SHELL_ENABLE   := yes
#CONFIG_DRIVER_ENABLE := no
#CONFIG_AT_ENABLE     := no
#CONFIG_LIBC_ENABLE   := yes
#CONFIG_CJSON_ENABLE  := yes
#CONFIG_TCPIP_TYPE    := lwip
#CONFIG_DTLS_TYPE     := mbedtls
#CONFIG_EMBEDTLS_MODE := crt
#CONFIG_MQTT_TYPE     := paho
#CONFIG_LWM2M_TYPE    := none
#CONFIG_OC_MQTT_TYPE  := soft
#CONFIG_OC_LWM2M_TYPE := none
#CONFIG_LOADER_ENABLE := no
#CONFIG_OC_LWM2M_DEMO_TYPE:= oc_lwm2m_demo_dtls
#CONFIG_OC_MQTT_DEMO_TYPE:= oc_mqtt_demo_static

################################################################################

######################OC LWM2M CONFIURE TEMPLATE#################################
CONFIG_OS_TYPE       := liteos
CONFIG_SHELL_ENABLE   := yes
CONFIG_DRIVER_ENABLE := yes
CONFIG_AT_ENABLE     := yes
CONFIG_LIBC_ENABLE   := yes
CONFIG_CJSON_ENABLE  := no
CONFIG_TCPIP_TYPE    := none
CONFIG_DTLS_TYPE     := none
CONFIG_EMBEDTLS_MODE := none
CONFIG_MQTT_TYPE     := none
CONFIG_LWM2M_TYPE    := none
CONFIG_OC_MQTT_TYPE  := none
CONFIG_OC_LWM2M_TYPE := bc35
CONFIG_LOADER_ENABLE := no
CONFIG_OC_LWM2M_DEMO_TYPE:= oc_lwm2m_demo_bearpi_template
CONFIG_OC_MQTT_DEMO_TYPE:= none
#################################################################################

include $(TOP_DIR)/iot_link/iot.mk
