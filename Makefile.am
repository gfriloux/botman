ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects
AM_MAKEFLAGS = --no-print-directory
AM_CPPFLAGS = -DDATA_DIR=\"$(datadir)\" @CPPFLAGS@                             \
   -I$(top_srcdir) -I$(top_srcdir)/src/include -I$(top_srcdir)/src/lib/gotham

EXTRA_DIST = rpmbuild/SPECS/@PACKAGE@.spec.in

MAINTAINERCLEANFILES = \
ABOUT-NLS \
Makefile.in \
aclocal.m4 \
compile \
config.guess \
config.h.in~ \
config.h.in \
config.sub \
configure \
config.cache-env \
depcomp \
install-sh \
ltmain.sh \
missing \
rpmbuild/SPECS/@PACKAGE@.spec

BUILT_SOURCES=

include data/Makefile.mk
include src/bin/Makefile.mk

if BUILD_GOTHAM
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gotham.pc

include src/lib/Makefile.mk
include src/modules/Makefile.mk
endif
