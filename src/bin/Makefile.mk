MAINTAINERCLEANFILES += \
src/bin/{botman,alfred}/*.gc{no,da}

bin_PROGRAMS =

if BUILD_BOTMAN
bin_PROGRAMS += src/bin/botman/botman

src_bin_botman_botman_SOURCES = \
src/bin/botman/main.c \
src/bin/botman/event.c \
src/bin/botman/botman.h
src_bin_botman_botman_CFLAGS = \
$(BOTMAN_CFLAGS) $(AM_CPPFLAGS) \
-DDATA_DIR=\"$(datadir)\"
src_bin_botman_botman_LDFLAGS = $(BOTMAN_LIBS)
if BUILD_GOTHAM
src_bin_botman_botman_LDADD = \
src/lib/libgotham.la
endif
endif

if BUILD_ALFRED
bin_PROGRAMS += src/bin/alfred/alfred

src_bin_alfred_alfred_SOURCES = \
src/bin/alfred/main.c \
src/bin/alfred/event.c \
src/bin/alfred/emoticons.c \
src/bin/alfred/alfred.h
src_bin_alfred_alfred_CFLAGS = \
$(ALFRED_CFLAGS) $(AM_CPPFLAGS) \
-DDATA_DIR=\"$(datadir)\"
src_bin_alfred_alfred_LDFLAGS = $(ALFRED_LIBS)
src_bin_alfred_alfred_LDADD = \
src/lib/libgotham.la
endif
