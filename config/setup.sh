#!/usr/bin/env bash

me="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export DBTOOL_DIR="$( cd "$( dirname "$me" )" && pwd )"

dbtool_cmake_flag=""
case `uname -n` in
    (*ubdaq-prod*)
    echo Setting up for ubdaq-prod machines...
    source /uboonenew/setup_online.sh
#    setup postgresql v9_3_6 -q p279
    setup postgresql v9_1_14 
    setup cmake v3_2_1
    setup root v5_34_25a -q e7:prof
    source /home/uboonedaq/.sqlaccess/prod_conf.sh

    case `whoami` in
	(uboonedaq)
        echo Setting up production config db...
	;;
	(*)
        echo Setting up test config db...
	export DBTOOL_READER_DB="testrunconfdb";
	export DBTOOL_WRITER_DB="testrunconfdb";
	export DBTOOL_ADMIN_DB="testrunconfdb";
	;;
    esac

    dbtool_cmake_flag="-DCMAKE_CXX_COMPILER=c++"
    ;;
    (*)
    if [[ -e $DBTOOL_DIR/config/${USER}_conf.sh ]]; then
	echo Sourcing a user specific DB config script...
	source $DBTOOL_DIR/config/${USER}_conf.sh
    else
	echo No special setup done for the server `uname -n`
    fi
    ;;
esac

# Abort if ROOT not installed. Let's check rootcint for this.
if [[ -z `command -v rootcint` ]]; then
    echo
    echo Looks like you do not have ROOT installed.
    echo You cannot use LArLite w/o ROOT!
    echo Aborting.
    echo
    return;
fi

# Abort if PSQL not installed. Let's check rootcint for this.
if [[ -z `command -v pg_config` ]]; then
    echo
    echo Looks like you do not have PSQL installed.
    echo You cannot use LArLite w/o PSQL!
    echo Aborting.
    echo
    return;
fi

# Abort if cmake not installed. Let's check rootcint for this.
if [[ -z `command -v cmake` ]]; then
    echo
    echo Looks like you do not have cmake installed.
    echo You cannot use LArLite w/o cmake!
    echo Aborting.
    echo
    return;
fi

export PATH=$DBTOOL_DIR/bin:$PATH
export LD_LIBRARY_PATH=$DBTOOL_DIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$DBTOOL_DIR/python:$PYTHONPATH

mkdir -p $DBTOOL_DIR/build
cd $DBTOOL_DIR/build
cmake $DBTOOL_DIR $dbtool_cmake_flag
echo ===============================================
echo === No error? Ready to build! Type \"make\" ===
echo ===============================================

