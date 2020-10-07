#include <jansson.h>
#include <string.h>
#include <stdio.h>

int status (const char *src, char *report)
{
    json_error_t error;
    json_t *root = json_loads (src, 0, &error);

    if(!root)
    {
        fprintf (stderr, "Error parsing JSON: on line %d: %s\n", error.line, error.text);
        return -1;
    }

    json_t *cpu_status = json_object_get (root, "CPU, %");
    json_t *ram_status = json_object_get (root, "RAM");
//    json_t *sto_status = json_object_get (root, "Storage");
//    json_t *net_status = json_object_get (root, "Network");

    if (!json_is_array (cpu_status))
    {
        fprintf (stderr, "Error: CPU value is not an array\n");
        json_decref (root);
        return -1;
    }

    json_t *tot_cpu_load = json_array_get (cpu_status, 0);

    if (!json_is_integer (tot_cpu_load))
    {
        fprintf(stderr, "Total CPU usage is not an integer\n");
        json_decref (root);
        return -1;
    }

    int ncpu = json_array_size (cpu_status);
    int tot_cpu_usage = json_integer_value (tot_cpu_load);

    char buff[256];

    sprintf (report, "Total usage of %2i CPUs: %3i%%, ", ncpu - 1, tot_cpu_usage);

    int mem_tot  = json_integer_value (json_object_get (ram_status, "Total"  ));
    int mem_free = json_integer_value (json_object_get (ram_status, "Free"   ));
    int mem_buff = json_integer_value (json_object_get (ram_status, "Buffers"));
    int mem_cach = json_integer_value (json_object_get (ram_status, "Cached" ));

    int mem_not_used = mem_free + mem_buff + mem_cach;
    int mem_used = mem_tot - mem_not_used;

    sprintf(buff, "Memory: %.1f MB used, %.1f MB free", mem_used/1024.0, mem_not_used/1024.0);

    strcat (report, buff);

    json_decref(root);
    return 0;
}
