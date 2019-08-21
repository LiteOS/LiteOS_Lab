#!/bin/sh
CUR_DIR="$(dirname "$(readlink -f "$0")")"
cd "$CUR_DIR"

# params
ARCH_TYPE="32" # 32/64
INSTALL_DIR=googletest

Usage() {
	echo "Usage:"
	echo "	${SCRIPT_NAME} [-h] [-m 32|64] args ..."
	echo ""
	echo "OPTIONS"
	echo "	-m <32|64>       Generate code for a 32-bit or 64-bit environment"
	echo "	-d <dir>         Specify installation directory"
	echo "	-h               Show help message"
	echo ""
} 

# getopts
while getopts "m:d:h" opt; do
	case $opt in
		m)
			ARCH_TYPE=$OPTARG
			echo "ARCH_TYPE: $ARCH_TYPE"
			;;
		d)
			INSTALL_DIR=$OPTARG
			echo "INSTALL_DIR: $INSTALL_DIR"
			;;
		h)
			Usage
			;;
		\?)
			Usage
			exit -1
			;;
	esac
done

# check
if [ -d "${INSTALL_DIR}" ]; then
	echo "google-test is deployed in ${INSTALL_DIR}"
	exit
fi

# download googletest
GOOGLETEST_BASE_NAME=googletest-release-1.8.1
GOOGLETEST_FILE=${GOOGLETEST_BASE_NAME}.zip
if [ ! -d ${GOOGLETEST_BASE_NAME} ]; then
	if [ ! -e ${GOOGLETEST_FILE} ];then
		wget -c https://github.com/google/googletest/archive/release-1.8.1.zip -O ${GOOGLETEST_FILE}
		if [ $? != 0 ]; then
			echo "Err: download googletest failed!"
			exit -2
		fi
	fi
	unzip -q ${GOOGLETEST_FILE}
fi

# flags
if [ "X$ARCH_TYPE" = "X32" ];then
ARCH_FLAGS="-D CMAKE_CXX_FLAGS=-m32"
fi

# dir
SRC_DIR=$CUR_DIR/$GOOGLETEST_BASE_NAME
INSTALL_DIR=$CUR_DIR/$INSTALL_DIR
BUILD_DIR=$SRC_DIR/build

# clean
rm -rf "$INSTALL_DIR"
rm -rf "$BUILD_DIR"

mkdir -p "$INSTALL_DIR/lib"
mkdir -p "$BUILD_DIR"

# build
cd "$BUILD_DIR"
cmake ${ARCH_FLAGS} ../
if [ $? != 0 ]; then
	exit -3
fi
make clean
make

# copy
A_FILES="$(find ./ "-name" *.a)"
cp -rf ${A_FILES} "$INSTALL_DIR/lib"
cp -rf ../googlemock/include "$INSTALL_DIR"
cp -rf ../googletest/include "$INSTALL_DIR"

cd "$CUR_DIR"