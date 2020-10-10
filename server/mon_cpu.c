#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <jansson.h>

#define TXT_BUFFER_SIZE 256
#define TIME_LAG 1
#define STAT_PATH "/proc/stat"

int cpu_usage (json_t *cpu_state)
{
    char buff[TXT_BUFFER_SIZE][TXT_BUFFER_SIZE];
    int ncpu = get_nprocs ();

    FILE* fp = fopen(STAT_PATH,"r");
    for (int i = 0; i < ncpu + 1; i++)
    {
        fgets(buff[i], TXT_BUFFER_SIZE, fp);
    }
    fclose(fp);

    sleep(TIME_LAG);

    fp = fopen(STAT_PATH,"r");
    for (int i = 0; i < ncpu + 1; i++)
    {
        fgets(buff[i + ncpu + 1], TXT_BUFFER_SIZE, fp);
    }
    fclose(fp);

    for (int i = 0; i < ncpu + 1; i++)
    {
        long long sum = 0, lastSum = 0;
        long long idle, lastIdle;

        char* token = strtok(buff[i], " ");

        for (int col = 0; token != NULL;)
        {
            token = strtok (NULL, " ");
            if (token != NULL)
            {
                lastSum += atoll (token);
                if (col == 3)
                    lastIdle = atoll (token);
                col++;
            }
        }

        token = strtok (buff[i + ncpu + 1], " ");

        for (int col = 0; token != NULL;)
        {
            token = strtok (NULL, " ");
            if (token != NULL)
            {
                sum += atoll (token);
                if (col == 3)
                    idle = atoll (token);
                col++;
            }
        }

        int cpu_usage_pct = (1000 *((sum - lastSum) - (idle - lastIdle)) / (sum - lastSum) + 5) / 10;
        json_t *json_cpu_pct;
        json_cpu_pct = json_integer(cpu_usage_pct);
        json_array_append (cpu_state, json_cpu_pct);
        json_decref (json_cpu_pct);
    }
    return 0;
}

