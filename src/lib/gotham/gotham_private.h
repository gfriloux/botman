#ifndef GOTHAM_PRIV_H
#define GOTHAM_PRIV_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#ifdef HAVE_EVIL
# include <Evil.h>
#endif
#include <Eet.h>
#include <Gotham.h>
#include <cJSON.h>

#include "Gotham_Common_Azy.h"

int gotham_log_dom;

#define CRI(...) EINA_LOG_DOM_CRIT(gotham_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(gotham_log_dom, __VA_ARGS__)
#define NFO(...) EINA_LOG_DOM_INFO(gotham_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(gotham_log_dom, __VA_ARGS__)

static inline void
_gotham_fake_free(void *d EINA_UNUSED, void *d2 EINA_UNUSED)
{}

void _gotham_citizen_list_free_cb(void *data);
Eina_Bool _gotham_citizen_vars_match(Eina_List *vars,
                                     Gotham_Citizen *citizen,
                                     const char *pattern);
Eina_Bool _gotham_citizen_authorized(Gotham *gotham,
                                     const char *jid);

Eina_Bool _gotham_event_connect(void *data, int type, void *ev);
Eina_Bool _gotham_event_disconnect(void *data, int type, void *ev);
void _gotham_event_message_free(void *data, void *ev);
Eina_Bool _gotham_event_message(void *data, int type, void *ev);
Eina_Bool _gotham_event_presence(void *data, int type, void *ev);
Eina_Bool _gotham_event_iq(void *data, int type, void *ev);

void _gotham_modules_load(Gotham *gotham);

char * gotham_utils_elapsed_time(double timestamp);
const char ** gotham_utils_json_array_stringify(cJSON *json_var);
#endif
