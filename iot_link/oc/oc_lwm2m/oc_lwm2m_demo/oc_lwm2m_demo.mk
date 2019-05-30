################################################################################
# this is used for compile the oc lwm2m demo
################################################################################

#two types for you to compile:oc_lwm2m_demo_dtls and oc_lwm2m_demo_nodtls

cfg_oc_lwm2m_demo_type = oc_lwm2m_demo_dtls
oc_lwm2m_demo_defs =
		
oc_lwm2m_demo_inc = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo
C_INCLUDES += $(oc_lwm2m_demo_inc)

ifeq ($(cfg_oc_lwm2m_demo_type),oc_lwm2m_demo_dtls)
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_dtls.c}
		
else ifeq ($(cfg_oc_lwm2m_demo_type),oc_lwm2m_demo_nodtls)	
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_nodtls.c}
endif
		 
C_SOURCES += $(oc_lwm2m_demo_src)
C_DEFS += $(oc_lwm2m_demo_defs)	