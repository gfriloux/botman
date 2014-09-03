MAINTAINERCLEANFILES += \
src/lib/modules/seen/*.gc{no,da}

moddir = $(libdir)/gotham/$(MODULE_ARCH)
mod_LTLIBRARIES = \
src/modules/access.la \
src/modules/help.la \
src/modules/seen.la \
src/modules/save.la \
src/modules/spam.la \
src/modules/rewrite.la

src_modules_access_la_SOURCES = \
src/modules/access/access.c \
src/modules/access/access.h \
src/modules/access/alfred.c \
src/modules/access/botman.c \
src/modules/access/citizen.c \
src/modules/access/conf.c \
src/modules/access/event.c \
src/modules/access/modules.c
src_modules_access_la_CFLAGS = $(GOTHAM_CFLAGS)
src_modules_access_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_access_la_LIBADD = \
src/lib/libgotham.la
src_modules_access_la_LIBTOOLFLAGS = --tag=disable-static


src_modules_help_la_SOURCES = \
src/modules/help/event.c \
src/modules/help/help.c \
src/modules/help/help.h
src_modules_help_la_CFLAGS = $(GOTHAM_CFLAGS)
src_modules_help_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_help_la_LIBADD = \
src/lib/libgotham.la
src_modules_help_la_LIBTOOLFLAGS = --tag=disable-static

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

src_modules_save_la_SOURCES = \
src/modules/save/conf.c \
src/modules/save/event.c \
src/modules/save/save.c \
src/modules/save/save.h \
src/modules/save/utils.c
src_modules_save_la_CFLAGS = \
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\"
src_modules_save_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_save_la_LIBADD = \
src/lib/libgotham.la
src_modules_save_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_spam_la_SOURCES = \
src/modules/spam/conf.c \
src/modules/spam/event.c \
src/modules/spam/queue.c \
src/modules/spam/spam.c \
src/modules/spam/spam.h \
src/modules/spam/utils.c
src_modules_spam_la_CFLAGS = \
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\"
src_modules_spam_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_spam_la_LIBADD = \
src/lib/libgotham.la
src_modules_spam_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_rewrite_la_SOURCES = \
src/modules/rewrite/conf.c \
src/modules/rewrite/event.c \
src/modules/rewrite/rewrite.c \
src/modules/rewrite/rewrite.h \
src/modules/rewrite/utils.c
src_modules_rewrite_la_CFLAGS = \
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\"
src_modules_rewrite_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_rewrite_la_LIBADD = \
src/lib/libgotham.la
src_modules_rewrite_la_LIBTOOLFLAGS = --tag=disable-static

