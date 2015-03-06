# begin_generated_IBM_copyright_prolog                             #
#                                                                  #
# This is an automatically generated copyright prolog.             #
# After initializing,  DO NOT MODIFY OR MOVE                       #
#  --------------------------------------------------------------- #
#                                                                  #
# (C) Copyright IBM Corp.  2007, 2008                              #
# IBM CPL License                                                  #
#                                                                  #
#  --------------------------------------------------------------- #
#                                                                  #
# end_generated_IBM_copyright_prolog                               #

# $1 = var name
# $2 = help short name
# $3 = default
# $4 = help text
AC_DEFUN(PAMI_ARG_WITH,
   [AC_ARG_WITH([$2],
      [AS_HELP_STRING([--with-$2], [$4 (default is $3, env:$PAMI_$1)])],
      [[$1]=$withval],
      [if [[ -z "$PAMI_$1" ]] ; then [$1]=$3 ; else [$1]="$PAMI_$1" ; fi])]
   [AC_SUBST([$1])]
)
AC_DEFUN(PAMI_ARG_WITH_OPT,
   [AC_ARG_WITH([$2],
      [AS_HELP_STRING([--with-$2=...], [$4 (default is $3, env:$PAMI_$1)])],
      [[$1]=$withval],
      [if [[ -z "$PAMI_$1" ]] ; then [$1]=$3 ; else [$1]="$PAMI_$1" ; fi])]
   [AC_SUBST([$1])]
)
AC_DEFUN(PAMI_ARG_WITH_PATH,
   [AC_ARG_WITH([$2],
      [AS_HELP_STRING([--with-$2=<dir>], [$4 (default is $3, env:$PAMI_$1)])],
      [[$1]=$withval],
      [if [[ -z "$PAMI_$1" ]] ; then [$1]=$3 ; else [$1]="$PAMI_$1" ; fi])]
   # readlink -f doesn't work on some platforms (we'll use perl instead)
   var="$[$1]"
   tmp=`perl -e "use Cwd 'realpath'; print realpath(\"$var\");"`
   [[$1]=$tmp]
   [AC_SUBST([$1])]
)
AC_DEFUN(PAMI_ARG_WITH_DEFINE,
   [AC_ARG_WITH([$2],
      [AS_HELP_STRING([--with-$2=...], [$4 (default is $3, env:$PAMI_$1)])],
      [[$1]=$withval],
      [if [[ -z "$PAMI_$1" ]] ; then [$1]=$3 ; else [$1]="$PAMI_$1" ; fi])]
   [AC_SUBST([$1])]
   [AC_DEFINE_UNQUOTED([$1], $$1, [$4])]
)
