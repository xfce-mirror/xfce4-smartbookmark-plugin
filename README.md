[![License](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://gitlab.xfce.org/panel-plugins/xfce4-smartbookmark-plugin/-/blob/master/COPYING)

# xfce4-smartbookmark-plugin

Xfce4-smartbookmark-plugin allows you to send requests directly to your
browser and perform a custom search.

Some URL examples:

 Google: https://www.google.it/search?q=
 Debian BTS: https://bugs.debian.org/
 Wikipedia: https://en.wikipedia.org/wiki/

----

### Homepage

[Xfce4-smartbookmark-plugin documentation](https://docs.xfce.org/panel-plugins/xfce4-smartbookmark-plugin)

### Changelog

See [NEWS](https://gitlab.xfce.org/panel-plugins/xfce4-smartbookmark-plugin/-/blob/master/NEWS) for details on changes and fixes made in the current release.

### Source Code Repository

[Xfce4-smartbookmark-plugin source code](https://gitlab.xfce.org/panel-plugins/xfce4-smartbookmark-plugin)

### Download a Release Tarball

[Xfce4-smartbookmark-plugin archive](https://archive.xfce.org/src/panel-plugins/xfce4-smartbookmark-plugin)
    or
[Xfce4-smartbookmark-plugin tags](https://gitlab.xfce.org/panel-plugins/xfce4-smartbookmark-plugin/-/tags)

### Installation

From source code repository: 

    % cd xfce4-smartbookmark-plugin
    % meson setup build
    % meson compile -C build
    % meson install -C build

From release tarball:

    % tar xf xfce4-smartbookmark-plugin-<version>.tar.xz
    % cd xfce4-smartbookmark-plugin-<version>
    % meson setup build
    % meson compile -C build
    % meson install -C build

### Uninstallation

    % ninja uninstall -C build

### Reporting Bugs

Visit the [reporting bugs](https://docs.xfce.org/panel-plugins/xfce4-smartbookmark-plugin/bugs) page to view currently open bug reports and instructions on reporting new bugs or submitting bugfixes.

