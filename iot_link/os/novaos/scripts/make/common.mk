#
# Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
#
# LiteOS NOVA is licensed under the Mulan PSL v1.
# You can use this software according to the terms and conditions of the Mulan PSL v1.
# You may obtain a copy of Mulan PSL v1 at:
#
# 	http://license.coscl.org.cn/MulanPSL
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
# FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v1 for more details.
#

CC            := '$(GNU_PREFIX)-gcc'
AS            := '$(GNU_PREFIX)-as'
AR            := '$(GNU_PREFIX)-ar' -r
LD            := '$(GNU_PREFIX)-ld'
NM            := '$(GNU_PREFIX)-nm'
OBJDUMP       := '$(GNU_PREFIX)-objdump'
OBJCOPY       := '$(GNU_PREFIX)-objcopy'
SIZE          := '$(GNU_PREFIX)-size'

cflags-common += --std=gnu99 -Wall -Werror
# keep every function in separate section. This will allow linker to dump unused functions
cflags-common += -ffunction-sections -fdata-sections -fno-strict-aliasing
cflags-common += -fno-builtin --short-enums -Wno-missing-braces
cflags-common += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

ifeq ($(L),1)
cflags-common += -Wa,-a,-ad,-alms=$(@:%.o=%.list)
endif

ifeq ($(D),1)
cflags-common += -O0 -gdwarf-2 -g3
else
cflags-common += -O3
endif

lflags        += -Wl,-Map=$(outdir)/$(target).map,--cref
lflags        += -T$(ld_script)
# let linker to dump unused sections
lflags        += -Wl,--gc-sections
# use newlib in nano version
lflags        += --specs=nano.specs -lc -lnosys