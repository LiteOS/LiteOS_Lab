################################################################################
# this is used for compile the macos socket
################################################################################

#configure the macos socket itself
LINUX_SOCKET_IMP_SOURCE  = ${wildcard $(iot_link_root)/network/tcpip/macos_socket/*.c}
C_SOURCES += $(LINUX_SOCKET_IMP_SOURCE)
		
LINUX_SOCKET_IMP_INC = -I $(iot_link_root)/network/tcpip/macos_socket
C_INCLUDES += $(LINUX_SOCKET_IMP_INC)

C_DEFS += -D CONFIG_MACOS_SOCKET_ENABLE=1
