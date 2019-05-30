################################################################################
# this is used for compile the iotlink
################################################################################

SAL_SOURCE  = ${wildcard $(iot_link_root)/network/tcpip/sal/*.c}
C_SOURCES += $(SAL_SOURCE)
		
SAL_INC = -I $(iot_link_root)/network/tcpip/sal
C_INCLUDES += $(SAL_INC)

#now suport lwip and linux socket
#SAL_CONFIG_TYPE = socket

ifeq ($(SAL_CONFIG_TYPE), lwip)
	include $(iot_link_root)/network/tcpip/lwip/lwip_imp.mk
else ifeq ($(SAL_CONFIG_TYPE), socket)
	include $(iot_link_root)/network/tcpip/linux/linux_socket_imp.mk
endif

C_DEFS += -D cfg_tcpip_enable=1
