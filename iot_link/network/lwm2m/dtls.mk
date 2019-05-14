################################################################################
# this is used for compile the dtls
################################################################################

LWM2M_MODULE_SOURCE  = ${wildcard $(iot_link_root)/network/lwm2m/lwm2m_al/*.c}
C_SOURCES += $(LWM2M_MODULE_SOURCE)
		
LWM2M_MODULE_INC = -I $(iot_link_root)/network/lwm2m/lwm2m_al
C_INCLUDES += $(LWM2M_MODULE_INC)

#only support it wakaama now
LWM2M_MODULE_CONFIG_TYPE = wakaama
ifeq ($(LWM2M_MODULE_CONFIG_TYPE), wakaama)
	include $(iot_link_root)/network/dtls/mbedtls/mbedtls.mk
endif
