################################################################################
# this is used for compile the oc lwm2m agent
################################################################################

OC_LWM2M_AL_SOURCE  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_al/*.c} 
C_SOURCES += $(OC_LWM2M_AL_SOURCE)	
		
OC_LWM2M_AL_INC = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_al
C_INCLUDES += $(OC_LWM2M_AL_INC)

OC_LWM2M_AL_DEFS = -D CFG_OC_LWM2M_ENABLE=1  -D WITH_LWM2M   
C_DEFS += $(OC_LWM2M_AL_DEFS)

#we should choose one of the oc lwm2m implemnt
include $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_imp/oc_lwm2m_imp.mk

OC_LWM2M_DEWO_SOURCE  = ${wildcard $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo/*.c} 
C_SOURCES += $(OC_LWM2M_DEWO_SOURCE)	
		
OC_LWM2M_DEMO_INC = -I $(iot_link_root)/oc/oc_lwm2m/oc_lwm2m_demo
C_INCLUDES += $(OC_LWM2M_DEMO_INC)