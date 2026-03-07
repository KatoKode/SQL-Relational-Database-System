#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([RedB], [1.0], [redb-bugs@redb.org])
AM_INIT_AUTOMAKE([silent-rules subdir-objects])
AM_SILENT_RULES
AC_CONFIG_SRCDIR([srv/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
if test "x${CXXFLAGS}" = x; then
  CXXFLAGS=""
fi

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for Command-line options

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AM_PATH_GLIB_2_0

have_json_glib=no
AC_SEARCH_LIBS([json_builder_new], [json-glib-1.0], [have_json_glib=yes])
if test "x${have_json_glib}" = xyes; then
  AC_CHECK_HEADERS([pthread.h])
#  AC_CHECK_HEADERS([json-glib/json-glib.h], [], [have_json_glib=no])
fi

if test "x${have_json_glib}" = xno; then
  AC_MSG_ERROR([Missing required library json-glib >= 1.0], [-1])
fi

have_gcrypt=no
AC_SEARCH_LIBS([gcry_cipher_open], [gcrypt], [have_gcrypt=yes])
if test "x${have_gcrypt}" = xyes; then
  AC_CHECK_HEADERS([gcrypt.h], [], [have_gcrypt=no])
fi

if test "x${have_gcrypt}" = xno; then
  AC_MSG_ERROR([Missing required library libgcrypt >= 1.6.1], [-1])
fi

have_ncurses=no
AC_SEARCH_LIBS([initscr], [ncurses], [have_ncurses=yes])
if test "x${have_ncurses}" = xyes; then
  AC_CHECK_HEADERS([ncurses.h], [], [have_ncurses=no])
fi

if test "x${have_ncurses}" = xno; then
  AC_MSG_ERROR([Missing required library libncurses], [-1])
fi

have_readline=no
AC_SEARCH_LIBS([readline], [readline], [have_readline=yes])
if test "x${have_readline}" = xyes; then
  AC_CHECK_HEADERS([readline/readline.h], [], [have_readline=no])
fi

if test "x${have_readline}" = xno; then
  AC_MSG_ERROR([Missing required library libreadline], [-1])
fi

have_history=no
AC_SEARCH_LIBS([using_history], [readline], [have_history=yes])
if test "x${have_history}" = xyes; then
  AC_CHECK_HEADERS([readline/history.h], [], [have_history=no])
fi

if test "x${have_history}" = xno; then
  AC_MSG_ERROR([Missing required library (for history) libreadline], [-1])
fi

AC_CONFIG_FILES([Makefile
                 eno/Makefile
                 logger/Makefile
                 util/Makefile
                 comm/Makefile
                 dio/Makefile
                 buffer/Makefile
                 bcalc/Makefile
                 schema/Makefile
                 lock/Makefile
                 cache/Makefile
                 row/Makefile
                 btree/Makefile
                 token/Makefile
                 tran/Makefile
                 comp/Makefile
                 sql/Makefile
                 dbms/Makefile
                 vm/Makefile
                 srvcry/Makefile
                 clicry/Makefile
                 srv/Makefile
                 cli/Makefile
                 redb_install_db/Makefile])

AC_OUTPUT
