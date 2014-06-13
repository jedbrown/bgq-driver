# begin_generated_IBM_copyright_prolog
#
# This is an automatically generated copyright prolog.
# After initializing,  DO NOT MODIFY OR MOVE
# ================================================================
#
# Licensed Materials - Property of IBM
#
# Blue Gene/Q
#
# (C) Copyright IBM Corp.  2011, 2011
#
# US Government Users Restricted Rights -
# Use, duplication or disclosure restricted
# by GSA ADP Schedule Contract with IBM Corp.
#
# This software is available to you under the
# Eclipse Public License (EPL).
#
# ================================================================
#
# end_generated_IBM_copyright_prolog

# process this file with autoconf to produce a configure script.

# Note: The original copy of this file was produced by autoscan 
# as "configure.scan" using an empty "configure.ac" file as a seed,
# and then renaming "configure.scan" to "configure.ac" [--DL].

AC_PREREQ(2.59)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)

# ============================================================================
#
#       System Type, Location, Default paths
#
# ============================================================================

AC_MSG_NOTICE([Getting Operating System Type, Location, Defaults ...])

BGQ_BUILD_DIR=`pwd -P`
AC_SUBST(BGQ_BUILD_DIR)
AC_MSG_RESULT([  BGQ_BUILD_DIR=$BGQ_BUILD_DIR])

# with opiton for which flavor of gnu glibc we build
#BGQ_GNU_MAKEFILE=Makefile.linux.glibc
#
AC_ARG_WITH(gnu-makefile,
  AS_HELP_STRING([--with-gnu-makefile=<Makefilename>], [name of the gnu make file to use]),
  [ BGQ_GNU_MAKEFILE="$withval"; ],
  [ BGQ_GNU_MAKEFILE="Makefile.linux.glibc"; ])


# allow a simpler specifiction, and make sure the file exists
case $BGQ_GNU_MAKEFILE in
  newlib) BGQ_GNU_MAKEFILE=Makefile.newlib;;

  linux)  BGQ_GNU_MAKEFILE=Makefile.linux.glibc;;
  Makefile)
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "!!!!!!!!!!!!!!  Invalid --with-gnu-makefile option  !!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        exit 1;;
esac
if [[[ -d gnu ]]]; then
     if [[[ ! -e gnu/$BGQ_GNU_MAKEFILE ]]]; then
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "!!!!!!!!!!!!!!  Invalid --with-gnu-makefile option  !!!!!!!!!!!!!!!!!!!!!!!!!"
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        exit 1
     fi
fi


AC_SUBST(BGQ_GNU_MAKEFILE)


#
# enable make floor option.  
#     usage --enable-makefloor=<floor dir>
#     
#     parameter is specified, then we override the prefix, 
#           
AC_ARG_ENABLE(makefloor,
              AS_HELP_STRING([--enable-makefloor=<floor dir>], [configure the build to make a floor release.  This will force the configure step to do the right thing when preparing a floor release]),
              [BGQ_ENABLE_MAKEFLOOR=$enableval],
	      [BGQ_ENABLE_MAKEFLOOR=no])

if [[[ $BGQ_ENABLE_MAKEFLOOR != no ]]] ; then

    if [[[ -z $BGQ_ENABLE_MAKEFLOOR ]]]; then
        echo
        echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	echo "Invalid syntax for --enable-makefloor"
    	echo "  usage for --enable-makefloor:"
	echo "    --enable-makefloor=<target-floor-dir>"
	echo "       where <target-floor-dir> is the directory you want to build the floor in."
        echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	exit 1;
    fi
    if [[ $prefix != "NONE" ]]; then
    	echo "--enable-makefloor and --prefix cannot be used together..."
	exit 1;
    fi 

    #
    # construct a driver name:
    # i.e. DRV320_2011-110805Q/x86_64-rhel52
    mc=`uname -m`

    os_release=`lsb_release -r | awk '{print $2}' | sed -e 's/\.//g'`
    
    dweek=$((`date +%W`+1))
    drvrname=`date +DRV\${dweek}0_%Y-%y%m%dQ`
    bn=$BGQ_ENABLE_MAKEFLOOR/$drvrname/$mc-rhel$os_release
    fd=$bn;
    let n=0;
    while [[[ -e $fd ]]]; do
	  let n=$n+1;
	  fd=${bn}_$n
    done
    BGQ_ENABLE_MAKEFLOOR=$fd;
    echo BGQ_ENABLE_MAKEFLOOR=$BGQ_ENABLE_MAKEFLOOR
    echo BGQ_GNU_MAKEFILE=$BGQ_GNU_MAKEFILE
    prefix=$BGQ_ENABLE_MAKEFLOOR
    #signal we are making a floor
fi


#
# give us a default prefix
#
if [[ $prefix = "NONE" ]]; then
  prefix=$BGQ_BUILD_DIR/work
fi
BGQ_INSTALL_DIR=$prefix
AC_SUBST(BGQ_INSTALL_DIR)
AC_MSG_RESULT([  BGQ_INSTALL_DIR=$BGQ_INSTALL_DIR])


AC_ARG_ENABLE(fast_memcpy,
              AS_HELP_STRING([--enable-fast_memcpy=yes],[Use BGQ optimized memcpy/memset]),
              [BGQ_ENABLE_FAST_MEMCPY=$enableval],
              [BGQ_ENABLE_FAST_MEMCPY=no])
  
AC_SUBST(BGQ_ENABLE_FAST_MEMCPY)
AC_MSG_RESULT([  BGQ_ENABLE_FAST_MEMCPY=$BGQ_ENABLE_FAST_MEMCPY])

#
# Determine Operating System Type
#
#   We recognise four types:
#
#     x86:    "uname -s" returns "Linux", "uname -m" returns "i686"
#     x86_64: "uname -s" returns "Linux", "uname -m" returns "x86_64"
#     ppc:    "uname -s" returns "Linux", "uname -m" returns "ppc64"
#     aix:    "uname -s" returns "AIX"
#
#   Any other combination causes "configure" to exit with an error
#
# Action:
#   $BGQ_OS_TYPE set to one of ( x86 ppc aix )
#

bgq_system=`uname -s`
bgq_machine=`uname -m`

case ${bgq_system:=unknown}:${bgq_machine:=unknown} in
  Linux:i686 )
    BGQ_HOST_OS="x86" ;;
  Linux:x86_64 )
    BGQ_HOST_OS="x86_64" ;;
  Linux:ppc )
    BGQ_HOST_OS="ppc" ;;
  Linux:ppc64 )
    BGQ_HOST_OS="ppc" ;;
  AIX:* )
    BGQ_HOST_OS="aix";;
  * )
    AC_MSG_ERROR([Unrecognised system "$bgq_system" or machine "$bgq_machine"]);;
  esac

AC_SUBST(BGQ_HOST_OS)
AC_MSG_RESULT([  BGQ_HOST_OS=$BGQ_HOST_OS])

#
# Determine Location
#
#   We recognise the following locations
#
#     watson:    domain is "watson.ibm.com"
#     pok:       domain is "pok.ibm.com"
#     rchland:   domain is "rchland.ibm.com"
#     raleigh:   domain is "raleigh.ibm.com"
#     unknown:   everything else
#
# Action:
#    $bgq_location set to one of ( watson rchland raleigh none )
#

if [ [[ $BGQ_HOST_OS = "aix" ]] ] ; then
  bgq_domain=`hostname`
else
  bgq_domain=`hostname -d`
  fi

case $bgq_domain in
  *watson.ibm.com )
    bgq_location="watson" ;;
  *pok.ibm.com )
    bgq_location="watson" ;;
  *rchland.ibm.com )
    bgq_location="rchland" ;;
  *raleigh.ibm.com )
    bgq_location="raleigh" ;;
  * )
    bgq_location="unknown" ;;
  esac

AC_MSG_RESULT([  bgq_location=$bgq_location])

