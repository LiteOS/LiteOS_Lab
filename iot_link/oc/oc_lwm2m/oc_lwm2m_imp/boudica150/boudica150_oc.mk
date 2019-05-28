BOUDICA150_SOURCE = \
		${wildcard $(TOP_DIR)/components/huawei_cdp/lwm2m/boudica150/*.c}
C_SOURCES += $(BOUDICA150_SOURCE)
        
BOUDICA150_INC = \
		-I $(TOP_DIR)/components/huawei_cdp/lwm2m/boudica150

C_INCLUDES += $(BOUDICA150_INC)