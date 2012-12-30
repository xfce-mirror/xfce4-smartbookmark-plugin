plugindir = $(libdir)/xfce4/panel/plugins

plugin_LTLIBRARIES =							\
	libsmartbookmark.la

libsmartbookmark_la_LDFLAGS =							\
	-avoid-version							\
	-module						\
	-no-undefined \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

libsmartbookmark_la_LIBADD = \
	@LIBXFCE4PANEL_LIBS@ \
	@LIBXFCE4UI_LIBS@

libsmartbookmark_la_SOURCES =							\
	smartbookmark.c


libsmartbookmark_la_CFLAGS =	 \
	-I$(top_srcdir) \
	@LIBXFCE4UI_CFLAGS@ \
	@LIBXFCE4PANEL_CFLAGS@ \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

desktop_in_files = smartbookmark.desktop.in

desktopdir = $(datadir)/xfce4/panel/plugins
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = $(desktop_in_files)

DISTCLEANFILES = $(desktop_DATA)
