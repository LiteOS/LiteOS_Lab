##############################CONFIGURE INTRODUCTION############################
#configure type      configure value
#------------------:------------------
#cfg_os_type       : linux macos liteos
#cfg_shell_enble   : yes no
#cfg_libc_enable   : yes no
#cfg_cjson_enable  : yes no
#cfg_tcpip_type    : lwip linux_socket macos_socket none
#cfg_dtls_type     : mbedtls none
#cfg_embedtls_mode : crt psk none
#cfg_mqtt_type     : paho none
#cfg_lwm2m_type    : wakaama none
#cfg_oc_mqtt_type  : soft none
#cfg_oc_lwm2m_type : soft bc35 none
#cfg_oc_bs_enable  : yes no
#cfg_loader_enable : yes no

######################LOADER     CONFIURE TEMPLATE###############################
#cfg_os_type       := liteos
#cfg_shell_enble   := yes
#cfg_libc_enable   := yes
#cfg_cjson_enable  := no
#cfg_tcpip_type    := none
#cfg_dtls_type     := none
#cfg_embedtls_mode := none
#cfg_mqtt_type     := none
#cfg_lwm2m_type    := none
#cfg_oc_mqtt_type  := none
#cfg_oc_lwm2m_type := none
#cfg_oc_bs_enable  := no
#cfg_loader_enable := yes
################################################################################# 
#####################  OC MQTT CONFIURE TEMPLATE###############################
#cfg_os_type       := liteos
#cfg_shell_enble   := yes
#cfg_libc_enable   := yes
#cfg_cjson_enable  := yes
#cfg_tcpip_type    := lwip
#cfg_dtls_type     := mbedtls
#cfg_embedtls_mode := crt
#cfg_mqtt_type     := paho
#cfg_lwm2m_type    := none
#cfg_oc_mqtt_type  := soft
#cfg_oc_lwm2m_type := none
#cfg_oc_bs_enable  := no
#cfg_loader_enable := no
################################################################################
#####################OC LWM2M CONFIURE TEMPLATE#################################
cfg_os_type       := liteos
cfg_shell_enble   := yes
cfg_libc_enable   := yes
cfg_cjson_enable  := yes
cfg_tcpip_type    := lwip
cfg_dtls_type     := mbedtls
cfg_embedtls_mode := psk
cfg_mqtt_type     := none
cfg_lwm2m_type    := wakaama
cfg_oc_mqtt_type  := none
cfg_oc_lwm2m_type := soft
cfg_oc_bs_enable  := no
cfg_loader_enable := no
#################################################################################

include $(TOP_DIR)/iot_link/iot.mk