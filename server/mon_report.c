#include <jansson.h>
#define BLANC_JSON_REPORT "{\"CPU, %\": [],\"RAM\": {}, \"Storage\": [], \"Network\" : []}"

int cpu_usage (json_t *cpu_state);
int storage_usage (json_t *storage_state);
int ram_usage (json_t *ram_state);
int net_usage (json_t *net_state);
int time_stamp(json_t *time_stmp);

char *system_state_report ()
{
    json_t *root = json_loads (BLANC_JSON_REPORT, 0, NULL);

    cpu_usage (json_object_get(root, "CPU, %"));
    ram_usage (json_object_get(root, "RAM"));
    storage_usage (json_object_get(root, "Storage"));
    net_usage (json_object_get(root, "Network"));

    time_stamp (root);

    char *s = json_dumps (root, 0);
    // FIXME The return value must be freed by the caller using free().
        
    json_decref (root);
    return s;
}
