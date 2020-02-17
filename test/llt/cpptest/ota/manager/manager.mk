################################################################################
# this is used for compile the pcp modules
################################################################################
PCP_MODULE_SRC  = ${wildcard $(iot_link_root)/ota/manager/*.c} 
C_SOURCES += $(PCP_MODULE_SRC)	
		
PCP_MODULE_INC = -I $(iot_link_root)/ota/manager
C_INCLUDES += $(PCP_MODULE_INC)

C_DEFS += -D CONFIG_MANAGER_ENABLE=1