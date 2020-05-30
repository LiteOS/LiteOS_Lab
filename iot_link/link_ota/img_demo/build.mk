################################################################################
# this is used for compile the ota img demo
################################################################################
ifeq ($(CONFIG_OTA_IMG_MEMORY),y)
    C_SOURCES += $(iot_link_root)/link_ota/img_demo/img_memory.c 
else ifeq ($(CONFIG_OTA_IMG_NULL),y)   
    C_SOURCES += $(iot_link_root)/link_ota/img_demo/img_null.c 
endif

