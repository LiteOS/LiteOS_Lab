################################################################################
# this is used for compile the oc lwm2m demo
################################################################################

#two types for you to compile:oc_lwm2m_demo_dtls and oc_lwm2m_demo_nodtls

cfg_oc_lwm2m_demo_type = oc_lwm2m_demo_dtls_bs
cfg_oc_lwm2m_demo_defs =
		
oc_lwm2m_demo_inc = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo
C_INCLUDES += $(oc_lwm2m_demo_inc)

ifeq ($(cfg_oc_lwm2m_demo_type),oc_lwm2m_demo_dtls_bs)
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_dtls_bs.c}
	cfg_oc_lwm2m_demo_defs = -D LWM2M_BOOTSTRAP=1

else ifeq ($(cfg_oc_lwm2m_demo_type),oc_lwm2m_demo_dtls)
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_dtls.c}
		
else ifeq ($(cfg_oc_lwm2m_demo_type),oc_lwm2m_demo_nodtls)	
	oc_lwm2m_demo_src  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/oc_lwm2m_demo_nodtls.c}
else

endif
		 
C_SOURCES += $(oc_lwm2m_demo_src)
C_DEFS += $(cfg_oc_lwm2m_demo_defs)
