################################################################################
# this is used for compile the queue modules
################################################################################

ifeq ($(CONFIG_LINKQUEUE_ENABLE), y)
    
    C_SOURCES += ${wildcard $(iot_link_root)/queue/*.c} 
    C_INCLUDES += -I $(iot_link_root)/queue

endif
