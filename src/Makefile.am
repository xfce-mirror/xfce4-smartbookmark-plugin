plugindir = $(libdir)/xfce4/panel-plugins

plugin_LTLIBRARIES =							\
	libsmartbookmark.la

libsmartbookmark_la_LDFLAGS =							\
	-O0		\
	-avoid-version							\
	-module						\
	@LIBXFCE4PANEL_LIBS@ \
	@LIBXFCEGUI4_LIBS@	\
	@GDK_LIBS@	\
	@LIBX11_LDFLAGS@

libsmartbookmark_la_SOURCES =							\
	smartbookmark.c


libsmartbookmark_la_CFLAGS =	 \
	-O0		\
	-I$(top_srcdir) \
	@LIBXFCEGUI4_CFLAGS@ \
	@LIBXFCE4PANEL_CFLAGS@ \
	@LIBX11_CFLAGS@	\
	@GDK_CFLAGS@	\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"

desktop_in_in_files = smartbookmark.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)

desktopdir = $(datadir)/xfce4/panel-plugins
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = 				\
	$(desktop_in_in_files)

DISTCLEANFILES =			\
	$(desktop_DATA) $(desktop_in_files)

# get full path into .desktop file
%.desktop.in: %.desktop.in.in
	sed -e "s^@INTERNAL_PLUGIN_PATH@^$(libdir)/xfce4/panel-plugins^" \
		$< > $@
