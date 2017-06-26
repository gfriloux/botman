EXTRA_DIST += \
   data/config/modules/access.conf \
   data/config/modules/alert.conf \
   data/config/modules/gdb.conf \
   data/config/modules/info.conf \
   data/config/modules/install.conf \
   data/config/modules/notification.conf \
   data/config/modules/rewrite.conf \
   data/config/modules/save.conf \
   data/config/modules/seen.conf \
   data/config/modules/services.conf \
   data/config/modules/spam.conf \
   data/config/modules/ssh_tunnel.conf \
   data/config/modules/sysinfo.conf \
   data/config/modules/version.conf \
   data/www/index.html \
   data/www/api.html \
   data/www/query.html \
   data/www/logs.html \
   data/www/data/css/bootstrap.min.css \
   data/www/data/css/httpd.css \
   data/www/data/img/Alfred.png \
   data/www/data/img/Botman.png \
   data/www/data/img/Civilian.png \
   data/www/data/img/Unidentified.png \
   data/www/data/js/date.format.min.js \
   data/www/data/js/deps.js \
   data/www/data/js/query.js \
   data/www/data/js/log.js \
   data/www/data/js/jquery.min.js \
   data/www/data/js/handlebars-v4.0.5.js \
   data/www/data/js/bootstrap.min.js \
   data/www/data/tpl/api.tpl \
   data/www/data/tpl/dash.tpl \
   data/www/data/tpl/query.tpl \
   data/www/data/tpl/log.tpl \
   data/www/data/fonts/glyphicons-halflings-regular.ttf

wwwdir = $(datarootdir)/gotham/modules.d/httpd/www/
www_DATA = \
   data/www/index.html \
   data/www/api.html \
   data/www/query.html \
   data/www/logs.html

cssdir = $(datarootdir)/gotham/modules.d/httpd/www/data/css/
css_DATA = \
   data/www/data/css/bootstrap.min.css \
   data/www/data/css/httpd.css

imgdir = $(datarootdir)/gotham/modules.d/httpd/www/data/img
img_DATA = \
   data/www/data/img/Alfred.png \
   data/www/data/img/Botman.png \
   data/www/data/img/Civilian.png \
   data/www/data/img/Unidentified.png

jsdir = $(datarootdir)/gotham/modules.d/httpd/www/data/js
js_DATA = \
   data/www/data/js/date.format.min.js \
   data/www/data/js/deps.js \
   data/www/data/js/query.js \
   data/www/data/js/log.js \
   data/www/data/js/jquery.min.js \
   data/www/data/js/handlebars-v4.0.5.js \
   data/www/data/js/bootstrap.min.js

tpldir = $(datarootdir)/gotham/modules.d/httpd/www/data/tpl/
tpl_DATA = \
   data/www/data/tpl/api.tpl \
   data/www/data/tpl/dash.tpl \
   data/www/data/tpl/query.tpl \
   data/www/data/tpl/log.tpl

fontsdir = $(datarootdir)/gotham/modules.d/httpd/www/data/fonts
fonts_DATA = \
   data/www/data/fonts/glyphicons-halflings-regular.ttf
