dnl Version information
XDT_VERSION_INIT([0.5.2], [git])

AC_COPYRIGHT([Copyright (c) 2006-2020
              The Xfce development team. All rights reserved. 

Written by by Emanuele Rocca <ema@debian.org>])
AC_INIT([xfce4-smartbookmark-plugin], [xdt_version], [https://gitlab.xfce.org/panel-plugins/xfce4-smartbookmark-plugin])
AC_PREREQ([2.69])
AC_REVISION([xdt_version_build])

dnl Initialize automake
AM_INIT_AUTOMAKE([1.8 dist-bzip2 tar-ustar no-dist-gzip foreign])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE()
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIRS([m4])

dnl check for basic programs
AC_PROG_CC()
AC_PROG_INSTALL()

dnl Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

dnl Check for i18n support
GETTEXT_PACKAGE="$PACKAGE"
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Name of default gettext domain])
AC_SUBST([GETTEXT_PACKAGE])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

dnl Check for required packages
XDT_CHECK_PACKAGE([GLIB], [glib-2.0], [2.50.0])
XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.22.0])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.12.0])
XDT_CHECK_PACKAGE([LIBXFCE4PANEL], [libxfce4panel-2.0], [4.12.0])

XDT_FEATURE_DEBUG([xdt_debug_default])

AC_CONFIG_FILES([Makefile
           po/Makefile.in
           src/Makefile])
AC_OUTPUT
