CURRENT_DIR = $(abspath $(CURDIR))
IOT_LINK_DIR = $(abspath $(CURRENT_DIR)/../../../iot_link)
LZMA_DIR = $(abspath $(IOT_LINK_DIR)/compression_algo/lzma) 
HDIFFPATCH_DIR = $(abspath $(IOT_LINK_DIR)/upgrade_patch/HDiffPatch)
OTA_SRC = $(abspath $(CURRENT_DIR)/../SRC)

HPATCH_OBJ := \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HPatch/patch.o \
    $(HDIFFPATCH_DIR)/file_for_patch.o \
    $(HDIFFPATCH_DIR)/dirDiffPatch/dir_patch/dir_patch.o

HDIFF_OBJ := \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/diff.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/bytes_rle.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/suffix_string.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/compress_detect.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/limit_mem_diff/digest_matcher.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/limit_mem_diff/stream_serialize.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/libdivsufsort/divsufsort64.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/libdivsufsort/divsufsort.o \
    $(HDIFFPATCH_DIR)/libHDiffPatch/HDiff/private_diff/limit_mem_diff/adler_roll.o \
	$(HDIFFPATCH_DIR)/hdiffz.o \
    $(HPATCH_OBJ)

DEF_FLAGS := \
    -O3 -DNDEBUG -D_FILE_OFFSET_BITS=64 \
    -D_IS_NEED_ORIGINAL=0 \
    -D_IS_NEED_ALL_CompressPlugin=0 \
    -D_IS_NEED_DEFAULT_CompressPlugin=0 \
    -D_IS_NEED_ALL_ChecksumPlugin=0 \
    -D_IS_NEED_DEFAULT_ChecksumPlugin=0 \
	-D_IS_NEED_DIR_DIFF_PATCH=0 \
    -D_CompressPlugin_lzma -I'$(LZMA_DIR)/../lzma/C' \
    -D_7ZIP_ST \
    -D_IS_USED_MULTITHREAD=0 \
	-D_IS_NEED_MAIN=0

PATCH_LINK := 
DIFF_LINK  := $(PATCH_LINK)

CFLAGS   += $(DEF_FLAGS) 
CXXFLAGS += $(DEF_FLAGS)

.PHONY: all clean

all: lzmaLib libhdiffpatch.a hdiffzlib

LZMA_DEC_OBJ := 'LzmaDec.o'
LZMA_OBJ     := 'LzFind.o' 'LzmaEnc.o' $(LZMA_DEC_OBJ)
LZMA_SRC     := '$(LZMA_DIR)/../lzma/C/LzFind.c' '$(LZMA_DIR)/../lzma/C/LzmaDec.c' '$(LZMA_DIR)/../lzma/C/LzmaEnc.c'
			 
lzmaLib: # https://www.7-zip.org/sdk.html  https://github.com/sisong/lzma
	$(CC) -c $(CFLAGS) $(LZMA_SRC)		 


libhdiffpatch.a: $(HDIFF_OBJ)
	$(AR) rcs $@ $^

hdiffzlib: 
	$(CXX) $(OTA_SRC)/hdiffz_ota.cpp -shared -fPIC $(HDIFF_OBJ) $(LZMA_OBJ) $(CXXFLAGS) $(DIFF_LINK) -o libhdiffz.dll

RM := rm -f

clean:
	$(RM) libhdiffpatch.a libhdiffz.dll $(HDIFF_OBJ) $(LZMA_OBJ)

