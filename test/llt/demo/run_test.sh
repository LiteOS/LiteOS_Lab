#!/bin/bash
CUR_DIR="$(dirname "$(readlink -f "$0")")"
cd "$CUR_DIR"

TOP_DIR=$CUR_DIR/../../..
TEST_DIR=$TOP_DIR/test
THIRD_DIR=$TEST_DIR/3rd
MOCK_DIR=$TEST_DIR/llt/mock
KERNEL_DIR=$MOCK_DIR/build/kernel

TEST_CASE=test

#
# functions
#
clean()
{
	make distclean
}

run()
{
	# deploy
	sh $THIRD_DIR/deploy_googletest.sh

	# set variable
	export CONFIG_MEM_TYPE="bestfit_little"

	# make lib
	cd "$KERNEL_DIR"
	make clean && make
	cd -

	# make test
	make clean && make

	./${TEST_CASE}
}

main()
{
	if [ "X$1" = "Xclean" ]; then
		clean
	else
		run
	fi
}

main "$@"