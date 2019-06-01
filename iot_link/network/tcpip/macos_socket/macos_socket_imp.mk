################################################################################
# this is used for compile the linux socket
################################################################################

#configure the linux socket itself
LINUX_SOCKET_IMP_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/network/tcpip/macos_socket/*.c}
C_SOURCES += $(LINUX_SOCKET_IMP_SOURCE)
		
LINUX_SOCKET_IMP_INC = -I $(TOP_DIR)/iot_link/network/tcpip/macos_socket
C_INCLUDES += $(LINUX_SOCKET_IMP_INC)

C_DEFS += -D cfg_macos_socket_enable=1
