################################################################################
# this is used for compile the oceanconnect
################################################################################
ifeq ($(CONFIG_OTA_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/link_ota/*.c}
    C_INCLUDES += -I $(iot_link_root)/link_ota
    C_DEFS += -D CONFIG_OTA_ENABLE=1
    ifeq ($(CONFIG_OTA_PATCH),y)
    	include  $(iot_link_root)/link_ota/compress/build.mk  
 		include  $(iot_link_root)/link_ota/differential/build.mk  
 		include  $(iot_link_root)/link_ota/patch/build.mk    
    endif
    
    ifeq ($(CONFIG_OTA_IMG_DEMO),y)
    	include  $(iot_link_root)/link_ota/img_demo/build.mk   
    endif
    
    ifeq ($(CONFIG_OTA_IMG_CHECK),y)
    	include  $(iot_link_root)/link_ota/img_check/build.mk   
    endif
    
    ifeq ($(CONFIG_OTA_DOWNLOADCHECK_ENABLE),y)
       	include  $(iot_link_root)/link_ota/download_check/build.mk   
    endif
    
    ifeq ($(CONFIG_OTA_DOWNLOAD_ENABLE),y)
       	include  $(iot_link_root)/link_ota/download/build.mk   
    endif
    
       
endif

