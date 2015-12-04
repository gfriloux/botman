##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [1])
m4_define([v_min], [2])
m4_define([v_mic], [16])

m4_define([v_ver], [v_maj.v_min.v_mic])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)

AC_INIT([botman], [v_ver], [guillaume@friloux.me])

AC_PREREQ(2.01)
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([m4])

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

LT_INIT([win32-dll disable-static])
AC_PROG_CXX
AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_PROG_CC
AM_PROG_CC_C_O

AC_PROG_MAKE_SET

AC_HEADER_STDBOOL
AC_C_INLINE

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "${MODULE_ARCH}", "Module architecture")

EFL_CHECK_FUNCS([pbotman], [fnmatch])
EFL_CHECK_TESTS([pbotman], [enable_tests="yes"], [enable_tests="no"])

if test "x${enable_tests}" = "xyes" ; then
   CFLAGS="-W -Wall -g -O0 -DDEBUG -fprofile-arcs -ftest-coverage"
   LDFLAGS="-lgcov -fvisibility=default"
else
   CFLAGS="$CFLAGS -W -Wall -O2"
   LDFLAGS="-Wl,--as-needed -fvisibility=default"
fi

CPK_CHECK_TESTING([enable_testing="yes"], [enable_testing="no"])

AC_CONFIG_FILES([
Makefile
])

min_efl_version=1.7
GOTHAM_REQUIRES=
ALFRED_REQUIRES=
BOTMAN_REQUIRES=
build_windows="no"
build_unix="no"

case "$host_os" in
  mingw*)
    build_windows="yes"
    AC_DEFINE([HAVE_EVIL], [1], [Set to 1 if Evil package is installed])
  ;;
  *)
    build_unix="yes"
  ;;
esac

want_gotham=
build_gotham=
AC_ARG_ENABLE([gotham],
   [AC_HELP_STRING([--disable-gotham],
                   [disable building gotham @<:@default=detect@:>@])],
                   [want_gotham=$enableval]
)
if test "x$want_gotham" != "xno" ; then
   GOTHAM_REQUIRES="shotgun eina >= $min_efl_version ecore >= $min_efl_version eet >= $min_efl_version eio >= $min_efl_version"
   PKG_CHECK_MODULES(GOTHAM, [$GOTHAM_REQUIRES], [build_gotham=yes], [build_gotham=no])
else
   build_gotham=no
fi

build_alfred=
want_alfred=
AC_ARG_ENABLE([alfred],
   [AC_HELP_STRING([--disable-alfred],
                   [disable building alfred daemon. 
                   @<:@default=detect@:>@])],
   [want_alfred=$enableval], [])
if test -z "$want_alfred" ; then
   if test "x$build_gotham" = "xno" ; then
      want_alfred=no
   fi
fi
if test "x$want_alfred" != "xno" ; then
   ALFRED_REQUIRES="$GOTHAM_REQUIRES eio >= $min_efl_version"
   PKG_CHECK_MODULES(ALFRED, [$ALFRED_REQUIRES],
                     [build_alfred=yes], [build_alfred=no])
else
   build_alfred=no
fi

build_botman=
want_botman=
AC_ARG_ENABLE([botman],
   [AC_HELP_STRING([--disable-botman],
                   [disable building botman daemon.
                   @<:@default=detect@:>@])],
   [want_botman=$enableval], [])
if test "x$want_botman" != "xno" ; then
   BOTMAN_REQUIRES="eina ecore eet shotgun"
   if test "x$build_gotham" = "xno" ; then
      BOTMAN_REQUIRES="${BOTMAN_REQUIRES} gotham"
   fi
   PKG_CHECK_MODULES(BOTMAN, [$BOTMAN_REQUIRES],
                     [build_botman=yes], [build_botman=no])
else
   build_botman=no
fi

AM_CONDITIONAL(BUILD_GOTHAM, [test "x${build_gotham}" = "xyes"])
AM_CONDITIONAL(BUILD_ALFRED, [test "x${build_alfred}" = "xyes"])
AM_CONDITIONAL(BUILD_BOTMAN, [test "x${build_botman}" = "xyes"])
AM_CONDITIONAL(BUILD_WINDOWS, [test "x${build_windows}" = "xyes"])
AM_CONDITIONAL(BUILD_UNIX, [test "x${build_unix}" = "xyes"])

AC_SUBST(GOTHAM_REQUIRES)

AC_CONFIG_HEADER([config.h])

AC_OUTPUT([
gotham.pc
])

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  prefix..........: $prefix"
echo "  tests...........: ${enable_tests} (Coverage: ${efl_enable_coverage})"
echo "  CFLAGS..........: $CFLAGS"
echo "  LDFLAGS.........: $LDFLAGS"
echo
echo "  gotham.......: ${build_gotham}"
echo "  alfred.......: ${build_alfred}"
echo "  botman.......: ${build_botman}"
echo

