GNU_PREFIX := arm-none-eabi

ifdef      LOSCFG_CORTEX_M0
cflags-arch += -mcpu=cortex-m0
else ifdef LOSCFG_CORTEX_M3
cflags-arch += -mcpu=cortex-m3
else ifdef LOSCFG_CORTEX_M4
cflags-arch += -mcpu=cortex-m4
fpu-version := -mfpu=fpv4-sp-d16
else ifdef LOSCFG_CORTEX_M7
cflags-arch += -mcpu=cortex-m7
fpu-version := -mfpu=fpv5-sp-d16
endif

ifdef      LOSCFG_FPU
cflags-arch += -mfloat-abi=hard $(fpu-version)
else
cflags-arch += -mfloat-abi=soft
endif

cflags-arch += -mthumb -mabi=aapcs