#
# Setup defaults (not including vhdl compile and sim)
#
#   At this point we only set up defaults, we don't check
#   that defaults exist or are accessible.  Checks happen
#   later.
#
#   Note: we set the generic defaults first, then override as needed
#
#   Note: for vhdl compile and simulation the CTE tools must be available
#     see http://w3vlsi.watson.ibm.com/CTE/sites.html for site locations
#
# Action:
#     $bgq_default_floor:       floor location
#     $bgq_default_gnu:         a floor which contains a gnu toolchain build, used for gnu only
#     $bgq_default_expat_path:  root of directory tree in which "expat" library is installed
#                               expat ".h" files should be installed in $bgq_default_expat_path/include
#                               expat libraries should be installed in $bgq_default_expat_path/lib
#

bgq_default_floor=not_defined
bgq_default_gnu=not_defined
bgq_default_ant_home=not_defined
bgq_java_home=/usr
bgq_default_expat_path=/usr
bgq_default_twinstar_dir=/gsa/yktgsa/projects/t/twinstar
bgq_default_twinstar_dd1_dir=/gsa/yktgsa/projects/t/twinstar_dd1
bgq_default_twinstar_dd2_dir=/gsa/yktgsa/projects/t/twinstar_dd2
bgq_default_mambo=/auto/BGQ/releases/floor-mambo
bgq_default_rpmversion=0
bgq_default_rpmrelease=0
bgq_default_rpmmodification=0
bgq_default_rpmdriver=0
bgq_default_rpmsuffix=


case $bgq_location:$BGQ_HOST_OS in
  watson:x86_64 )
    bgq_default_floor=/bgsys/bgq/drivers/x86_64.floor ;
    bgq_default_gnu=/bgsys/bgq/drivers/x86_64.floor  ;
    bgq_java_home=/usr/lib/jvm/java-1.6.0-ibm.x86_64 ;
    bgq_default_ant_home=/opt/apache-ant-1.7.1 ;
    ;;
  watson:x86 )
    bgq_default_floor=/bgsys/bgq/drivers/i686.floor  ;
    bgq_default_gnu=/bgsys/bgq/drivers/i686.floor  ;
    bgq_java_home=/opt/ibm/java-x86-60 ;
    bgq_default_ant_home=/opt/apache-ant-1.7.1 ;
    ;;
  raleigh:x86 )
    bgq_default_floor=_not_defined ;
    bgq_default_gnu=_not_defined ;
    ;;
  raleigh:x86_64 )
    bgq_default_floor=_not_defined ;
    bgq_default_gnu=_not_defined ;
    ;;
  watson:aix )
    bgq_default_floor=_not_defined
    bgq_default_gnu=_not_defined ;
    bgq_default_expat_path=/afs/watson.ibm.com/projects/B/bluegene/bgp/bpc2/tools/misc;
    ;;
  watson:ppc )
    bgq_default_floor=/bgsys/bgq/drivers/ppcfloor ;
    bgq_default_gnu=/bgsys/bgq/drivers/ppctoolchain ;
    bgq_java_home=/opt/ibm/java-ppc64-60 ;
    bgq_default_ant_home=/opt/apache-ant-1.8.1 ;
    bgq_default_mambo=/bgsys/drivers/ppc64.mambo ;
    ;;
  rchland:x86 )
    bgq_default_floor=$BGQ_INSTALL_DIR ;
    ;;
  rchland:ppc )
    if [[ `uname -r | grep el6 > /dev/null; echo $?` == 0 ]]; then
      bgq_default_floor=/bgsys/drivers/ppcfloor ;
      bgq_default_gnu=/bgsys/drivers/ppctoolchain ;
    else
      bgq_default_floor=/bgsys/bgq/drivers/ppcfloor ;
      bgq_default_gnu=/bgsys/bgq/drivers/ppctoolchain ;
    fi
    bgq_java_home=/usr/lib/jvm/java-1.6.0-ibm.ppc64 ;
    bgq_default_ant_home=/opt/apache-ant-1.7.1 ;
    bgq_default_mambo=/bgsys/drivers/ppc64.mambo ;
    ;;
  esac

AC_MSG_RESULT([  The following defaults are defined initially, they may be changed later])
AC_MSG_RESULT([    bgq_default_floor=$bgq_default_floor])
AC_MSG_RESULT([    bgq_default_gnu=$bgq_default_gnu])
AC_MSG_RESULT([    bgq_default_ant_home=$bgq_default_ant_home])
AC_MSG_RESULT([    bgq_java_home=$bgq_java_home])
AC_MSG_RESULT([    bgq_default_expat_path=$bgq_default_expat_path])
AC_MSG_RESULT([    bgq_default_twinstar_dir=$bgq_default_twinstar_dir])
AC_MSG_RESULT([    bgq_default_twinstar_dd1_dir=$bgq_default_twinstar_dd1_dir])
AC_MSG_RESULT([    bgq_default_twinstar_dd2_dir=$bgq_default_twinstar_dd2_dir])
AC_MSG_RESULT([    bgq_default_rpmversion=$bgq_default_rpmversion])
AC_MSG_RESULT([    bgq_default_rpmrelease=$bgq_default_rpmrelease])
AC_MSG_RESULT([    bgq_default_rpmmodification=$bgq_default_rpmmodification])
AC_MSG_RESULT([    bgq_default_rpmdriver=$bgq_default_rpmdriver])
AC_MSG_RESULT([    bgq_default_rpmsuffix=$bgq_default_rpmsuffix])


AC_ARG_WITH(gnu,
  AS_HELP_STRING([--with-gnu=<path>], [path to floor directory containing built gnu compilers]),
  [ export BGQGNU_DIR="$withval"; ],
  [ export BGQGNU_DIR="$bgq_default_gnu"; ])

AC_SUBST(BGQGNU_DIR)
AC_MSG_RESULT([  BGQGNU_DIR=$BGQGNU_DIR])

AC_ARG_WITH(expat,
	  AS_HELP_STRING([--with-expat=<path>],[path to install location of expat libraries and header files]),
         [ BGQ_EXPAT_PATH="$withval"; ],
         [ BGQ_EXPAT_PATH="$bgq_default_expat_path"; ])
AC_SUBST(BGQ_EXPAT_PATH)

AC_ARG_WITH(date,
          AS_HELP_STRING([--with-date=<date>],[for toolchain build path, date is embedded ]),
         [ bgq_gnu_date="$withval"; ],
         [ bgq_gnu_date=`date -u +%g%m%d`; ])
AC_SUBST(BGQ_DATE)

AC_ARG_WITH(gnu-install-dir,
          AS_HELP_STRING([--with-gnu-install-dir=<path for toolchain install>],[for toolchain build path ]),
         [ BGQ_GNU_INSTALL_DIR="$withval"; ],
         [ BGQ_GNU_INSTALL_DIR="/bgsys/drivers/toolchain/V1R2M2-$bgq_gnu_date"; ])
AC_SUBST(BGQ_GNU_INSTALL_DIR)


#
# Setup defaults for vhdl compile and simulation
#

bgq_fusion_release=R64_2
bgq_fusion_subpath=tools/fusion
bgq_ddx_version_default=dd2

bgq_enable_hwsim=no

bgq_default_dd1=not_defined
bgq_default_dd2=not_defined
bgq_cteprojpath_dd1=not_defined
bgq_cteprojpath_dd2=not_defined
bgq_ctedenalipath_dd1=not_defined
bgq_ctedenalipath_dd2=not_defined

bgq_ctepath=not_defined
bgq_ctetexsimpath=not_defined
bgq_cds_inst_dir=not_defined
bgq_cds_lib_path=not_defined
bgq_ibmeda_keyfile_path=not_defined


case $bgq_location:$BGQ_HOST_OS in
  watson:x86_64 )
    bgq_enable_hwsim=yes ;

    bgq_default_dd1=/bgq/sys/watson/x86dd1
    bgq_default_dd2=/bgq/sys/watson/x86dd2
    bgq_cteprojpath_dd1=/data/bgp/bpc ;
    bgq_cteprojpath_dd2=/data/bgp/bpc2 ;
    bgq_ctedenalipath_dd1=$bgq_cteprojpath_dd1/libraries/denali/denali ;
    bgq_ctedenalipath_dd2=$bgq_cteprojpath_dd2/libraries/denali/denali ;

    bgq_ctepath=/afs/watson.ibm.com/projects/vlsi/cte ;
    bgq_ctetexsimpath=/afs/eda.fishkill.ibm.com/ship/texsim/10.1 ;
    bgq_cds_inst_dir=/afs/watson.ibm.com/projects/vlsi/cte/tools/cds/IUS/5.7.s4 ;
    bgq_cds_lib_path=/lib:/afs/watson.ibm.com/projects/vlsi/cte/tools/cds/LDV/vol3/tools.lnx86/lib ;
    bgq_ibmeda_keyfile_path=/data/bgq/tools/etc/keyfile ;
    ;;


  watson:x86 )
    bgq_enable_hwsim=yes ;

