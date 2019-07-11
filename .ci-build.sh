#!/bin/bash
set -o errexit

function do_build ()
{
    eval $1

    if [ $? -ne 0 ]
    then
        echo "Build fail! Exiting the build!"
        echo "Build command:"
        echo "$1"
        exit $?
    fi
}

function gcc_build ()
{
    echo "building $1 with default config"

    do_build "make -j2 -C $1 > /dev/null"

    config="$1/config_demos"

    if [ -d "$config" ]
    then
        for c in $config/*.mk
        do
            f="config_demos/`basename $c`"
            if [ -f "$1/$f" ]
            then
                make clean -C $1
                echo "building $1 with config = $f"

                do_build "make -j2 -C $1 CONFIG_FILE=$f > /dev/null"
            fi
        done
    fi
}

function build_target ()
{
    if [ -f "$1/Makefile" ]
    then
        gcc_build $1
    else
        itr_target $1
    fi
}

function itr_target ()
{
    if [ -d "$1" ]
    then
        for t in $1/*
        do
            if [ -d "$t" ]
            then
                build_target $t
            fi
        done
    fi
}

itr_target "./targets"

