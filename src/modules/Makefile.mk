MAINTAINERCLEANFILES += \
src/lib/modules/seen/*.gc{no,da}

moddir = $(libdir)/gotham/$(MODULE_ARCH)
mod_LTLIBRARIES = \
src/modules/access.la \
src/modules/help.la \
src/modules/install.la \
src/modules/module.la \
src/modules/seen.la \
src/modules/save.la \
src/modules/spam.la \
src/modules/rewrite.la \
src/modules/sysinfo.la \
src/modules/version.la \
src/modules/ssh_tunnel.la \
src/modules/gdb.la \
src/modules/info.la

src_modules_access_la_SOURCES = \
src/modules/access/access.c \
src/modules/access/access.h \
src/modules/access/alfred.c \
src/modules/access/botman.c \
src/modules/access/citizen.c \
src/modules/access/conf.c \
src/modules/access/event.c \
src/modules/access/modules.c
src_modules_access_la_CFLAGS = $(GOTHAM_CFLAGS) -DSYSCONF_DIR=\"$(sysconfdir)\"
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

src_modules_install_la_SOURCES = \
src/modules/install/botman.c \
src/modules/install/conf.c \
src/modules/install/event.c \
src/modules/install/jobs.c \
src/modules/install/install.c \
src/modules/install/install.h
src_modules_install_la_CFLAGS = $(GOTHAM_CFLAGS) -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_install_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS) -Wl,-z,defs
src_modules_install_la_LIBADD = \
src/lib/libgotham.la
src_modules_install_la_LIBTOOLFLAGS = --tag=disable-static


src_modules_module_la_SOURCES = \
src/modules/module/actions.c \
src/modules/module/event.c \
src/modules/module/module.c \
src/modules/module/module.h
src_modules_module_la_CFLAGS = $(GOTHAM_CFLAGS)
src_modules_module_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_module_la_LIBADD = \
src/lib/libgotham.la
src_modules_module_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_seen_la_SOURCES = \
src/modules/seen/seen.c \
src/modules/seen/utils.c \
src/modules/seen/conf.c \
src/modules/seen/event.c
src_modules_seen_la_CFLAGS = $(GOTHAM_CFLAGS) -DSYSCONF_DIR=\"$(sysconfdir)\"
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
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
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
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
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
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_rewrite_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_rewrite_la_LIBADD = \
src/lib/libgotham.la
src_modules_rewrite_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_sysinfo_la_SOURCES = \
src/modules/sysinfo/alfred.c \
src/modules/sysinfo/botman.c \
src/modules/sysinfo/conf.c \
src/modules/sysinfo/event.c \
src/modules/sysinfo/sysinfo.c \
src/modules/sysinfo/sysinfo.h
src_modules_sysinfo_la_CFLAGS = \
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_sysinfo_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_sysinfo_la_LIBADD = \
src/lib/libgotham.la
src_modules_sysinfo_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_version_la_SOURCES = \
src/modules/version/alfred.c \
src/modules/version/botman.c \
src/modules/version/conf.c \
src/modules/version/event.c \
src/modules/version/find.c \
src/modules/version/utils.c \
src/modules/version/version.c \
src/modules/version/version.h
src_modules_version_la_CFLAGS = \
$(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_version_la_LDFLAGS = \
-no-undefined -module -avoid-version \
$(GOTHAM_LIBS)
src_modules_version_la_LIBADD = \
src/lib/libgotham.la
src_modules_version_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_ssh_tunnel_la_SOURCES = \
   src/modules/ssh_tunnel/alfred.c \
   src/modules/ssh_tunnel/ssh_tunnel.c \
   src/modules/ssh_tunnel/ssh_tunnel.h \
   src/modules/ssh_tunnel/botman.c \
   src/modules/ssh_tunnel/conf.c \
   src/modules/ssh_tunnel/event.c
src_modules_ssh_tunnel_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_ssh_tunnel_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS) -Wl,-z,defs
src_modules_ssh_tunnel_la_LIBADD = \
   src/lib/libgotham.la
src_modules_ssh_tunnel_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_gdb_la_SOURCES = \
   src/modules/gdb/backtrace.c \
   src/modules/gdb/botman.c \
   src/modules/gdb/conf.c \
   src/modules/gdb/module.c \
   src/modules/gdb/event.c \
   src/modules/gdb/botman_delete.c \
   src/modules/gdb/botman_fetch.c \
   src/modules/gdb/botman_list.c \
   src/modules/gdb/utils.c \
   src/modules/gdb/gdb.h
src_modules_gdb_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_gdb_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS) -Wl,-z,defs
src_modules_gdb_la_LIBADD = \
   src/lib/libgotham.la
src_modules_gdb_la_LIBTOOLFLAGS = --tag=disable-static

src_modules_info_la_SOURCES = \
   src/modules/info/alfred.c \
   src/modules/info/conf.c \
   src/modules/info/event.c \
   src/modules/info/info.c \
   src/modules/info/info.h
src_modules_info_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_info_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS) -Wl,-z,defs
src_modules_info_la_LIBADD = \
   src/lib/libgotham.la
src_modules_info_la_LIBTOOLFLAGS = --tag=disable-static
   