# temporarly point this at bgp paths..
    bgq_default_dd1=/bgq/sys/watson/x86dd1
    bgq_default_dd2=/bgq/sys/watson/x86dd2
    bgq_cteprojpath_dd1=/data/bgp/bpc ;
    bgq_cteprojpath_dd2=/data/bgp/bpc2 ;
    bgq_ctedenalipath_dd1=$bgq_cteprojpath_dd1/libraries/denali/denali ;
    bgq_ctedenalipath_dd2=$bgq_cteprojpath_dd2/libraries/denali/denali ;

    bgq_ctepath=/afs/watson.ibm.com/projects/vlsi/cte ;
    bgq_ctetexsimpath=/afs/eda.fishkill.ibm.com/ship/texsim/10.1 ;
    bgq_cds_inst_dir=/afs/watson.ibm.com/projects/vlsi/cte/tools/cds/IUS/5.7.s4 ;
    bgq_cds_lib_path=/lib:/afs/watson.ibm.com/projects/vlsi/cte/tools/cds/LDV/vol3/tools.lnx86/lib ;
    bgq_ibmeda_keyfile_path=/data/bgq/tools/etc/keyfile ;
    ;;

  watson:aix )
    bgq_enable_hwsim=yes ;

    bgq_default_dd1=/.../watson.ibm.com/fs/projects/vlsi/bgq/bpc/simulation/models/aixdd1 ;
    bgq_default_dd2=/.../watson.ibm.com/fs/projects/vlsi/bgq/bpc/simulation/models/aixdd2 ;
    bgq_cteprojpath_dd1=/.../watson.ibm.com/fs/projects/vlsi/bgq/bpc ;
    bgq_cteprojpath_dd2=/.../watson.ibm.com/fs/projects/vlsi/bgq/bpc2 ;
    bgq_ctedenalipath_dd1=$bgq_cteprojpath_dd1/libraries/denali/denali ;
    bgq_ctedenalipath_dd2=$bgq_cteprojpath_dd2/libraries/denali/denali ;

    bgq_ctepath=/afs/watson.ibm.com/projects/vlsi/cte ;
    bgq_ctetexsimpath=/afs/eda.fishkill.ibm.com/ship/texsim/10.1 ;
    bgq_cds_inst_dir=/afs/watson.ibm.com/projects/vlsi/cte/tools/cds/IUS/5.7.s4 ;
    bgq_cds_lib_path=/lib:/afs/watson.ibm.com/projects/vlsi/cte/tools/cds/LDV/vol3/tools.lnx86/lib ;
    bgq_ibmeda_keyfile_path=/afs/watson/projects/E/eda/keyfile ;
    ;;

  raleigh:x86 )
    bgq_enable_hwsim=yes ;

    bgq_default_dd1=not_defined ;
    bgq_default_dd2=not_defined ;
    bgq_cteprojpath_dd1=/simBGp/bgq/bpc ;
    bgq_cteprojpath_dd2=/data/bgq/bpc2 ;
    bgq_ctedenalipath_dd1=$bgq_cteprojpath_dd1/libraries/denali/denali ;
    bgq_ctedenalipath_dd2=$bgq_cteprojpath_dd2/libraries/denali/denali ;

    bgq_ctepath=/afs/raleigh.ibm.com/cadtools/cte ;
    bgq_ctetexsimpath=/afs/eda.fishkill.ibm.com/ship/texsim/10.1 ;
    bgq_cds_inst_dir=/home/simfinit/IUS57.s3 ;
    bgq_cds_lib_path=/lib:/home/simfinit/IUS57.s3/tools.lnx86/lib ;
    bgq_ibmeda_keyfile_path=/data/bgq/tools/etc/keyfile ;
    ;;  

  raleigh:x86_64 )
    bgq_enable_hwsim=yes ;

    bgq_default_dd1=not_defined ;
    bgq_default_dd2=not_defined ;
    bgq_cteprojpath_dd1=/simBGp/bgq/bpc ;
    bgq_cteprojpath_dd2=/simBGp/bgq/bpc2.dd2 ;
    bgq_ctedenalipath_dd1=$bgq_cteprojpath_dd1/libraries/denali/denali64 ;
    bgq_ctedenalipath_dd2=$bgq_cteprojpath_dd2/libraries/denali/denali64 ;

    bgq_ctepath=/afs/raleigh.ibm.com/cadtools/cte ;
    bgq_ctetexsimpath=/afs/eda.fishkill.ibm.com/ship/texsim/10.1 ;
    bgq_cds_inst_dir=/home/simfinit/IUS57.s4 ;
    bgq_cds_lib_path=/lib:/home/simfinit/IUS57.s4/tools.lnx86/lib ;
    bgq_ibmeda_keyfile_path=/data/bgq/tools/etc/keyfile ;
    ;;

  esac

AC_MSG_RESULT([  The following defaults are defined initially, they may be changed later])
AC_MSG_RESULT([    bgq_fusion_release=$bgq_fusion_release])
AC_MSG_RESULT([    bgq_fusion_subpath=$bgq_fusion_subpath])
AC_MSG_RESULT([    bgq_enable_hwsim=$bgq_enable_hwsim])

AC_MSG_RESULT([    bgq_default_dd1=$bgq_default_dd1])
AC_MSG_RESULT([    bgq_default_dd2=$bgq_default_dd2])
AC_MSG_RESULT([    bgq_cteprojpath_dd1=$bgq_cteprojpath_dd1])
AC_MSG_RESULT([    bgq_cteprojpath_dd2=$bgq_cteprojpath_dd2])
AC_MSG_RESULT([    bgq_ctedenalipath_dd1=$bgq_ctedenalipath_dd1])
AC_MSG_RESULT([    bgq_ctedenalipath_dd2=$bgq_ctedenalipath_dd2])

AC_MSG_RESULT([    bgq_ctepath=$bgq_ctepath])
AC_MSG_RESULT([    bgq_ctetexsimpath=$bgq_ctetexsimpath])
AC_MSG_RESULT([    bgq_cds_inst_dir=$bgq_cds_inst_dir])
AC_MSG_RESULT([    bgq_cds_lib_path=$bgq_cds_lib_path])
AC_MSG_RESULT([    bgq_ibmeda_keyfile_path=$bgq_ibmeda_keyfile_path])


# 
# Now determine if VHDL simulation support is wanted and available
#

AC_MSG_NOTICE([Testing if VHDL simulation is wanted and available])


#
# This section makes the basic decision whether VHDL compile / sim support is needed.
# The variable BGQ_ENABLE_HWSIM is used to register whether VHDL support is needed.
# The default setting for this variable is location dependent (see previous section).
# The default can be overridden by a configure option: --enable-hwsim
# Additionally, if the CTE tools directory is not available, BGQ_ENABLE_HWSIM
# is forced to "no".
#

AC_ARG_ENABLE(hwsim,
              AS_HELP_STRING([--enable-hwsim=yes],[include configuration to build vhdl simulation code.]),
              [BGQ_ENABLE_HWSIM=$enableval],
              [BGQ_ENABLE_HWSIM=$bgq_enable_hwsim])

AC_ARG_ENABLE(dd1,
              AS_HELP_STRING([--enable-dd1=yes],[configure to use dd1 ddx.]),
              [BGQ_DDX_VERSION=dd1],
              [BGQ_DDX_VERSION=$bgq_ddx_version_default])

AC_ARG_WITH(ctepath,
            AS_HELP_STRING([--with-ctepath],[path to cte tools, only useful if vhdl support is enabled.]),
            [CTEPATH=$withval],
	    [CTEPATH=$bgq_ctepath])

AC_ARG_WITH(twinstarpath,
            AS_HELP_STRING([--with-twinstarpath],[path to twinstar release]),
            [TWINSTARPATH=$withval],
	    [TWINSTARPATH=$bgq_default_twinstar_dir])

