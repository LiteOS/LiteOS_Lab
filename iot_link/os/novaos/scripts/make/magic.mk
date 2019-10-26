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

# put all dir in dirs
dirs             += $(curdir)

src-$(curdir)    := $(sort $(abspath $(wildcard $(addprefix $(curdir)/$(src-prefix)/,$(src-y)))))


usr-$(curdir)    := $(sort $(abspath $(wildcard $(addprefix $(curdir)/$(src-prefix)/,$(usr-y)))))

cflags-$(curdir) := $(cflags-$(par-$(curdir))) $(cflags-l-y)
cflags-global    += $(cflags-g-y)

inc-$(curdir)    := $(inc-$(par-$(curdir))) $(addprefix -I, $(abspath $(addprefix $(curdir)/, $(sort $(inc-l-y)))))
inc-global       += $(addprefix -I, $(abspath $(sort $(dir $(wildcard $(addsuffix /./,$(addprefix $(curdir)/,$(inc-g-y))))))))

# expand $(dir-y) and append to $(subs)
ifneq ($(words $(dir-y)),0)
    # _subs hold all the result after wildcard in dir-y add suffix /./ with wildcard to force only find dir
    _subs        := $(abspath $(sort $(dir $(wildcard $(addsuffix /./,$(addprefix $(curdir)/,$(dir-y)))))))

    # append $(proj) to the _subs of $(os_root)
    ifeq ($(curdir),$(os_root))
        ifeq ($(V),1)
            $(info appending the project dir [$(proj)] to _subs)
        endif
        _subs    := $(proj) $(_subs)
    endif

    # save the parent dir of xxx in variable par-xxx
    define def-par
        ifeq ($(V),1)
            $$(info setting par-$(1) = [$(2)])
        endif
        par-$(1) := $(2)
    endef

    $(foreach s,$(_subs),$(eval $(call def-par,$(s),$(curdir))))

    subs         += $(_subs)
endif

# define per-file specific attributes like: [cflags/inc]-/w/dir-a/dir-b/file.c
define def-file-attr
    # $(1) is 'cflags' or 'inc'
    # $(2) is each file name in full path
    # $(3) is '$(cflags-$(curdir))' or '$(inc-$(curdir))'
    # $(4) is '$(curdir)/'

    $(1)-$(2)    := $(3)

    # append cflags-xxx.c, inc-xxx.c to cflags-/w/aa/bb/cc/xxx.c, inc-/w/aa/bb/cc/xxx.c
    ifneq ($$($(1)-$$(patsubst $(4)%,%,$(2))),)
        ifeq ($(V),1)
            $$(info appending $(1)-$$(patsubst $(4)%,%,$(2)) = [$$($(1)-$$(patsubst $(4)%,%,$(2)))] to $(1)-$(2))
        endif

        ifeq ($(1),cflags)
            $(1)-$(2) += $$($(1)-$$(patsubst $(4)%,%,$(2)))
        endif

        ifeq ($(1),inc)
            $(1)-$(2) += $$(addprefix -I,$$(abspath $(4)$$($(1)-$$(patsubst $(4)%,%,$(2)))))
        endif

        # now undefine $(1)-$$(patsubst $(4)%,%,$(2))
        override $(1)-$$(patsubst $(4)%,%,$(2)) :=
    endif
endef

ifneq ("src-$(curdir)", "")
$(foreach s,$(src-$(curdir)),$(eval $(call def-file-attr,cflags,$(s),$(cflags-$(curdir)),$(curdir)/)))
$(foreach s,$(src-$(curdir)),$(eval $(call def-file-attr,inc,$(s),$(inc-$(curdir)),$(curdir)/)))
endif

ifeq ($(V),1)
    # basic info
    $(info curdir = $(strip $(curdir)))
    $(info src-y  = $(strip $(src-y)))
    $(info dirs   = $(strip $(dirs)))
    $(info dir-y  = $(strip $(dir-y)))
    $(info _subs  = $(_subs))
    $(info subs   = $(strip $(subs)))

    # meta info
    $(info src-curdir: src-$(curdir) = $(strip $(src-$(curdir))))
    $(info par-curdir: par-$(curdir) = $(strip $(par-$(curdir))))
    $(info inc-curdir: inc-$(curdir) = $(inc-$(curdir)))
    $(info flags-curdir: cflags-$(curdir) = $(cflags-$(curdir)))
    $(info inc-par-curdir: inc-$(par-$(curdir)) = $(strip $(inc-$(par-$(curdir)))))

    # inc, cflags info
    $(foreach s,$(src-$(curdir)),$(info cflags-$(s) = $(cflags-$(s))))
    $(foreach s,$(src-$(curdir)),$(info inc-$(s) = $(inc-$(s))))
    $(info inc-global = $(inc-global))
endif

ifneq ($(words $(subs)),0)
    parent       := $(curdir)
    curdir       := $(firstword $(subs))
    subs         := $(wordlist 2,$(words $(subs)),$(subs))
    dir-y        :=
    src-y        :=
    usr-y        :=
    cflags-l-y   :=
    cflags-g-y   :=
    inc-g-y      :=
    inc-l-y      :=
    src-prefix   :=
    ifeq ($(V),1)
        $(info ================================================================================)
        $(info parsing $(curdir)/build.mk)
    endif
    include      $(curdir)/build.mk
    include      $(MAGIC)
endif
