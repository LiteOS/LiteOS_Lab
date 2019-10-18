################################################################################
# this is used for compile the queue modules
################################################################################
QUEUE_MODULE_SRC  = ${wildcard $(iot_link_root)/queue/*.c} 
C_SOURCES += $(QUEUE_MODULE_SRC)	
		
QUEUE_MODULE_INC = -I $(iot_link_root)/queue
C_INCLUDES += $(QUEUE_MODULE_INC)

C_DEFS += -D CONFIG_QUEUE_ENABLE=1