TWINSTARPATH_DD1=${TWINSTARPATH}_dd1
TWINSTARPATH_DD2=${TWINSTARPATH}_dd2


if [[[ $BGQ_ENABLE_HWSIM = yes ]]] ; then

  if [[[ $BGQ_DDX_VERSION = "dd1" ]]] ; then
    bgq_default_ddx=$bgq_default_dd1
    bgq_cteprojpath=$bgq_cteprojpath_dd1
    bgq_ctedenalipath=$bgq_ctedenalipath_dd1
  elif [[[ $BGQ_DDX_VERSION = "dd2" ]]] ; then
    bgq_default_ddx=$bgq_default_dd2
    bgq_cteprojpath=$bgq_cteprojpath_dd2
    bgq_ctedenalipath=$bgq_ctedenalipath_dd2
  else
    AC_MSG_WARN([Invalid DDX version $BGQ_DDX_VERSION specified, disabling hwsim build])
    BGQ_ENABLE_HWSIM=no
    fi

  if [[[ ! -d $CTEPATH ]]] ; then
    BGQ_ENABLE_HWSIM=no    
    AC_MSG_WARN([CTE tools path $CTEPATH is not accessible, disabling hwsim build])
    CTEPATH=not_defined
    fi

else

  CTEPATH=not_defined

fi



#
# Remaining variables for hwsim configuration
#

if [[[ $BGQ_ENABLE_HWSIM = "yes" ]]] ; then 
  FUSION_RELEASE=$bgq_fusion_release
  CTEFUSIONPATH=$CTEPATH/$bgq_fusion_subpath
  CTESIMARAMABASE=$CTEPATH/tools/simarama
  CTETEXSIMPATH=$bgq_ctetexsimpath
  CTEPROJPATH=$bgq_cteprojpath
  CDS_INST_DIR=$bgq_cds_inst_dir
  CDS_LIB_PATH=$bgq_cds_lib_path
  CTEDENALIPATH=$bgq_ctedenalipath
  IBMEDA_KEYFILE_PATH=$bgq_ibmeda_keyfile_path
  PONY_SRC=$CTEPATH/tools/pony2/PT9_0/src
  PONY_LIB=$CTEPATH/tools/pony2/PT9_0/build/fusion2/$FUSION_RELEASE/dbg/lib/
  if [ [[ $BGQ_HOST_OS = "aix" ]] ] ; then
     CTESIMARAMAPATH=$CTEPATH/tools/simarama/bin.64
  else
     CTESIMARAMAPATH=$CTEPATH/tools/simarama/bin
  fi








else

  FUSION_RELEASE=not_defined
  CTEFUSIONPATH=not_defined
  CTESIMARAMABASE=not_defined
  CTESIMARAMAPATH=not_defined
  CTETEXSIMPATH=not_defined
  CTEPROJPATH=not_defined
  CDS_INST_DIR=not_defined
  CDS_LIB_PATH=not_defined
  CTEDENALIPATH=not_defined
  IBMEDA_KEYFILE_PATH=not_defined
  PONY_SRC=not_defined
  PONY_LIB=not_defined


  fi

#
# Report hwsim configuration
#

AC_SUBST(FUSION_RELEASE)
AC_SUBST(CTEFUSIONPATH)

AC_SUBST(BGQ_ENABLE_HWSIM)
AC_SUBST(BGQ_DDX_VERSION)
AC_SUBST(CTEPATH)
AC_SUBST(CTETEXSIMPATH)
AC_SUBST(CTESIMARAMABASE)
AC_SUBST(CTESIMARAMAPATH)
AC_SUBST(CTEPROJPATH)
AC_SUBST(CDS_INST_DIR)
AC_SUBST(CDS_LIB_PATH)
AC_SUBST(CTEDENALIPATH)
AC_SUBST(IBMEDA_KEYFILE_PATH)
AC_SUBST(PONY_SRC)
AC_SUBST(PONY_LIB)
AC_SUBST(TWINSTARPATH)
AC_SUBST(TWINSTARPATH_DD1)
AC_SUBST(TWINSTARPATH_DD2)

if [[[ $BGQ_ENABLE_HWSIM = "yes" ]]] ; then
  AC_MSG_RESULT([  VHDL Compile and Simulation enabled, paths configured as follows:])
else
  AC_MSG_RESULT([  VHDL Compile and Simulation disabled, paths configured as follows:])
  fi

AC_MSG_RESULT([    BGQ_ENABLE_HWSIM=$BGQ_ENABLE_HWSIM])
AC_MSG_RESULT([    CTEPATH=$CTEPATH])
AC_MSG_RESULT([    TWINSTARPATH=$TWINSTARPATH])
AC_MSG_RESULT([    FUSION_RELEASE=$FUSION_RELEASE])
AC_MSG_RESULT([    CTEFUSIONPATH=$CTEFUSIONPATH])
AC_MSG_RESULT([    CTETEXSIMPATH=$CTETEXSIMPATH])
AC_MSG_RESULT([    CTESIMARAMABASE=$CTESIMARAMABASE])
AC_MSG_RESULT([    CTESIMARAMAPATH=$CTESIMARAMAPATH])
AC_MSG_RESULT([    CTEPROJPATH=$CTEPROJPATH])
AC_MSG_RESULT([    CDS_INST_DIR=$CDS_INST_DIR])
AC_MSG_RESULT([    CDS_INST_DIR=$CDS_INST_DIR])
AC_MSG_RESULT([    CTEDENALIPATH=$CTEDENALIPATH])
AC_MSG_RESULT([    IBMEDA_KEYFILE_PATH=$IBMEDA_KEYFILE_PATH])
AC_MSG_RESULT([    PONY_SRC=$PONY_SRC])
AC_MSG_RESULT([    PONY_LIB=$PONY_LIB])


if [[[ -d $bgq_default_floor ]]] ; then
  bgq_default_floor_physical=`cd $bgq_default_floor; pwd -P`
else
  bgq_default_floor_physical=not_defined
  fi

AC_MSG_RESULT([   bgq_default_floor_physical=$bgq_default_floor_physical])

AC_ARG_WITH(floor-symlinks,
  AS_HELP_STRING([--with-floor-symlinks],[create symlinks to the floor directories]),
  [ export FLOOR_SYMLINKS="$withval"; ],
  [ export FLOOR_SYMLINKS="NO"; ])

AC_ARG_WITH(floor,
  AS_HELP_STRING([--with-floor=<path>],[path to floor release]),
  [ export BGQ_FLOOR_DIR="$withval"; ],
  [ export BGQ_FLOOR_DIR="$bgq_default_floor_physical"; ])

if [[[ $BGQ_FLOOR_DIR != $bgq_default_floor_physical ]]]; then
  if [[[ -d $BGQ_FLOOR_DIR ]]] ; then
    if [[[ $FLOOR_SYMLINKS = NO ]]]; then
      BGQ_FLOOR_DIR=`cd $BGQ_FLOOR_DIR; pwd -P`
    fi
  else
    if [[[ $BGQ_FLOOR_DIR != none ]]]; then
        BGQ_FLOOR_DIR=not_defined
    fi
  fi
fi

#-----------------------------------------------
# Need to set-up some variables for Linux
#

# BGQ_LINUX_OS_PATH set-up
# Need to use some intelligence here now that we have moved to creating a new BG/Q Linux Distro
# for each driver.  First we check to see if we appear to be in a sandbox (install dir ends with work).
# If we are we create a sandbox distro using the user's name and a date.  Otherwise we should be
# in a driver build and we want to use the name of the driver for the distro.  If the user
# doesn't like either of these options they can still override with --with-linux-os.
bgq_distro_default=RHEL6.5
AC_ARG_WITH(distro,
        AS_HELP_STRING([--with-distro=<name>], [RHEL6.4]),
        [ BGQ_DISTRO="$withval"; ],
        [ BGQ_DISTRO="$bgq_distro_default"])
AC_SUBST(BGQ_DISTRO)
AC_MSG_RESULT([  BGQ_DISTRO=$BGQ_DISTRO])

INSTALL_SUBDIR=`/bin/echo $BGQ_INSTALL_DIR | awk 'BEGIN{FS="/"} ; {print $NF}'`
if [[[ $INSTALL_SUBDIR == "work" ]]] ; then
	USER_DISTRO=`whoami`_`date +%y%m%d`
	bgq_linux_os_default=/bgsys/linux/"$BGQ_DISTRO"_$USER_DISTRO
