################################################################################
# this is used for compile the oceanconnect
################################################################################
ifeq ($(CONFIG_OTA_SHA256CHECK_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/link_ota/download_check/sha256check/*.c}
    C_INCLUDES += -I $(iot_link_root)/link_ota/download_check/sha256check     
endif

ifeq ($(CONFIG_OTA_SIGN_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/link_ota/download_check/sign/*.c}
    C_INCLUDES += -I $(iot_link_root)/link_ota/download_check/sign     
endif

ifeq ($(CONFIG_OTA_SIGNVERIFY_ENABLE),y)
    C_SOURCES += ${wildcard $(iot_link_root)/link_ota/download_check/sign_verify/*.c}
    C_INCLUDES += -I $(iot_link_root)/link_ota/download_check/sign_verify     
endif

ifeq ($(CONFIG_OTA_SIGN_TEST),y)
    C_SOURCES += $(iot_link_root)/link_ota/download_check/sign/ota_sign_test.c
endif

