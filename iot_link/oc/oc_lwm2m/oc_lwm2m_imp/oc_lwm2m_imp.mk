
#OC_LWM2M_IMP_DEMO is for the demo; and OC_LWM2M_IMP_AGENT
				
ifeq ($(cfg_oc_lwm2m_type),demo)
	include $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_imp/oc_lwm2m_imp_demo/oc_lwm2m_impdemo.mk
else ifeq ($(cfg_oc_lwm2m_type),soft)
	include $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_imp/atiny_lwm2m/atiny_lwm2m.mk
else
        
endif
