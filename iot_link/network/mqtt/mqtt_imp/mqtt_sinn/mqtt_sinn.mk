################################################################################
# this is used for compile the mqtt lib
################################################################################

mqtt_sinn_src = \
        ${wildcard $(TOP_DIR)/iot_link/network/mqtt/mqtt_imp/mqtt_sinn/mqtt_port/*.c} \
        ${wildcard $(TOP_DIR)/iot_link/network/mqtt/mqtt_imp/mqtt_sinn/mqtt_sinn/*.c}
        C_SOURCES += $(mqtt_sinn_src)

mqtt_sinn_inc = \
        -I $(TOP_DIR)/iot_link/network/mqtt/mqtt_imp/mqtt_sinn/mqtt_sinn \
        -I $(TOP_DIR)/iot_link/network/mqtt/mqtt_imp/mqtt_sinn/mqtt_port

C_INCLUDES += $(mqtt_sinn_inc)
