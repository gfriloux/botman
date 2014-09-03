MAINTAINERCLEANFILES += \
src/lib/modules/seen/*.gc{no,da}

moddir = $(libdir)/gotham/$(MODULE_ARCH)
mod_LTLIBRARIES = \
src/modules/seen.la

src_modules_seen_la_SOURCES = \
src/modules/seen/seen.c \
src/modules/seen/utils.c \
src/modules/seen/conf.c \
src/modules/seen/event.c
src_modules_seen_la_CFLAGS = $(GOTHAM_CFLAGS)
src_modules_seen_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_seen_la_LIBADD = \
src/lib/libgotham.la
src_modules_seen_la_LIBTOOLFLAGS = --tag=disable-static
