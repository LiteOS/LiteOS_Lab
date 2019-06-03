################################################################################
# this is used for compile the dtls
################################################################################

ifneq ($(cfg_dtls_type),none)

    DTLS_AL_SOURCE  = ${wildcard $(iot_link_root)/network/dtls/dtls_al/*.c}
    C_SOURCES += $(DTLS_AL_SOURCE)
    		
    DTLS_AL_INC = -I $(iot_link_root)/network/dtls/dtls_al
    C_INCLUDES += $(DTLS_AL_INC)
    
    ifeq ($(cfg_dtls_type), mbedtls)    	
    	include $(iot_link_root)/network/dtls/mbedtls/mbedtls.mk
    endif

endif

