################################################################################
# this is used for compile the macos socket
################################################################################

#configure the macos socket itself
ESP8266_SOCKET_IMP_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/network/tcpip/esp8266_socket/*.c}
C_SOURCES += $(ESP8266_SOCKET_IMP_SOURCE)
		
ESP8266_SOCKET_IMP_INC = -I $(TOP_DIR)/iot_link/network/tcpip/esp8266_socket
C_INCLUDES += $(ESP8266_SOCKET_IMP_INC)

C_DEFS += -D CONFIG_ESP8266_SOCKET_ENABLE=1
