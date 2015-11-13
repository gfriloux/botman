MAINTAINERCLEANFILES +=                                                        \
src/lib/gotham/*.gc{no,da}

if BUILD_GOTHAM
include_HEADERS = src/include/Gotham.h

noinst_LTLIBRARIES =                                                           \
src/lib/libcjson.la

lib_LTLIBRARIES =                                                              \
src/lib/libgotham.la

src_lib_libgotham_la_SOURCES =                                                 \
   src/lib/gotham/gotham.c                                                     \
   src/lib/gotham/gotham_private.h                                             \
   src/lib/gotham/citizen.c                                                    \
   src/lib/gotham/command.c                                                    \
   src/lib/gotham/conf.c                                                       \
   src/lib/gotham/emoticons.c                                                  \
   src/lib/gotham/event.c                                                      \
   src/lib/gotham/modules.c                                                    \
   src/lib/gotham/modules_conf.c                                               \
   src/lib/gotham/utils.c                                                      \
   src/include/Gotham.h
src_lib_libgotham_la_CFLAGS =                                                  \
   $(GOTHAM_CFLAGS) $(AM_CPPFLAGS) -DEFL_GOTHAM_BUILD                          \
   -DGOTHAM_MODULE_PATH=\"$(libdir)/gotham/$(MODULE_ARCH)\"
src_lib_libgotham_la_LDFLAGS = $(GOTHAM_LIBS)                                  \
   -version-info @version_info@ -no-undefined
src_lib_libgotham_la_LIBADD =                                                  \
   src/lib/libcjson.la

src_lib_libcjson_la_SOURCES =                                                  \
   src/lib/extras/cJSON.c                                                      \
   src/include/cJSON.h
endif
