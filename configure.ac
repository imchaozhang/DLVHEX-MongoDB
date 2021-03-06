#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([dlvhex-mongoplugin],[2.0.0],[dlvhex-devel@lists.sourceforge.net])
AC_CONFIG_SRCDIR([src/MongoPlugin.cpp])
AC_CONFIG_HEADERS(include/[config.h])


# for documentation of the versioning scheme see
# https://sourceforge.net/apps/trac/dlvhex/wiki/LibraryVersions
DLVHEX_DEFINE_VERSION([MONGOPLUGIN],[$PACKAGE_VERSION])
DLVHEX_DEFINE_VERSION([DLVHEX_ABI],[7.0.0])

AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

# by default we look into /opt/local and /sw for include files (macports vs. fink)
case $host_os in
     darwin*|macosx*)
	CPPFLAGS="$CPPFLAGS -I/opt/local/include -I/sw/include"
     ;;
esac 


# Checks for programs.
AC_PROG_CXX
AC_LANG([C++])

# Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS([sys/param.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_LIBTOOL_DLOPEN # we build a module library
AC_DISABLE_STATIC # we don't need a static library
AC_PROG_LIBTOOL

#
# check for other libraries here
#

# check for dlvhex, sets various variables
DLVHEX_REQUIRE([2.0.0])
PKG_CHECK_MODULES([DLVHEX], [dlvhex2 >= 2.0.0])


# checking for boost libs
BOOST_REQUIRE([1.45.0])
BOOST_SMART_PTR
BOOST_STRING_ALGO
BOOST_TOKENIZER



# Checks for mongoclient.
AC_ARG_WITH(
  mongo,
  [AC_HELP_STRING([--with-mongo=PATH],
    [mongoDB include path [default=/usr/local/include]])],
  [MONGO_PATH="$withval"],
  [MONGO_PATH="/usr/local/include"]
)

AC_CHECK_FILE(
  "${MONGO_PATH}/mongo/client/dbclient.h",
  [MONGO_INCLUDEDIR=${MONGO_PATH}],
  [AC_MSG_ERROR([Missing required mongoDB client header.])]
)

AC_ARG_WITH(
  mongo-lib,
  [AC_HELP_STRING([--with-mongo-lib=PATH],
    [mongo client library path [default=/usr/local/lib]])],
  [MONGO_LIBPATH="$withval"],
  [MONGO_LIBPATH="/usr/local/lib"]
)

AS_IF([test "x${MONGO_LIBPATH}" != xno],
  MONGO_LDFLAGS="-L${MONGO_LIBPATH}"
)

AC_MSG_CHECKING([for mongoDB client library])
AC_LANG(C++)
LDFLAGS="$LDFLAGS ${MONGO_LDFLAGS} -lmongoclient -lboost_system -lboost_thread -lboost_filesystem -lboost_program_options"

  MONGO_LIBS="-lmongoclient"
  MONGO_LIBS="-L${MONGO_LIBPATH} $MONGO_LIBS"
  
AC_MSG_RESULT(yes)


# mongoDB client libraries.
MONGO_INCLUDES="-I${MONGO_INCLUDEDIR}"
MONGO_LDFLAGS="${MONGO_LDFLAGS} ${MONGO_LIBS}"
AC_SUBST(MONGO_INCLUDES)
AC_SUBST(MONGO_LDFLAGS)
AC_SUBST(MONGO_LIBS)



# where the plugin will be installed by default.
# running configure will take sysplugindir from dlvhex.pc,
# running configure --enable-userinstall will take homeplugindir.
AC_ARG_ENABLE(userinstall, 
             [  --enable-userinstall    Install plugin into home directory],
             [plugindir=$HOME/$DLVHEX_USERPLUGINDIR],
             [plugindir=$DLVHEX_SYSPLUGINDIR]
             )
AC_SUBST([plugindir])
if test "x$plugindir" == "x"; then
	AC_MSG_ERROR([Cannot determine nonempty value for plugindir!])
fi

#
# all needed makefiles
#

AC_CONFIG_FILES([Makefile
				include/Makefile
                src/Makefile])

AC_OUTPUT

dnl output configuration
cat <<EOF
 
$PACKAGE_TARNAME $PACKAGE_VERSION configuration:

 - Compiler settings
 CC: $CC
 CXX: $CXX
 CFLAGS: $CFLAGS
 CPPFLAGS: $CPPFLAGS
 CXXFLAGS: $CXXFLAGS
 LDFLAGS: $LDFLAGS
 MONGO_INCLUDES: $MONGO_INCLUDES
 
 
 =======================
 MONGO_LDFLAGS: $MONGO_LDFLAGS

 - from pkg-config
 DLVHEX_CFLAGS: $DLVHEX_CFLAGS
 DLVHEX_LIBS: $DLVHEX_LIBS
 DLVHEX_BINDIR: $DLVHEX_BINDIR
 
 =======================
 MONGO_LIBS: $MONGO_LIBS
 
 - Path settings
 prefix: $prefix
 plugindir: $plugindir
EOF
