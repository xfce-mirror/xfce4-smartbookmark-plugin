dnl Version information
m4_define([smartbookmark_version_major], [0])
m4_define([smartbookmark_version_minor], [4])
m4_define([smartbookmark_version_micro], [1])
m4_define([smartbookmark_version],
    [smartbookmark_version_major.smartbookmark_version_minor.smartbookmark_version_micro])

AC_COPYRIGHT([Copyright (c) 2006 by Emanuele Rocca <ema@debian.org>])
AC_INIT([xfce4-smartbookmark-plugin], [smartbookmark_version()], [ema@debian.org])

dnl Initialize automake
AM_INIT_AUTOMAKE([AC_PACKAGE_TARNAME()], [AC_PACKAGE_VERSION()])
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE()

dnl check for basic programs
AC_PROG_CC()
AC_PROG_INSTALL()
AC_PROG_LIBTOOL()
AC_PROG_INTLTOOL()

dnl Check for i18n support
XDT_I18N([hu it fr zh_TW])

dnl Check for required packages
XDT_CHECK_LIBX11_REQUIRE()
XDT_CHECK_PACKAGE([GDK], [gdk-2.0], [2.0])
XDT_CHECK_PACKAGE([LIBXFCEGUI4], [libxfce4panel-1.0], [4.2])
XDT_CHECK_PACKAGE([LIBXFCE4PANEL], [libxfce4panel-1.0], [4.3.20])

XDT_FEATURE_DEBUG()

AC_OUTPUT([Makefile
           po/Makefile.in
           src/Makefile])
