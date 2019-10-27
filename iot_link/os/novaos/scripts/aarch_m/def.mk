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

GNU_PREFIX := arm-none-eabi

ifdef CONFIG_CORTEX_M0
cflags-arch += -mcpu=cortex-m0
else ifdef CONFIG_CORTEX_M3
cflags-arch += -mcpu=cortex-m3
else ifdef CONFIG_CORTEX_M4
cflags-arch += -mcpu=cortex-m4
fpu-version := -mfpu=fpv4-sp-d16
else ifdef CONFIG_CORTEX_M7
cflags-arch += -mcpu=cortex-m7
fpu-version := -mfpu=fpv5-sp-d16
endif

ifdef CONFIG_FPU
cflags-arch += -mfloat-abi=hard $(fpu-version)
else
cflags-arch += -mfloat-abi=soft
endif

cflags-arch += -mthumb -mabi=aapcs