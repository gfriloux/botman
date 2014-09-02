MAINTAINERCLEANFILES += \
src/lib/gotham/*.gc{no,da}

noinst_LTLIBRARIES = \
src/lib/libgotham.la

src_lib_libgotham_la_SOURCES = \
src/lib/gotham/gotham.c \
src/lib/gotham/gotham_private.h \
src/lib/gotham/citizen.c \
src/lib/gotham/command.c \
src/lib/gotham/conf.c \
src/lib/gotham/emoticons.c \
src/lib/gotham/event.c \
src/lib/gotham/modules.c \
src/lib/gotham/modules_conf.c \
src/lib/gotham/utils.c
src_lib_libgotham_la_CFLAGS = \
$(GOTHAM_CFLAGS) $(AM_CPPFLAGS) \
-DGOTHAM_MODULE_PATH=\"$(libdir)/gotham/$(MODULE_ARCH)\"
src_lib_libgotham_la_LDFLAGS = $(GOTHAM_LIBS)
