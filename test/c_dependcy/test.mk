################################################################################
# this is used for compile the test
################################################################################
test_dependcy = $(SDK_DIR)/test/c_dependcy
test_dependcy_src  = $(test_dependcy)/test_main.c

ifeq ($(TEST_CONFIG_OC_MQTT_ENABLE), y)
test_dependcy_src += $(test_dependcy)/test_case_oc_mqtt.c
C_DEFS += -D TEST_CONFIG_OC_MQTT_ENABLE=1
endif

ifeq ($(TEST_CONFIG_MQTT_AL_ENABLE), y)
test_dependcy_src += $(test_dependcy)/test_case_mqtt_al.c
C_DEFS += -D TEST_CONFIG_MQTT_AL_ENABLE=1
endif

C_SOURCES += $(test_dependcy_src)	

test_dependcy_inc = -I $(test_dependcy)
C_INCLUDES += $(test_dependcy_inc)

C_DEFS += -D CONFIG_AUTO_TEST=1

