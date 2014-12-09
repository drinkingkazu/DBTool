#!/bin/bash

# clean up previously set env
me="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
`python $me/python/clean_env.py LD_LIBRARY_PATH`
`python $me/python/clean_env.py DYLD_LIBRARY_PATH`
`python $me/python/clean_env.py PYTHONPATH`
`python $me/python/clean_env.py PATH`

if [[ -z $FORCE_DBTOOL_DIR ]]; then
    # If DBTOOL_DIR not set, try to guess
    # Find the location of this script:
    me="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    # Find the directory one above.
    export DBTOOL_DIR="$( cd "$( dirname "$me" )" && pwd )"
else
    export DBTOOL_DIR=$FORCE_DBTOOL_DIR
fi
unset me;

echo "DBTOOL_DIR = $DBTOOL_DIR"
if [[ -z $DBTOOL_DIR ]]; then
    echo \$DBTOOL_DIR not set! 
    echo You have to set this first.
else 
    # Abort if ROOT not installed. Let's check rootcint for this.
    if [[ -z `command -v rootcint` ]]; then
	echo
	echo Looks like you do not have ROOT installed.
	echo You cannot use LArLite w/o ROOT!
	echo Aborting.
	echo
	return;
    fi
    DBTOOL_OS=`uname -s`

    # Set path to sub directories
    export DBTOOL_LIBDIR=$DBTOOL_DIR/lib

    if [[ -z $USER_MODULE ]]; then
	export USER_MODULE=""
    fi

    # Check compiler availability for clang++ and g++
    DBTOOL_CXX=clang++
    if [ `command -v $DBTOOL_CXX` ]; then
	export DBTOOL_CXX="clang++ -std=c++11";
    else
	DBTOOL_CXX=g++
	if [[ -z `command -v $DBTOOL_CXX` ]]; then
	    echo
	    echo Looks like you do not have neither clang or g++!
	    echo You need one of those to compile LArLite... Abort config...
	    echo
	    return;
	fi
	export DBTOOL_CXX;
	if [ $DBTOOL_OS = 'Darwin' ]; then
	    echo $DBTOOL_OS
	    echo
	    echo "***************** COMPILER WARNING *******************"
	    echo "*                                                    *"
	    echo "* You are using g++ on Darwin to compile LArLite.    *"
	    echo "* Currently LArLite assumes you do not have C++11    *"
	    echo "* in this combination. Contact the author if this is *"
	    echo "* not the case. At this rate you have no ability to  *"
	    echo "* compile packages using C++11 in LArLite.           *"
	    echo "*                                                    *"
	    echo "* Help to install clang? See manual/contact author!  *"
	    echo "*                                                    *"
	    echo "******************************************************"
	    echo 
	fi
    fi
    if [[ -z $ROOTSYS ]]; then
	case `uname -n` in
	    (houston.nevis.columbia.edu)
	    if [[ -z ${ROOTSYS} ]]; then
		source /usr/nevis/adm/nevis-init.sh
		setup root
		export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH;
	    fi
	    ;;
	    (*)
	    echo
	    echo "****************** PyROOT WARNING ********************"
	    echo "*                                                    *"
	    echo "* Did not find your \$ROOTSYS. To use PyROOT feature, *"
	    echo "* Make sure ROOT.py is installed (comes with ROOT).  *"
	    echo "* You need to export \$PYTHONPATH to include the dir  *"
	    echo "* where ROOT.py exists.                              *"
	    echo "*                                                    *"
	    echo "* Help to install PyROOT? See manual/contact author! *"
	    echo "*                                                    *"
	    echo "******************************************************"
	    echo
	    ;;
	esac
    else
	export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH;
    fi

    export LD_LIBRARY_PATH=$DBTOOL_LIBDIR:$LD_LIBRARY_PATH
    export PYTHONPATH=$DBTOOL_DIR/python:$PYTHONPATH
    if [ $DBTOOL_OS = 'Darwin' ]; then
	export DYLD_LIBRARY_PATH=$DBTOOL_LIBDIR:$DYLD_LIBRARY_PATH
    fi
    export PATH=$DBTOOL_DIR/bin:$PATH
    echo
    echo "Finish configuration. To build, type:"
    echo "> cd \$DBTOOL_DIR"
    echo "> make"
    echo
fi
