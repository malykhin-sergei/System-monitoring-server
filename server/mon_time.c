#include <time.h>
#include <jansson.h>

#define TIME_STR_MAXLEN 50

int time_stamp (json_t *time_stmp)
{
    time_t t = time(NULL);
    struct tm *tmp;
    tmp = localtime(&t);

    char current_time[TIME_STR_MAXLEN];
    json_t *json_curr_time;

    strftime (current_time, TIME_STR_MAXLEN, "%c", tmp);
    json_curr_time = json_string (current_time);
    json_object_set (time_stmp, "Time", json_curr_time);
    json_decref (json_curr_time);

    return 0;
}
