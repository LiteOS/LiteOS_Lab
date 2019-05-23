################################################################################
# this is used for compile the dtls
################################################################################
#only support it wakaama now
LWM2M_MODULE_CONFIG_TYPE = wakaama
ifeq ($(LWM2M_MODULE_CONFIG_TYPE), wakaama)
	include $(iot_link_root)/network/lwm2m/lwm2m_imp/wakaama/wakaama.mk
endif
