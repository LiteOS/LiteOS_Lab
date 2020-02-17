#
# Copyright (c) <2020>, <Huawei Technologies Co., Ltd>. All rights reserved
#

MAKEFILES = $(shell find . -type f -name Makefile | sed 's,\./Makefile,,g')
SUB_DIRS = $(shell dirname $(MAKEFILES))

GCOV_INFO_SUFFIX := /gcov/gcov.info

GCOV_INFOS := $(addsuffix $(GCOV_INFO_SUFFIX), $(SUB_DIRS))

all:
	@$(foreach N,$(SUB_DIRS),make -C $(N);)

clean:
	@$(foreach N,$(SUB_DIRS),make -C $(N) clean;)
	rm -rf build

run:
	@$(foreach N,$(SUB_DIRS),make -C $(N) run;)

cov:
	@$(foreach N,$(SUB_DIRS),make -C $(N) cov;)
	@mkdir -p build/result
	genhtml -o build/html $(GCOV_INFOS) --rc genhtml_branch_coverage=1

	@$(foreach N,$(SUB_DIRS),cp $(N)/Result.htm build/result/Result_$(notdir $(N)).htm;)

memcheck:
	@$(foreach N,$(SUB_DIRS),make -C $(N) memcheck;)
