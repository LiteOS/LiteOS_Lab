################################################################################
# this is used for compile the iotlink
################################################################################

SAL_SOURCE  = ${wildcard $(iot_link_root)/network/tcpip/sal/*.c}
C_SOURCES += $(SAL_SOURCE)
		
SAL_INC = -I $(iot_link_root)/network/tcpip/sal
C_INCLUDES += $(SAL_INC)

#now only suport liteos
SAL_CONFIG_TYPE = lwip

ifeq ($(SAL_CONFIG_TYPE), lwip)
	include $(iot_link_root)/network/tcpip/lwip/lwip_imp.mk
endif


