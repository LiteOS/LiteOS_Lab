################################################################################
# this is used for compile the macos socket
################################################################################

#configure the macos socket itself
RTK8710_TCPIP_SRC  = ${wildcard $(iot_link_root)/network/tcpip/rtk8710/*.c}
C_SOURCES += $(RTK8710_TCPIP_SRC)
		
RTK8710_TCPIP_INC = -I $(iot_link_root)/network/tcpip/rtk8710
C_INCLUDES += $(RTK8710_TCPIP_INC)
