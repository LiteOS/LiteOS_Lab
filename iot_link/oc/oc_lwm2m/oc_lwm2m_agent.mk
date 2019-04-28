################################################################################
# this is used for compile the oc lwm2m agent
################################################################################

##uptils now , we only support the boudica150
WITH_BOUDICA150 = yes

OC_LWM2M_AGENT_SOURCE  = \
		${wildcard $(TOP_DIR)/components/huawei_cdp/lwm2m/*.c} 
C_SOURCES += $(OC_LWM2M_AGENT_SOURCE)	
		
OC_LWM2M_AGENT_INC = \
	    -I $(TOP_DIR)/components/huawei_cdp/lwm2m

C_INCLUDES += $(OC_LWM2M_AGENT_INC)


OC_LWM2M_AGENT_DEFS = \
     		-D LOS_CFG_OC_LWM2M_AGENT=1
    
C_DEFS += $(OC_LWM2M_AGENT_DEFS)

				
ifeq ($(WITH_BOUDICA150),yes)
	BOUDICA150_SOURCE = \
    		${wildcard $(TOP_DIR)/components/huawei_cdp/lwm2m/boudica150/*.c}
	C_SOURCES += $(BOUDICA150_SOURCE)
            
    BOUDICA150_INC = \
    		-I $(TOP_DIR)/components/huawei_cdp/lwm2m/boudica150
    
    C_INCLUDES += $(BOUDICA150_INC)
        
endif