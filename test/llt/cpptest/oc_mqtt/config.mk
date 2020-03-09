##
# note: how to configure iot_link? you can see targets/LINUX/GCC/config.mk.
#		Keep it simple here
##

CONFIG_OS_TYPE          := "linux"

CONFIG_ARCH_CPU_TYPE    := "x86-64"

CONFIG_TCPIP_ENABLE     := y
CONFIG_TCPIP_TYPE       := "linux_socket"

CONFIG_CJSON_ENABLE     := y

CONFIG_DTLS_ENABLE      := y
CONFIG_DTLS_TYPE        := "mbedtls_cert"

CONFIG_OC_MQTT_ENABLE   := y
CONFIG_OC_MQTT_TYPE     := "soft"

include $(TOP_DIR)/iot_link/iot.mk
