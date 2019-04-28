################################################################################
# this is used for compile the iotlink
################################################################################


LITEOS_IMP_SOURCE  = ${wildcard $(TOP_DIR)/iot_link/oskernel/liteos/*.c}
C_SOURCES += $(LITEOS_IMP_SOURCE)
		
LITEOS_IMP_INC = -I $(TOP_DIR)/iot_link/oskernel/liteos
C_INCLUDES += $(LITEOS_IMP_INC)

