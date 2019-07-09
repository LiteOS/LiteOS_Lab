BOUDICA150_SOURCE = \
		${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_imp/boudica150/*.c} 
C_SOURCES += $(BOUDICA150_SOURCE)
        
BOUDICA150_INC = \
		-I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_imp/boudica150

C_INCLUDES += $(BOUDICA150_INC)
C_DEFS += -D cfg_oc_lwm2m_boudica150_enable=1