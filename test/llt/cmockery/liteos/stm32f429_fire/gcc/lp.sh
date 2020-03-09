#!/bin/sh
CUR_DIR="$(dirname "$(readlink -f "$0")")"
cd $CUR_DIR

usage()
{
    echo "Usage: script [option]"
    echo "Options:"
    # echo "  tickless_no_hwi     Use tickless and unsupport platform hardware interrupt (shell can't receive)"
    echo "  tickless            Use tickless and support platform hardware interrupt"
    echo "  runstop             Unuse tickless but the system goes to sleep when it's idle"
    echo "  -h|--help           Show the help information"
    echo ""
    echo "Low-Power test case"
    echo ""
}

run()
{
    make clean
    make -j24 "$@"
}

main()
{
    if [ "X$1" = "Xtickless" ]; then
        run LP_MODE=tickless
    # elif [ "X$1" = "Xtickless_no_hwi" ]; then
    #     run LP_MODE=tickless_no_hwi
    elif [ "X$1" = "Xrunstop" ]; then
        run LP_MODE=runstop
    elif [ "X$1" = "X-h" ] || [ "X$1" = "X--help" ]; then
        usage
    else
        run
    fi
}

main "$@"