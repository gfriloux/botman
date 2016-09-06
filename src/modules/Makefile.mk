MAINTAINERCLEANFILES += \
src/lib/modules/seen/*.gc{no,da}

moddir = $(libdir)/gotham/$(MODULE_ARCH)
mod_LTLIBRARIES = \
   src/modules/access.la \
   src/modules/help.la \
   src/modules/module.la \
   src/modules/seen.la \
   src/modules/save.la \
   src/modules/spam.la \
   src/modules/info.la \
   src/modules/notification.la

BUILT_SOURCES += .sources_notification
src_modules_notification_la_SOURCES = \
   src/modules/notification/main.c \
   src/modules/notification/main.h \
   src/modules/notification/alfred.c \
   src/modules/notification/utils.c \
   src/modules/notification/Module_Common_Azy.c \
   src/modules/notification/Module_Common_Azy.h \
   src/modules/notification/Module_Common.c \
   src/modules/notification/Module_Common.h
src_modules_notification_la_CFLAGS = $(GOTHAM_CFLAGS) -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_notification_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_notification_la_LIBADD = \
   src/lib/libgotham.la
.sources_notification: src/modules/notification/notification.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/notification \
	                    $(top_srcdir)/src/modules/notification/notification.azy

BUILT_SOURCES += .sources_access
src_modules_access_la_SOURCES = \
   src/modules/access/access.c \
   src/modules/access/access.h \
   src/modules/access/alfred.c \
   src/modules/access/botman.c \
   src/modules/access/citizen.c \
   src/modules/access/event.c \
   src/modules/access/modules.c \
   src/modules/access/Module_Common_Azy.c \
   src/modules/access/Module_Common_Azy.h \
   src/modules/access/Module_Common.c \
   src/modules/access/Module_Common.h
src_modules_access_la_CFLAGS = $(GOTHAM_CFLAGS) -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_access_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_access_la_LIBADD = \
   src/lib/libgotham.la
src_modules_access_la_LIBTOOLFLAGS = --tag=disable-static
.sources_access: src/modules/access/access.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/access \
	                    $(top_srcdir)/src/modules/access/access.azy

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

src_modules_module_la_SOURCES = \
   src/modules/module/actions.c \
   src/modules/module/module.c \
   src/modules/module/module.h
src_modules_module_la_CFLAGS = $(GOTHAM_CFLAGS)
src_modules_module_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_module_la_LIBADD = \
   src/lib/libgotham.la
src_modules_module_la_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES += .sources_seen
src_modules_seen_la_SOURCES = \
   src/modules/seen/seen.c \
   src/modules/seen/event.c \
   src/modules/seen/seen_query.c \
   src/modules/seen/seen.h \
   src/modules/seen/Module_Common_Azy.c \
   src/modules/seen/Module_Common_Azy.h \
   src/modules/seen/Module_Common.c \
   src/modules/seen/Module_Common.h
src_modules_seen_la_CFLAGS = $(GOTHAM_CFLAGS) -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_seen_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_seen_la_LIBADD = \
   src/lib/libgotham.la
src_modules_seen_la_LIBTOOLFLAGS = --tag=disable-static
.sources_seen: src/modules/seen/seen.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/seen \
	                    $(top_srcdir)/src/modules/seen/seen.azy

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
   src/lib/libgotham.la \
   src/lib/libcjson.la
src_modules_save_la_LIBTOOLFLAGS = --tag=disable-static

BUILT_SOURCES += .sources_spam
src_modules_spam_la_SOURCES = \
   src/modules/spam/event.c \
   src/modules/spam/queue.c \
   src/modules/spam/spam.c \
   src/modules/spam/spam.h \
   src/modules/spam/spam_query.c \
   src/modules/spam/utils.c \
   src/modules/spam/Module_Common_Azy.c \
   src/modules/spam/Module_Common_Azy.h \
   src/modules/spam/Module_Common.c \
   src/modules/spam/Module_Common.h
src_modules_spam_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_spam_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_spam_la_LIBADD = \
   src/lib/libgotham.la
src_modules_spam_la_LIBTOOLFLAGS = --tag=disable-static
.sources_spam: src/modules/spam/spam.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/spam \
	                    $(top_srcdir)/src/modules/spam/spam.azy


BUILT_SOURCES += .sources_info
src_modules_info_la_SOURCES = \
   src/modules/info/alfred.c \
   src/modules/info/alfred_sort.c \
   src/modules/info/botman.c \
   src/modules/info/utils.c \
   src/modules/info/info.c \
   src/modules/info/info.h \
   src/modules/info/Module_Common_Azy.c \
   src/modules/info/Module_Common_Azy.h \
   src/modules/info/Module_Common.c \
   src/modules/info/Module_Common.h
src_modules_info_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_info_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_info_la_LIBADD = \
   src/lib/libgotham.la
src_modules_info_la_LIBTOOLFLAGS = --tag=disable-static
.sources_info: src/modules/info/info.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/info \
	                    $(top_srcdir)/src/modules/info/info.azy
   
if BUILD_UNIX
mod_LTLIBRARIES += \
   src/modules/install.la \
   src/modules/rewrite.la \
   src/modules/sysinfo.la \
   src/modules/version.la \
   src/modules/ssh_tunnel.la \
   src/modules/gdb.la
   
src_modules_install_la_SOURCES = \
   src/modules/install/botman.c \
   src/modules/install/conf.c \
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

BUILT_SOURCES += .sources_rewrite
src_modules_rewrite_la_SOURCES = \
   src/modules/rewrite/event.c \
   src/modules/rewrite/rewrite.c \
   src/modules/rewrite/rewrite.h \
   src/modules/rewrite/utils.c \
   src/modules/rewrite/Module_Common_Azy.c \
   src/modules/rewrite/Module_Common_Azy.h \
   src/modules/rewrite/Module_Common.c \
   src/modules/rewrite/Module_Common.h
src_modules_rewrite_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_rewrite_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS)
src_modules_rewrite_la_LIBADD = \
   src/lib/libgotham.la
   src_modules_rewrite_la_LIBTOOLFLAGS = --tag=disable-static
.sources_rewrite: src/modules/rewrite/rewrite.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/rewrite \
	                    $(top_srcdir)/src/modules/rewrite/rewrite.azy

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

BUILT_SOURCES += .sources_version
src_modules_version_la_SOURCES = \
   src/modules/version/alfred.c \
   src/modules/version/botman.c \
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
.sources_version: src/modules/version/version.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/version \
	                    $(top_srcdir)/src/modules/version/version.azy

BUILT_SOURCES += .sources_sshtunnel
src_modules_ssh_tunnel_la_SOURCES = \
   src/modules/ssh_tunnel/alfred.c \
   src/modules/ssh_tunnel/ssh_tunnel.c \
   src/modules/ssh_tunnel/ssh_tunnel.h \
   src/modules/ssh_tunnel/botman.c \
   src/modules/ssh_tunnel/event.c \
   src/modules/ssh_tunnel/Module_Common_Azy.c \
   src/modules/ssh_tunnel/Module_Common_Azy.h \
   src/modules/ssh_tunnel/Module_Common.c \
   src/modules/ssh_tunnel/Module_Common.h
src_modules_ssh_tunnel_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_ssh_tunnel_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS) -Wl,-z,defs
src_modules_ssh_tunnel_la_LIBADD = \
   src/lib/libgotham.la
src_modules_ssh_tunnel_la_LIBTOOLFLAGS = --tag=disable-static
.sources_sshtunnel: src/modules/ssh_tunnel/ssh_tunnel.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/ssh_tunnel \
	$(top_srcdir)/src/modules/ssh_tunnel/ssh_tunnel.azy

BUILT_SOURCES += .sources_gdb
src_modules_gdb_la_SOURCES = \
   src/modules/gdb/backtrace.c \
   src/modules/gdb/botman.c \
   src/modules/gdb/module.c \
   src/modules/gdb/event.c \
   src/modules/gdb/botman_delete.c \
   src/modules/gdb/botman_fetch.c \
   src/modules/gdb/botman_list.c \
   src/modules/gdb/utils.c \
   src/modules/gdb/gdb.h \
   src/modules/gdb/Module_Common_Azy.c \
   src/modules/gdb/Module_Common_Azy.h \
   src/modules/gdb/Module_Common.c \
   src/modules/gdb/Module_Common.h
src_modules_gdb_la_CFLAGS = \
   $(GOTHAM_CFLAGS) -DDATA_DIR=\"$(datadir)\" \
   -DSYSCONF_DIR=\"$(sysconfdir)\"
src_modules_gdb_la_LDFLAGS = \
   -no-undefined -module -avoid-version \
   $(GOTHAM_LIBS) -Wl,-z,defs
src_modules_gdb_la_LIBADD = \
   src/lib/libgotham.la
src_modules_gdb_la_LIBTOOLFLAGS = --tag=disable-static
.sources_gdb: src/modules/gdb/gdb.azy
	azy_parser -H -p -o $(top_srcdir)/src/modules/gdb \
	                    $(top_srcdir)/src/modules/gdb/gdb.azy
endif