else
	DRIVER=`/bin/echo $BGQ_INSTALL_DIR | awk 'BEGIN{FS="/"} ; {print $(NF-1)}'`
	bgq_linux_os_default=/bgsys/linux/"$BGQ_DISTRO"_$DRIVER
fi
AC_ARG_WITH(linux-os,
  AS_HELP_STRING([--with-linux-os=<path>], [path to linux OS root directory]),
  [ BGQ_LINUX_OS_PATH="$withval"; ],
  [ BGQ_LINUX_OS_PATH="$bgq_linux_os_default"])
AC_SUBST(BGQ_LINUX_OS_PATH)
AC_MSG_RESULT([  BGQ_LINUX_OS_PATH=$BGQ_LINUX_OS_PATH])

bgq_distrofs_base_path_default=/bgsys/linux/"$BGQ_DISTRO"_base
AC_ARG_WITH(base-distro,
  AS_HELP_STRING([--with-base-distro=<path>], [path to base ION distribution]),
  [ BGQ_DISTROFS_BASE_PATH="$withval"; ],
  [ BGQ_DISTROFS_BASE_PATH="$bgq_distrofs_base_path_default"])
AC_SUBST(BGQ_DISTROFS_BASE_PATH)
AC_MSG_RESULT([  BGQ_DISTROFS_BASE_PATH=$BGQ_DISTROFS_BASE_PATH])

bgq_distro_repo_path_default=/install/redhat/"$BGQ_DISTRO"-ppc/Packages
AC_ARG_WITH(distro_repo_path,
	AS_HELP_STRING([--with-distro-repo-path=<path>], [path to ION distribution package repository]),
	[ BGQ_DISTRO_REPO_PATH="$withval"; ],
	[ BGQ_DISTRO_REPO_PATH="$bgq_distro_repo_path_default"])
AC_SUBST(BGQ_DISTRO_REPO_PATH)
AC_MSG_RESULT([  BGQ_DISTRO_REPO_PATH=$BGQ_DISTRO_REPO_PATH])
	

#
# RB: if we are on a ppc system use the native headers
#     as per Mike Nelson (apparently there was some LLNL request about this).
if [ [[ $BGQ_HOST_OS = "ppc" ]] ] ; then
  bgq_linux_headers_default=/usr
else
  bgq_linux_headers_default=$BGQ_INSTALL_DIR/linux
fi


# BGQ_LINUX_HEADERS_PATH set up
AC_ARG_WITH(linux-headers,
  AS_HELP_STRING([--with-linux-headers=<path>], [path to linux headers to use with build]),
  [ BGQ_LINUX_HEADERS_PATH="$withval"; ],
  [ BGQ_LINUX_HEADERS_PATH="$bgq_linux_headers_default"])
AC_SUBST(BGQ_LINUX_HEADERS_PATH)
AC_MSG_RESULT([  BGQ_LINUX_HEADERS_PATH=$BGQ_LINUX_HEADERS_PATH])

# BGQ_MCP_LIB set up
AC_ARG_WITH(mcp-lib,
  AS_HELP_STRING([--with-mcp-lib=<path>], [path to where mcp is installed]),
  [ BGQ_MCP_LIB="$withval"; ],
  [ BGQ_MCP_LIB="/usr/local"])
AC_SUBST(BGQ_MCP_LIB)
AC_MSG_RESULT([  BGQ_MCP_LIB=$BGQ_MCP_LIB])

# BGQ_LINUX_KERNEL_MAJOR_VERSION set-up
bgq_linux_kernel_major_version_default=2.6.32
AC_ARG_WITH(linux-kernel-major-version,
  AS_HELP_STRING([--with-linux-kernel-major-version=<version>], [Linux kernel version (e.g. 2.6.32)]),
  [ BGQ_LINUX_KERNEL_MAJOR_VERSION="$withval"; ],
  [ BGQ_LINUX_KERNEL_MAJOR_VERSION="$bgq_linux_kernel_major_version_default"])
AC_SUBST(BGQ_LINUX_KERNEL_MAJOR_VERSION)
AC_MSG_RESULT([  BGQ_LINUX_KERNEL_MAJOR_VERSION=$BGQ_LINUX_KERNEL_MAJOR_VERSION])

# BGQ_LINUX_KERNEL_RELEASE set-up
bgq_linux_kernel_release_default=431.1.1.bgq.el6
AC_ARG_WITH(linux-kernel-release,
  AS_HELP_STRING([--with-linux-kernel-release=<release>], [Linux kernel release (e.g. 358.11.1.bgq.el6)]),
  [ BGQ_LINUX_KERNEL_RELEASE="$withval"; ],
  [ BGQ_LINUX_KERNEL_RELEASE="$bgq_linux_kernel_release_default"])
AC_SUBST(BGQ_LINUX_KERNEL_RELEASE)
AC_MSG_RESULT([  BGQ_LINUX_KERNEL_RELEASE=$BGQ_LINUX_KERNEL_RELEASE])

# BGQ_LINUX_KERNEL_BUILDID set-up
bgq_linux_kernel_buildid_default=_V1R2M2_DEV0
AC_ARG_WITH(linux-kernel-buildid,
  AS_HELP_STRING([--with-linux-kernel-buildid=<buildid>], [Linux kernel buildid (e.g. _V1R2M1_DEV1)]),
  [ BGQ_LINUX_KERNEL_BUILDID="$withval"; ],
  [ BGQ_LINUX_KERNEL_BUILDID="$bgq_linux_kernel_buildid_default"])
AC_SUBST(BGQ_LINUX_KERNEL_BUILDID)
AC_MSG_RESULT([  BGQ_LINUX_KERNEL_BUILDID=$BGQ_LINUX_KERNEL_BUILDID])

# BGQ_LINUX_KERNEL_ARCH set-up
bgq_linux_kernel_arch_default=ppc64
AC_ARG_WITH(linux-kernel-arch,
  AS_HELP_STRING([--with-linux-kernel-arch=<arch>], [Linux kernel arch (e.g. ppc64)]),
  [ BGQ_LINUX_KERNEL_ARCH="$withval"; ],
  [ BGQ_LINUX_KERNEL_ARCH="$bgq_linux_kernel_arch_default"])
AC_SUBST(BGQ_LINUX_KERNEL_ARCH)
AC_MSG_RESULT([  BGQ_LINUX_KERNEL_ARCH=$BGQ_LINUX_KERNEL_ARCH])

BGQ_LINUX_KERNEL_VERSION=$BGQ_LINUX_KERNEL_MAJOR_VERSION-$BGQ_LINUX_KERNEL_RELEASE.$BGQ_LINUX_KERNEL_ARCH
AC_SUBST(BGQ_LINUX_KERNEL_VERSION)
AC_MSG_RESULT([  BGQ_LINUX_KERNEL_VERSION=$BGQ_LINUX_KERNEL_VERSION])

#-----------------------------------------------

#
# DDX location and tag, 
#   this is a floor in which ddx has been built
#

if [[[ $BGQ_ENABLE_HWSIM = yes ]]] ; then 

  if [[[ -d $bgq_default_ddx ]]] ; then
    bgq_default_ddx_physical=`cd $bgq_default_ddx; pwd -P`
  else
    bgq_default_ddx_physical=not_defined
    fi

  AC_ARG_WITH(ddx,
    AS_HELP_STRING([--with-ddx=<path>],[path to floor directory containing prebuilt ddx compilers]),
    [ export BGQDDX_DIR="$withval"; ],
    [ export BGQDDX_DIR="$bgq_default_ddx_physical"; ])



else

  BGQDDX_DIR=not_defined

  fi

AC_SUBST(BGQDDX_DIR)
AC_MSG_RESULT([  BGQDDX_DIR=$BGQDDX_DIR])


if [[[ -e $BGQ_FLOOR_DIR/svn_revision ]]] ; then
  BGQ_FLOOR_TAG=`grep Revision $BGQ_FLOOR_DIR/svn_revision | awk '{print $2}'`
else
  BGQ_FLOOR_TAG=not_defined
  fi


