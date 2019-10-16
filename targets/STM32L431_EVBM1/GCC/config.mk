##############################CONFIGURE INTRODUCTION############################
#configure type      configure value
#------------------:------------------
#CONFIG_OS_TYPE       : "linux" "macos" "liteos"
#CONFIG_SHELL_ENABLE   : y n
#CONFIG_LIBC_ENABLE   : y n
#CONFIG_CJSON_ENABLE  : y n
#CONFIG_TCPIP_TYPE    : "lwip" "linux_socket" "macos_socket" "none"
#CONFIG_DTLS_TYPE     : "mbedtls" "none"
#CONFIG_EMBEDTLS_MODE : "crt" "psk" "none"
#CONFIG_MQTT_TYPE     : "paho" "none"
#CONFIG_LWM2M_TYPE    : "wakaama" "none"
#CONFIG_OC_MQTT_TYPE  : "soft" "none"
#CONFIG_OC_LWM2M_TYPE : "soft" "boudica150" "none"
#CONFIG_LOADER_ENABLE : y n
#CONFIG_OC_LWM2M_DEMO_TYPE:"none"  "oc_lwm2m_demo_dtls" and "oc_lwm2m_demo_nodtls" "oc_lwm2m_demo_bs_dtls"  "oc_lwm2m_demo_bearpi_template"
#CONFIG_OC_MQTT_DEMO_TYPE:"none" "oc_mqtt_demo_static" "oc_mqtt_demo_bs"

######################LOADER     CONFIURE TEMPLATE###############################
#CONFIG_OS_TYPE       = "liteos"
#CONFIG_ARCH_CPU_TYPE := "armv7-m"
#CONFIG_SHELL_ENABLE   = y
#CONFIG_LIBC_ENABLE   = y
#CONFIG_CJSON_ENABLE  = n
#CONFIG_TCPIP_TYPE    = "none"
#CONFIG_DTLS_TYPE     = "none"
#CONFIG_EMBEDTLS_MODE = "none"
#CONFIG_MQTT_TYPE     = "none"
#CONFIG_LWM2M_TYPE    = "none"
#CONFIG_OC_MQTT_TYPE  = "none"
#CONFIG_OC_LWM2M_TYPE = "none"
#CONFIG_LOADER_ENABLE = y
#CONFIG_OC_LWM2M_DEMO_TYPE= "none"
#CONFIG_OC_MQTT_DEMO_TYPE= "none"
#################################################################################
 
#####################  OC MQTT CONFIURE TEMPLATE################################
#CONFIG_OS_TYPE       = "liteos"
#CONFIG_ARCH_CPU_TYPE = "armv7-m"
#CONFIG_SHELL_ENABLE   = y
#CONFIG_DRIVER_ENABLE = n
#CONFIG_AT_ENABLE     = n
#CONFIG_LIBC_ENABLE   = y
#CONFIG_CJSON_ENABLE  = y
#CONFIG_TCPIP_TYPE    = "lwip"
#CONFIG_DTLS_TYPE     = "mbedtls"
#CONFIG_EMBEDTLS_MODE = "crt"
#CONFIG_MQTT_TYPE     = "paho"
#CONFIG_LWM2M_TYPE    = "none"
#CONFIG_OC_MQTT_TYPE  = "soft"
#CONFIG_OC_LWM2M_TYPE = "none"
#CONFIG_LOADER_ENABLE = n
#CONFIG_OC_LWM2M_DEMO_TYPE= "none"
#CONFIG_OC_MQTT_DEMO_TYPE= "oc_mqtt_demo_static"

################################################################################

######################OC LWM2M CONFIURE TEMPLATE#################################
CONFIG_OS_TYPE       := "liteos"
CONFIG_ARCH_CPU_TYPE := "armv7-m"
CONFIG_SHELL_ENABLE  := y
CONFIG_STIMER_ENABLE := y
CONFIG_DRIVER_ENABLE := y
CONFIG_AT_ENABLE     := y
CONFIG_LIBC_ENABLE   := y
CONFIG_CJSON_ENABLE  := n
CONFIG_TCPIP_TYPE    := "none"
CONFIG_DTLS_TYPE     := "none"
CONFIG_EMBEDTLS_MODE := "none"
CONFIG_MQTT_TYPE     := "none"
CONFIG_LWM2M_TYPE    := "none"
CONFIG_OC_MQTT_TYPE  := "none"
CONFIG_OC_LWM2M_TYPE := "boudica150"
CONFIG_LOADER_ENABLE := n
CONFIG_OC_LWM2M_DEMO_TYPE:= "oc_lwm2m_demo_nodtls"
CONFIG_OC_MQTT_DEMO_TYPE:= "none"
#################################################################################

CONFIG_USER_DEMO  = "oc_streetlight_template"


