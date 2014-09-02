

AC_DEFUN([CPK_CHECK_TESTING],
[

AC_ARG_ENABLE([testing],
   [AC_HELP_STRING([--enable-testing], [enable Testing environnment @<:@default=no@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       enable_testing="yes"
       XMPP_SERVER="xmpp-test.asp64.lan"
       ASP64_REPO="r.asp64.lan"
    else
       enable_testing="no"
    fi
   ],
   [enable_testing="no"])

AC_MSG_CHECKING([whether we build for testing])
AC_MSG_RESULT([${enable_testing}])

AS_IF([test "x$enable_testing" = "xyes"], [$1], [$2])
])
