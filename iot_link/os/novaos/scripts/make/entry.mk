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

# global variables
VPATH           = $(os_root)
MAGIC           = $(os_root)/scripts/make/magic.mk

outdir          = gnuc

target          = $(notdir $(PWD))

# default values for VERBOSE, LIST, DEBUG
V              ?= 0
L              ?= 0
D              ?= 0

# variables used for iterate sub dirs must be defined as ':='
# parse from $(os_root)
subs           := $(abspath $(os_root))
curdir         :=
proj           := $(abspath ./)

# all dir (not abspath) being built will be added to dirs
dirs           :=

cflags-global  :=
inc-global     := -I$(os_root)/include -I$(proj)/config

arch            = $(patsubst "%",%,$(CONFIG_ARCH))
soc             = $(curdir)/$(patsubst "%",%,$(CONFIG_SOC))

include        config/kconfig.mk
include        $(os_root)/scripts/$(arch)/def.mk
include        $(os_root)/scripts/make/common.mk
include        $(MAGIC)

cflags-global  += $(cflags-arch)

ifndef GENIDE

################################################################################
# tool gcc rules
################################################################################

ifdef CONFIG_USER_APP
srcs           := $(patsubst $(os_root)%,$(outdir)%,$(foreach d,$(dirs),$(usr-$(d))))
else
# src-$(dir) holds the filename with absolutely path
srcs           := $(patsubst $(os_root)%,$(outdir)%,$(foreach d,$(dirs),$(src-$(d))))
endif

objs           := $(srcs:.c=.o)
objs           := $(objs:.s=.o)

outdirs        := $(sort $(dir $(objs)))

ifeq ($(V),1)
    $(foreach v, $(sort $(filter src-% inc-% par-% cflags-%, $(.VARIABLES))), $(info $(v) = $($(v))))
    $(info objects = $(objs))
    $(info outdir = $(strip $(outdir)))
    $(info outdirs = $(outdirs))
endif

.PHONY : all
all : $(outdir)/$(target).bin $(outdir)/$(target).hex

$(outdir)/$(target).hex : $(outdir)/$(target).elf
	$(OBJCOPY) $< $@ -O ihex

$(outdir)/$(target).bin : $(outdir)/$(target).elf
	$(OBJCOPY) $< $@ -O binary

$(outdir)/$(target).elf : $(objs) $(ld_script)
	$(CC) $(filter %.o, $^) $(cflags-arch) $(lflags) -o $@
	$(SIZE) $@

$(outdir)/%.o : %.c
	$(strip $(CC) $(cflags-common) $(cflags-global) $(cflags-$<) -I$(proj) $(inc-global) $(inc-$(abspath $<)) $< -c -o $@)

$(outdir)/%.o : %.s
	$(strip $(CC) $(cflags-common) $(cflags-global) $(cflags-$<) -I$(proj) $(inc-global) $(inc-$(abspath $<)) $< -c -o $@)

.PHONY : clean
clean :
	@rm -rf $(objs) $(objs:.o=.d) $(objs:.o=.list)

# define rules for creating out dirs
define dir-rule
    ifeq ($(V),1)
        $$(info define rule for $(1))
    endif
    $(1) :
	@mkdir -p $(1)
endef

$(foreach d,$(outdirs),$(eval $(call dir-rule,$(d))))

# define depend rules for objs to dirs
define obj-rule
    ifeq ($(V),1)
        $$(info define rule for $(1) : $$(dir $(1)))
    endif
    $(1) : | $$(dir $(1))
endef

$(foreach o,$(objs),$(eval $(call obj-rule,$(o))))

-include $(objs:.o=.d)

else

################################################################################
# other IDE, like keil, segger ES
################################################################################

ini             = .flags.ini

ide             = $(patsubst "%",%,$(CONFIG_IDE_NAME))

# all, default, do nothing
.PHONY : all
all :
	@echo "IDE support enabled, make has no effect. Please use the IDE project compile the project :-)"

# src-$(dir) or usr-$(dir) holds the filename with absolutely path

ifdef CONFIG_USER_APP
dir-prefix = usr
else
dir-prefix = src
endif

$(ini) :
	$(file >  $@,[global])
	$(file >> $@,flags = $(strip $(filter -D%, $(cflags-common) $(cflags-global))))
	$(file >> $@,inc   = $(strip $(inc-global)))
	$(foreach f, $(foreach d,$(dirs),$($(dir-prefix)-$(d))),    \
                     $(file >>$@,[$(f)])                            \
                     $(file >>$@,flags = $(strip $(cflags-$(f))))   \
                     $(file >>$@,inc   = $(strip $(filter-out $(inc-global), $(inc-$(abspath $(f)))))))

.PHONY : gen-ide
gen-ide: $(ini)
	@python3 $(os_root)/scripts/ide-gen-$(ide) $<

.PHONY : clean
clean :
	@echo "no action for cleanning IDE, please do it in the IDE :-)"

endif

syscall_stub.c :
	@python3 $(os_root)/scripts/$(arch)/syscall_gen $(os_root)/core/services/src/syscall.def

