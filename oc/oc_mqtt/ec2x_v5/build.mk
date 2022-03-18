EC2X_SOURCE = ${wildcard $(iot_link_root)/oc/oc_mqtt/ec2x_v5/*.c}
C_SOURCES += $(EC2X_SOURCE)

EC2X_INC = -I $(iot_link_root)/oc/oc_mqtt/ec2x_v5
C_INCLUDES += $(EC2X_INC)