AC_SUBST(BGQ_FLOOR_TAG)
AC_MSG_RESULT([  BGQ_FLOOR_TAG=$BGQ_FLOOR_TAG])


#----------------------------
# Select target architecture
#
AC_ARG_WITH(target,
  AS_HELP_STRING([--with-target=<target>],[select machine architecture (default=A2)]),
  [ BGQ_CROSS_TARGET_NAME="$withval"; ],
  [ BGQ_CROSS_TARGET_NAME="A2"; ])

if [[[ $BGQ_CROSS_TARGET_NAME = "A2" ]]] ; then
  BGQ_CROSS_TARGET_ARCH=powerpc64-bgq-linux
else if [[[ $BGQ_CROSS_TARGET_NAME = "undefined" ]]] ; then
  echo "Error: please specify --with-target=A2"
  exit 1
else
  echo "Error: unrecognized --with-target: $BGQ_CROSS_TARGET_NAME"
  exit 1
fi
fi
AC_SUBST(BGQ_CROSS_TARGET_NAME)
AC_SUBST(BGQ_CROSS_TARGET_ARCH)
#
#----------------------------

#----------------------------
# Select mambo simulator to use
#
AC_ARG_WITH(mambo,
  AS_HELP_STRING([--with-mambo=<path>],[path to floor directory containing prebuilt mambo simulator]),
  [ BGQ_MAMBO_DIR="$withval"; ],
  [ BGQ_MAMBO_DIR="$bgq_default_mambo"; ])

AC_SUBST(BGQ_MAMBO_DIR)
#
#----------------------------


if [[[ -e ./gnu ]]]; then
    BGQ_CROSS_TOOLS_DIR=$BGQ_INSTALL_DIR/gnu-linux
else
if [[[ -e $BGQGNU_DIR/gnu-linux ]]]; then
    BGQ_CROSS_TOOLS_DIR=$BGQGNU_DIR/gnu-linux
else
    BGQ_CROSS_TOOLS_DIR=$BGQ_FLOOR_DIR/gnu-linux
fi
fi

AC_ARG_WITH(beam,
  AS_HELP_STRING([--with-beam=<path>],[path to prebuilt BEAM compiler]),
  [ BEAM_BIN="$withval/bin"; ],
  [ BEAM_BIN="/bgsys/bgq/beam/beam-3.5.1/bin"; ])

AC_ARG_WITH(beam-outputdir,
  AS_HELP_STRING([--with-beam-outputdir=<path>],[output directory for BEAM static analysis]),
  [ BEAM_OUTPUT="$withval"; ],
  [ BEAM_OUTPUT="stdout"; ])

if [[[ -d $BEAM_BIN ]]] ; then
   echo BEAM installation at $BEAM_BIN
   echo Configuring BEAM
   WITH_BEAM=1
   rm -rf $BGQ_INSTALL_DIR/beam
   mkdir -p $BGQ_INSTALL_DIR/beam
   cp scripts/bgq_beam_parms.tcl $BGQ_INSTALL_DIR/beam/.
   if [[[ -e $BGQ_CROSS_TOOLS_DIR/bin/$BGQ_CROSS_TARGET_ARCH\-gcc ]]] ; then
      $BEAM_BIN/beam_configure --force -c   $BGQ_CROSS_TOOLS_DIR/bin/$BGQ_CROSS_TARGET_ARCH\-gcc -o $BGQ_INSTALL_DIR/beam/a2compiler_c_config
      $BEAM_BIN/beam_configure --force -cpp $BGQ_CROSS_TOOLS_DIR/bin/$BGQ_CROSS_TARGET_ARCH\-g++ -o $BGQ_INSTALL_DIR/beam/a2compiler_cpp_config
   else
      cp scripts/a2compiler_c_config   $BGQ_INSTALL_DIR/beam/a2compiler_c_config
      cp scripts/a2compiler_cpp_config $BGQ_INSTALL_DIR/beam/a2compiler_cpp_config
   fi
   $BEAM_BIN/beam_configure --force -c   gcc -o $BGQ_INSTALL_DIR/beam/sncompiler_c_config
   $BEAM_BIN/beam_configure --force -cpp g++ -o $BGQ_INSTALL_DIR/beam/sncompiler_cpp_config
else
   echo BEAM installation not found at $BEAM_BIN
fi
AC_SUBST(BEAM_BIN)
AC_SUBST(BEAM_OUTPUT)
AC_SUBST(WITH_BEAM)
AC_MSG_RESULT([  WITH_BEAM=$WITH_BEAM])
AC_MSG_RESULT([  BEAM_BIN=$BEAM_BIN])






AC_SUBST(BGQ_FLOOR_DIR)
AC_SUBST(BGQ_FLOOR_TAG)
AC_SUBST(BGQ_CROSS_TOOLS_DIR)


AC_MSG_RESULT([  BGQ_FLOOR_DIR=$BGQ_FLOOR_DIR])
AC_MSG_RESULT([  BGQ_FLOOR_TAG=$BGQ_FLOOR_TAG])
AC_MSG_RESULT([  BGQ_CROSS_TOOLS_DIR=$BGQ_CROSS_TOOLS_DIR])




###############################################################################
###############################################################################
###############################################################################
###############################################################################

# Checks for programs.

AC_PROG_CXX
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LEX
AC_PROG_LN_S

AC_CHECK_PROG(CP_F, cp, cp -f)
AC_CHECK_PROG(RM_F, rm, rm -f)
AC_CHECK_PROG(MV, mv, mv)
AC_CHECK_PROG(AR, ar, ar)
AC_CHECK_PROG(LN, ln, ln)
AC_CHECK_PROG(MKDIR,mkdir,mkdir)
AC_CHECK_PROG(TEE,tee,tee)
AC_CHECK_PROG(TOUCH,touch,touch)


#----------------------------
# Check for RHEL6
#
RHEL6=no
if [[[ `uname -r | grep el6 > /dev/null; echo $?` == 0 ]]]; then
  RHEL6=yes
fi

AC_SUBST(RHEL6)

OS_ARCH="$bgq_machine"
AC_SUBST(OS_ARCH)

#----------------------------
# Set up Ant
#
HAVE_ANT=no

AC_ARG_WITH(ant_home,
  AS_HELP_STRING([--with-ant_home=<path>],[path to Ant home]),
  [ ANT_HOME="$withval"; ],
  [ ANT_HOME="$bgq_default_ant_home"; ])

ANT="$ANT_HOME/bin/ant"

if [[[ "$RHEL6" == "yes" ]]]; then
  ANT_HOME="/usr/share/ant"
  ANT="/usr/bin/ant"
fi

if [[[ -e $ANT ]]]; then
  HAVE_ANT=yes
fi

AC_SUBST(HAVE_ANT)
AC_SUBST(ANT_HOME)
AC_SUBST(ANT)
#
#----------------------------


#----------------------------
# Set up Java
#
JAVA_BUILDS=no
HAVE_JAVA6=no

AC_ARG_WITH(java_home,
  AS_HELP_STRING([--with-java_home=<path>],[path to Java JDK]),
  [ JAVA_HOME="$withval"; ],
  [ JAVA_HOME="$bgq_java_home"; ])

AC_SUBST(JAVA_HOME)
AC_PATH_PROG(JAVA, java, /bin/false , $JAVA_HOME/bin)
AC_PATH_PROG(JAVAC, javac, /bin/false , $JAVA_HOME/bin)
AC_PATH_PROG(JAR, jar, /bin/false , $JAVA_HOME/bin)

if [[[ -e $JAVAC ]]]; then
  HAVE_JAVA6=yes
fi

if [[[ ($HAVE_JAVA6 = yes) && ($HAVE_ANT = yes) ]]]; then
  JAVA_BUILDS=yes
fi

AC_SUBST(HAVE_JAVA6)
AC_SUBST(JAVA_BUILDS)
#
#----------------------------

#----------------------------
# Select debug flags
# 
AC_ARG_WITH(debug,
  AS_HELP_STRING([--with-debug=flags>],[flags to enable debugging symbols]),
  [ DEBUG_FLAGS="$withval" ],
  [ DEBUG_FLAGS="-g" ])

AC_SUBST(DEBUG_FLAGS)

#
#----------------------------

