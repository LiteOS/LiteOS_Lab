ER_COAP_SRC = \
        ${wildcard $(TOP_DIR)/components/connectivity/lwm2m/core/er-coap-13/er-coap-13.c}
        C_SOURCES += $(ER_COAP_SRC)

LWM2M_SRC = \
        ${wildcard $(TOP_DIR)/components/connectivity/lwm2m/core/*.c} \
        ${wildcard $(TOP_DIR)/components/connectivity/lwm2m/examples/shared/*.c}
        C_SOURCES += $(LWM2M_SRC)

ATINY_TINY_SRC = \
        ${wildcard $(TOP_DIR)/components/connectivity/agent_tiny/atiny_lwm2m/*.c}
        
ifeq ($(USE_FOTA), yes)
        C_SOURCES += $(ATINY_TINY_SRC)
else
        ATINY_TINY_SRC_NO_FOTA = \
        $(filter-out $(TOP_DIR)/components/connectivity/agent_tiny/atiny_lwm2m/atiny_fota_manager.c \
        $(TOP_DIR)/components/connectivity/agent_tiny/atiny_lwm2m/atiny_fota_state.c \
        $(TOP_DIR)/components/connectivity/agent_tiny/atiny_lwm2m/firmware_update.c, $(ATINY_TINY_SRC))
        C_SOURCES += $(ATINY_TINY_SRC_NO_FOTA)
endif

AGENT_DEMO_SRC = \
        ${wildcard $(TOP_DIR)/demos/agenttiny_lwm2m/*.c}
        C_SOURCES += $(AGENT_DEMO_SRC)