#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
AC_PREREQ(2.61)
AC_INIT(magic, 5.25)
AM_INIT_AUTOMAKE([foreign no-dist no-installinfo no-installman -Wall])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

#
# define config.h variables
#
AH_TEMPLATE(DEFAULT_LUA_MAGIC_FILE)
AC_DEFINE_UNQUOTED(DEFAULT_LUA_MAGIC_FILE, ["$CONFDIR/magic.mgc"])


#
# Checks for programs.
#
AC_LANG_C
AC_PROG_CC

#
# define luarocks variables
#
AC_SUBST([LIBFLAG], ["$LIBFLAG"])
AC_SUBST([OBJ_EXTENSION], ["$OBJ_EXTENSION"])
AC_SUBST([LIB_EXTENSION], ["$LIB_EXTENSION"])
AC_SUBST([LIBDIR], ["$LIBDIR"])
AC_SUBST([LIBS], ["$LIBS"])

#
# checking required headers
#
AC_CHECK_HEADERS(
    string.h errno.h,,
    AC_MSG_FAILURE([required header not found])
)

#
# checking required libraries
#
AC_CHECK_LIB([z], [inflate], [], [AC_MSG_ERROR(library libz not found.)])


AC_CONFIG_FILES([ \
    Makefile \
    src/Makefile \
])
AC_OUTPUT