#----------------------------
# Select db2 paths
# 
AC_ARG_WITH(db,
  AS_HELP_STRING([--with-db=yes|no>],[specify to use the DB code]),
  [ BGQDB="$withval" ],
  [ BGQDB="no" ])
AC_SUBST(BGQDB)

AC_ARG_WITH(db2path,
  AS_HELP_STRING([--with-db2path=<path>],[specify path of DB2 client install]),
  [ BGQDB2PATH="$withval" ],
  [ BGQDB2PATH="/dbhome/bgqsysdb/sqllib" ])
AC_SUBST(BGQDB2PATH)

AC_ARG_WITH(db2inst,
  AS_HELP_STRING([--with-db2inst=<db2instance>],[specify DB2 instance]),
  [ BGQDB2INSTANCE="$withval" ],
  [ BGQDB2INSTANCE="bgqsysdb" ])
AC_SUBST(BGQDB2INSTANCE)

AC_ARG_WITH(db-name,
  AS_HELP_STRING([--with-db-name=<dbname>],[specify DB2 database name]),
  [ BGQDB_DBNAME="$withval" ],
  [ BGQDB_DBNAME="BGDB0SCR" ])
AC_SUBST(BGQDB_DBNAME)

AC_ARG_WITH(db-user,
  AS_HELP_STRING([--with-db-user=<dbuser>],[specify DB2 database username]),
  [ BGQDB_DBUSER="$withval" ],
  [ BGQDB_DBUSER="bgqsysdb" ])
AC_SUBST(BGQDB_DBUSER)

AC_ARG_WITH(db-password,
  AS_HELP_STRING([--with-db-password=<dbpwd>],[specify DB2 database password]),
  [ BGQDB_DBPWD="$withval" ],
  [ BGQDB_DBPWD="db24bgq" ])
AC_SUBST(BGQDB_DBPWD)

#----------------------------
# Select docbook paths
# 
AC_ARG_WITH(docbook-path,
  AS_HELP_STRING([--with-docbook-path=<path|no>],[specify to use docbook]),
  [ DOCBOOK="$withval" ],
  [ DOCBOOK="/usr/share/apps/ksgmltools2/docbook/xsl/manpages/docbook.xsl" ])
AC_SUBST(DOCBOOK)


#---------------------------
# Configure build of control system simulator on x86_64
AC_ARG_WITH(control-system-simulation,
  AS_HELP_STRING([--with-control-system-simulation],[Configure build of control system simulator on x86_64]),
  [ BGQ_CONTROL_SYSTEM_SIMULATION="yes" ],
  [ BGQ_CONTROL_SYSTEM_SIMULATION="no" ])
AC_SUBST(BGQ_CONTROL_SYSTEM_SIMULATION)


#----------------------------
# Setup RPMs
#	
AC_ARG_WITH(rpmversion,
  AS_HELP_STRING([--with-rpmversion=<version>],[version setting for an rpm]),
  [ BGQ_RPMVERSION="$withval"; ],
  [ BGQ_RPMVERSION="$bgq_default_rpmversion"; ])

AC_SUBST(BGQ_RPMVERSION)

AC_ARG_WITH(rpmrelease,
  AS_HELP_STRING([--with-rpmrelease=<version>],[version setting for an rpm]),
  [ BGQ_RPMRELEASE="$withval"; ],
  [ BGQ_RPMRELEASE="$bgq_default_rpmrelease"; ])

AC_SUBST(BGQ_RPMRELEASE)

AC_ARG_WITH(rpmmodification,
  AS_HELP_STRING([--with-rpmmodification=<version>],[version setting for an rpm]),
  [ BGQ_RPMMODIFICATION="$withval"; ],
  [ BGQ_RPMMODIFICATION="$bgq_default_rpmmodification"; ])

AC_SUBST(BGQ_RPMMODIFICATION)

AC_ARG_WITH(rpmdriver,
  AS_HELP_STRING([--with-rpmdriver=<version>],[version setting for an rpm]),
  [ BGQ_RPMDRIVER="$withval"; ],
  [ BGQ_RPMDRIVER="$bgq_default_rpmdriver"; ])

AC_SUBST(BGQ_RPMDRIVER)

AC_ARG_WITH(rpmsuffix,
  AS_HELP_STRING([--with-rpmsuffix=<version>],[version setting for an rpm]),
  [ BGQ_RPMSUFFIX="$withval"; ],
  [ BGQ_RPMSUFFIX="$bgq_default_rpmsuffix"; ])

AC_SUBST(BGQ_RPMSUFFIX)


AC_ARG_WITH(packaging-gpg-name,
  AS_HELP_STRING([--with-packaging-gpg-name=<keyname>],[GPG key name for signing the RPMs]),
  [ PACKAGING_GPG_NAME="$withval"; ],
  [ PACKAGING_GPG_NAME="no"; ])

AC_SUBST(PACKAGING_GPG_NAME)


AC_ARG_WITH(rpm-install-dir,
  AS_HELP_STRING([--with-rpm-install-dir=<directory>],[Location to install RPMs]),
  [ RPM_INSTALL_DIR="$withval"; ],
  [ RPM_INSTALL_DIR="no"; ])

AC_SUBST(RPM_INSTALL_DIR)


# If --with-rpm-install-dir AND --with-efix-install-dir, then when will generate the current efix to the location.
# Rather than building all RPMs.
# See bgq/build/packaging.
AC_ARG_WITH(efix-install-dir,
  AS_HELP_STRING([--with-efix-install-dir=<directory>],[Location to install built efixes]),
  [ EFIX_INSTALL_DIR="$withval"; ],
  [ EFIX_INSTALL_DIR="no"; ])

AC_SUBST(EFIX_INSTALL_DIR)


#----------------------------
# Navigator configuration for where to get Dojo and the JSDoc toolkit.
AC_ARG_WITH(dojo,
  AS_HELP_STRING([--with-dojo=<path-to-zip-file>],[Path to zip file containing Dojo.]),
  [ DOJO="$withval" ],
  [ DOJO="dojo/InternalUseOnly/dojo.tgz" ])
AC_SUBST(DOJO)

AC_ARG_WITH(jsdoc,
  AS_HELP_STRING([--with-jsdoc=<path-to-zip-file>],[Path to JSDoc toolkit zip file.]),
  [ JSDOC_ZIP="$withval" ],
  [ JSDOC_ZIP="jsdoc_toolkit-2.4.0.zip" ])
AC_SUBST(JSDOC_ZIP)


#----------------------------
# Revision. This goes into a header file so that servers can print out a revision number.
# By default, the revision will be calculated using svn info.
# If you don't have svn info available, specify it manually using this option.
# This must be a number.
AC_ARG_WITH(revision,
  AS_HELP_STRING([--with-revision=<number>],[Revision number]),
  [ BGQ_REVISION="$withval" ],
  [ BGQ_REVISION="auto" ])
AC_SUBST(BGQ_REVISION)


#----------------------------
# Open-source tarball install location.
# If this is set then the open source tarball will be created where specified.
# If this is not specified then the open source tarball will not be created.
AC_ARG_WITH(open-source-install,
  AS_HELP_STRING([--with-open-source-install=<filename>],[Open-source output directory]),
  [ BGQ_OPEN_SOURCE_INSTALL="$withval" ],
  [ BGQ_OPEN_SOURCE_INSTALL="no" ])
AC_SUBST(BGQ_OPEN_SOURCE_INSTALL)


#
#----------------------------

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Checks for files that contain configuration variables.


if [[[ -e scripts/hwsim_env.sh.in ]]]; then
   MAKE_HWSIM_ENV=scripts/hwsim_env.sh
fi
if [[[ -e scripts/runfctest.sh.in ]]]; then
   MAKE_RUNFCTEST=scripts/runfctest.sh
fi
if [[[ -e scripts/RunMambo.sh.in ]]]; then
   MAKE_RUN_MAMBO=scripts/RunMambo.sh
fi
if [[[ -e scripts/RunMamboRegression.sh.in ]]]; then
   MAKE_RUN_MAMBO_REGRESSION=scripts/RunMamboRegression.sh
fi
if [[[ -e scripts/RunMmcsLite.sh.in ]]]; then
   MAKE_RUN_MMCS_LITE=scripts/RunMmcsLite.sh
fi
if [[[ -e scripts/MmcsLiteSetup.sh.in ]]]; then
   MAKE_MMCS_LITE_SETUP=scripts/MmcsLiteSetup.sh
