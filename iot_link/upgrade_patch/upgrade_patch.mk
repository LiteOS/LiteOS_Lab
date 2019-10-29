
UPGRADE_MODULE_SRC = $(iot_link_root)/upgrade_patch/HDiffPatch/libHDiffPatch/HPatch/patch.c \
			$(iot_link_root)/upgrade_patch/HDiffPatch/file_for_patch.c \
			$(iot_link_root)/upgrade_patch/HDiffPatch/dirDiffPatch/dir_patch/dir_patch.c
C_SOURCES += $(UPGRADE_MODULE_SRC)
C_DEFS += -D _CompressPlugin_lzma

UPGRADE_MODULE_INC = -I $(iot_link_root)/upgrade_patch/HDiffPatch
C_INCLUDES += $(UPGRADE_MODULE_INC)
