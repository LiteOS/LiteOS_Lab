BOUDICA120_SOURCE = ${wildcard $(iot_link_root)/oc/oc_lwm2m/boudica120_oc/*.c} 
C_SOURCES += $(BOUDICA120_SOURCE)
        
BOUDICA120_INC = -I $(iot_link_root)/oc/oc_lwm2m/boudica120_oc
C_INCLUDES += $(BOUDICA120_INC)

C_DEFS += -D CONFIG_OC_LWM2M_BOUDICA120_ENABLE=1