################################################################################
# this is used for compile the iotlink
################################################################################

SAL_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/network/tcpip/sal/*.c}
C_SOURCES += $(SAL_SOURCE)
		
SAL_INC = -I $(TOP_DIR)/iot_link/network/tcpip/sal
C_INCLUDES += $(SAL_INC)

#now only suport liteos
SAL_CONFIG_TYPE = lwip

ifeq ($(SAL_CONFIG_TYPE), lwip)
	include $(TOP_DIR)/iot_link/network/tcpip/lwip/lwip_imp.mk
endif


