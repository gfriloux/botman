#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "seen.h"

/**
 * @addtogroup Gotham_Module_Seen
 * @{
 */

struct _time_resolution {
   const char *unit;
   double consumer,
          acquired;
}; /*!< Structure used for time resolution */

/**
 * Define units (day, hour, min, sec) and how many seconds each represents.
 */
struct _time_resolution time_lkt[] = {
   { "day" , 86400, 0 },
   { "hour", 3600 , 0 },
   { "min" , 60   , 0 },
   { "sec" , 1    , 0 },
   { NULL  , 0    , 0 }
};

/**
 * @brief Reset the time resolution variable.
 */
void
_seen_utils_elapsed_time_reset(void) {
   unsigned int i;
   DBG("void");
   for( i = 0; time_lkt[i].unit; i++ ) {
      time_lkt[i].acquired = 0;
   }
}

/**
 * @brief Return readable elapsed time from a timestamp.
 * @param timestamp Timestamp to convert
 * @return char * time in human readable format
 */
char *
seen_utils_elapsed_time(double timestamp) {
   unsigned int i;
   double time_tmp = timestamp;
   Eina_Strbuf *result;
   char *tmp;

   DBG("timestamp[%.0f]", timestamp);

   _seen_utils_elapsed_time_reset();
   result = eina_strbuf_new();

   for( i = 0; time_lkt[i].unit; i++ ) {

      while( time_tmp > time_lkt[i].consumer ) {
         time_tmp -= time_lkt[i].consumer;
         time_lkt[i].acquired++;
      }
      if( time_lkt[i].acquired )
         eina_strbuf_append_printf(result, "%.0f %s%s%s",
                            time_lkt[i].acquired,
                            time_lkt[i].unit,
                            (time_lkt[i].acquired) ? "s" : "",
                            (time_lkt[i+1].unit) ? ", " : "");
   }

   if( !eina_strbuf_length_get(result) )
      eina_strbuf_append(result, "0 sec");

   tmp = eina_strbuf_string_steal(result);
   eina_strbuf_free(result);
   return tmp;
}

/**
 * @}
 */
