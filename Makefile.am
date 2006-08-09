@SET_MAKE@

SUBDIRS = \
	po \
	src

AUTOMAKE_OPTIONS = \
        dist-bzip2

distclean-local:
		rm -rf *.cache *~ 

EXTRA_DIST = \
        README \
        intltool-extract.in \
        intltool-merge.in \
        intltool-update.in

DISTCLEANFILES = \
        intltool-extract \
        intltool-merge \
        intltool-update
