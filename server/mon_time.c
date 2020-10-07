#include <time.h>
#include <jansson.h>

#define TIME_STR_MAXLEN 50

int time_stamp (json_t *time_stmp)
{
    time_t t = time(NULL);
    struct tm *tmp;
    tmp = localtime(&t);

    char current_time[TIME_STR_MAXLEN];

    strftime (current_time, TIME_STR_MAXLEN, "%c", tmp);
    json_object_set (time_stmp, "Time", json_string (current_time));

    return 0;
}