fi
if [[[ -e scripts/comm_csets.sh.in ]]]; then
   MAKE_COMM_CSETS=scripts/comm_csets.sh
fi
if [[[ -e scripts/comm_efix.sh.in ]]]; then
   MAKE_COMM_EFIX=scripts/comm_efix.sh
fi
if [[[ -e ddx/Make.ddx.rules.in ]]]; then
   MAKE_DDX_RULES=ddx/Make.ddx.rules
fi

AC_CONFIG_FILES([Make.rules:scripts/Make.rules.in
                 $MAKE_HWSIM_ENV
                 $MAKE_RUNFCTEST
                 $MAKE_RUN_MAMBO
                 $MAKE_RUN_MAMBO_REGRESSION
                 $MAKE_RUN_MMCS_LITE
                 $MAKE_MMCS_LITE_SETUP
                 $MAKE_COMM_CSETS
                 $MAKE_COMM_EFIX
		 $MAKE_DDX_RULES
               ])

# Set configuration variables.

ln -sf $BGQ_BUILD_DIR/scripts/Makefile.main $BGQ_BUILD_DIR/Makefile; [[ $? -ne 0 ]] && AC_MSG_ERROR([command failed ])


###############################################################################
###############################################################################
# 
# handle copying a snapshot of headers, libraries and binaries from the
# floor directory.
#
echo BGQ_FLOOR_DIR=$BGQ_FLOOR_DIR
if [[  $BGQ_FLOOR_DIR != not_defined ]]; then
  for d in `ls $BGQ_FLOOR_DIR`; do
  [[ -d $BGQ_FLOOR_DIR/$d ] && [ "z$d" != "zbeam" ]] && floor_dirlist="$floor_dirlist $d"
  done;
fi
echo "floor directories: $floor_dirlist"



#
# check to see if we have ALL the top level files locally,
#
# have all directorys == unknown, yes, no
#
have_all_dirs=unknown;
echo "scanning dirs" $include_dir_list;

if [[[ -d $BGQ_FLOOR_DIR ]]]; then
   have_all_dirs=yes;
fi

for dx in $floor_dirlist; do
    base=`basename $dx`
    if [[[ ! -d $BGQ_BUILD_DIR/$base ]]]; then
        have_all_dirs=no;
    fi
done

#
# do we link to the floor 
#
bgq_link_to_floor=no
if [[[ $have_all_dirs == no ]]]; then
   if [[[ $BGQ_ENABLE_MAKEFLOOR == no ]]]; then
      if [[[ $BGQ_FLOOR_DIR != none ]]]; then
          bgq_link_to_floor=yes
      fi
   fi
fi

#
# if we are not making a floor and the with-floor=none is not specified,  
#   then the floor directory must be defined...
#
if [[[ $BGQ_ENABLE_MAKEFLOOR == no ]]]; then
    if [[[ $BGQ_FLOOR_DIR != none ]]]; then
      if [[[ ! -d $BGQ_FLOOR_DIR ]]]; then
    	  echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    	  echo "!!!!!!!!!!!!!!!!!!!! No floor directory found" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    	  echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	  echo ""
	  echo "if you wanted to build without a floor use \"scripts/configure --with-floor=none\""
	  echo ""
	  exit 1;
      fi
    fi
fi 


# no link to floor, make sure we have the comm source if we are building the 
# bgws
if [[ $BGQ_HOST_OS == x86_64 ]]; then
  if [[[ $bgq_link_to_floor != yes ]]]; then
    if [[[ ( -d bgpm) ]]]; then
        if [[[ ! ( -d comm) ]]]; then
            echo "bgpm directorie requires   the 'comm' directory"
            #echo
            #echo "  this directory is gathered from git with the following commands:"
            #echo 
            #echo "  cd <workdir>/bgq" 
            #echo "  git clone -b master git://git01.rchland.ibm.com/var/git/bgq/comm"
            #echo "  cd comm"
            #echo "  git clone git://git01.rchland.ibm.com/var/git/pami"
            #echo "  git clone git://git01.rchland.ibm.com/var/git/buildtools"
            #echo "  cd lib"
            #echo "  git clone git://git01.rchland.ibm.com/var/git/bgq/comm/lib/dev"
            #exit 1
            (git clone -b master ssh://git01.rchland.ibm.com/var/git/bgq/comm && 
             cd comm &&
             git clone ssh://git01.rchland.ibm.com/var/git/pami &&
             git clone ssh://git01.rchland.ibm.com/var/git/buildtools &&
             cd lib && 
             git clone ssh://git01.rchland.ibm.com/var/git/bgq/comm/lib/dev) || 
             exit 1;



        fi
    fi;
  fi
fi



mkdir -p $prefix/boot $prefix/bin $prefix/sbin $prefix/man
chmod +w $prefix

#
# suppress these links if --enable-makefloor or we have ALL the top level directories present
#
if [[[ $bgq_link_to_floor == yes ]]]; then


   if [[[ -e $BGQ_FLOOR_DIR/svn_revision ]]]; then
       [[[ -e $prefix/floor_revision ]]] && rm $prefix/floor_revision;
       ln -s -f $BGQ_FLOOR_DIR/svn_revision $prefix/floor_revision;
   fi
    echo "link floor directories";

    for dx in $floor_dirlist; do
        base=`basename $dx`
        if [[[ ! -d $BGQ_BUILD_DIR/$base ]]]; then
            case $base in
              include) ;;
	      bin) cp -su $BGQ_FLOOR_DIR/bin/* $prefix/bin ;;
	      sbin) cp -su $BGQ_FLOOR_DIR/sbin/* $prefix/sbin ;;
	      man) cp -su $BGQ_FLOOR_DIR/man/* $prefix/man ;;
              lib) ;;
	      boot) cp -su $BGQ_FLOOR_DIR/boot/* $prefix/boot ;;
	      crossbin | crosstools | gnu-linux)
	        [[[ -L $prefix/$base ]]] && rm  $prefix/$base
	        [[[ -d $prefix/$base ]]] && rm -rf $prefix/$base
	        # do cross tools and cross bin only if we don't have a gnu local...
  		if [[[ ! -d $BGQ_BUILD_DIR/gnu ]]]; then
                   echo "ln -s $BGQ_FLOOR_DIR/$base $prefix/$base"
                   ln -s $BGQ_FLOOR_DIR/$base $prefix/$base
		fi
                ;;
              example) ;;
              *)   echo "ln -s $BGQ_FLOOR_DIR/$base $prefix/$base"
		   [[[ -L $prefix/$base ]]] && rm  $prefix/$base
		   [[[ -d $prefix/$base ]]] && rm -rf $prefix/$base
                   ln -s $BGQ_FLOOR_DIR/$base $prefix/$base
                   ;;
            esac
        else
    	    echo "mkdir -p $prefix/$base"
	    test -h $prefix/$base
	    if [[[ $? == 0 ]]]; then
	      rm $prefix/$base
	    fi
    	    mkdir -p $prefix/$base
	fi
    done

    # write protect the base work directory...
    echo protecting $prefix
    #chmod gou-w $prefix
fi


# Generate configuration-dependent files.

AC_OUTPUT


chmod ugo+x $BGQ_BUILD_DIR/scripts/runfctest.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/RunMambo.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/RunMamboRegression.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/RunMmcsLite.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/MmcsLiteSetup.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/comm_csets.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/comm_efix.sh
chmod ugo+x $BGQ_BUILD_DIR/scripts/bgqddxglibctest

# grab the top level revision and output it into the top of the tree
# NOTE: this is the version of the top level /bgq dir
#       so this only reflects updates to that top level directory

if [[[ $BGQ_REVISION == auto ]]]; then
  svn info . >$prefix/svn_revision
else
  echo "Revision: $BGQ_REVISION" >$prefix/svn_revision
  echo "Last Changed Rev: $BGQ_REVISION" >> $prefix/svn_revision
  echo "URL: bgq" >>$prefix/svn_revision
fi


echo bgq_link_to_floor=$bgq_link_to_floor
echo have_all_dirs=$have_all_dirs
echo BGQ_ENABLE_MAKEFLOOR=$BGQ_ENABLE_MAKEFLOOR



