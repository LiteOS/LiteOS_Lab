################################################################################
# this is used for compile the oc lwm2m demo
################################################################################

#two types for you to compile:none oc_lwm2m_demo_dtls and oc_lwm2m_demo_nodtls oc_lwm2m_demo_bs_dtls
	
ifeq ($(cfg_oc_lwm2m_demo_type), none)
	oc_lwm2m_demo_src =
	oc_lwm2m_demo_inc =
	oc_lwm2m_demo_defs =
	
else ifeq ($(cfg_oc_lwm2m_demo_type), oc_lwm2m_demo_dtls_bs)

	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_dtls_bs.c}
	oc_lwm2m_demo_inc = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo
	oc_lwm2m_demo_defs = -D LWM2M_BOOTSTRAP=1 -D cfg_oc_lwm2m_demo_enable=1

else ifeq ($(cfg_oc_lwm2m_demo_type), oc_lwm2m_demo_dtls)
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_dtls.c}
	oc_lwm2m_demo_inc = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo
	oc_lwm2m_demo_defs = -D cfg_oc_lwm2m_demo_enable=1

else ifeq ($(cfg_oc_lwm2m_demo_type), oc_lwm2m_demo_nodtls)	
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_nodtls.c}
	oc_lwm2m_demo_inc = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo
	oc_lwm2m_demo_defs = -D cfg_oc_lwm2m_demo_enable=1
	
endif
		 
C_SOURCES += $(oc_lwm2m_demo_src)
C_INCLUDES += $(oc_lwm2m_demo_inc)
C_DEFS += $(oc_lwm2m_demo_defs)

