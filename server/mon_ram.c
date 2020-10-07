#include <string.h>
#include <jansson.h>

#define MEMINFO_PATH "/proc/meminfo"
#define MEMINFO_LINE_MAXLEN 80
#define MEM_UNIT_MAXLEN 8

int ram_usage (json_t *ram_state)
{
    /*
        #include <sys/sysinfo.h>

        struct sysinfo info;
        sysinfo (&info);

        json_t *meminfo = json_pack ("{s:I, s:I, s:I, s:I, s:I}",
            "Total",            info.totalram  * info.mem_unit,
            "Free",             info.freeram   * info.mem_unit,
            "Buffers",          info.bufferram * info.mem_unit,
            "Total swap space", info.totalswap * info.mem_unit,
            "Free swap space",  info.freeswap  * info.mem_unit);
        json_object_set(ram_state, "Usage, MB", meminfo);
    */
    FILE* fp = fopen (MEMINFO_PATH, "r");

    char str[MEMINFO_LINE_MAXLEN];
    char mem_unit[MEM_UNIT_MAXLEN];

    long long unsigned int val,
         mem_total,   mem_free,   mem_available,
         mem_buffers, mem_cached,
         swap_total, swap_free;

    for (; fgets (str, sizeof (str), fp) != NULL;)
    {
        if (sscanf (str, "MemTotal: %Lu %s \n", &val, mem_unit))
        {
            mem_total = val;
        }
        if (sscanf (str, "MemFree: %Lu %*s \n", &val))
        {
            mem_free = val;
        }
        if (sscanf (str, "MemAvailable: %Lu %*s \n", &val))
        {
            mem_available = val;
        }
        if (sscanf (str, "Buffers: %Lu %*s \n", &val))
        {
            mem_buffers = val;
        }
        if (sscanf (str, "Cached: %Lu %*s \n", &val))
        {
            mem_cached = val;
        }
        if (sscanf (str, "SwapTotal: %Lu %*s \n", &val))
        {
            swap_total = val;
        }
        if (sscanf (str, "SwapFree: %Lu %*s \n", &val))
        {
            swap_free = val;
        }
    }

    fclose (fp);

    json_t *meminfo = json_pack ("{s:I, s:I, s:I, s:I, s:I, s:I, s:I, s:s}",
                                 "Total",         mem_total, "Free",       mem_free,
                                 "Available", mem_available, "Buffers",    mem_buffers,
                                 "Cached",       mem_cached, "Total swap", swap_total,
                                 "Free swap",     swap_free, "Unit",       mem_unit);

    json_object_update_missing (ram_state, meminfo);

    return 0;
}
