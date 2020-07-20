################################################################################
# this is used for compile the macos socket
################################################################################

#configure the macos socket itself
ESP8266_TCPIP_SRC  = ${wildcard $(iot_link_root)/network/tcpip/esp8266/*.c}
C_SOURCES += $(ESP8266_TCPIP_SRC)
		
ESP8266_TCPIP_INC = -I $(iot_link_root)/network/tcpip/esp8266
C_INCLUDES += $(ESP8266_TCPIP_INC)

C_DEFS += -D CONFIG_AT_BAUDRATE=115200
 