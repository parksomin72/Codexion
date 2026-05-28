#include "main.h"


int init_mutex(t_coder *coder, t_info *info, int n)
{
    int i;

    i = 0;
    pthread_mutex_init(&info->stop_m, NULL);
    while (i < n)
    {
        info->dongles[i].dongle_available = 1;
        info->dongles[i].last_release_time = -info->dongle_cooldown;
        info->dongles[i].queue.capacity = info->number_of_coders;
        info->dongles[i].queue.size = 0;
        info->dongles[i].queue.arr = malloc(sizeof(t_request)
                * info->number_of_coders);
        if (!info->dongles[i].queue.arr)
            return (0);
        pthread_mutex_init(&info->dongles[i].m, NULL);
        pthread_cond_init(&info->dongles[i].cond, NULL);
        pthread_mutex_init(&coder[i].state_m, NULL);
        i++;
    }
    return (1);
}

void init_index(t_coder *coder, int n)
{
    int i;
    
    i = 0;
    while (i < n) {
        coder[i].left_index = i;
        coder[i].right_index = (i + 1) % n;
        i++;
    }
}

int init_info(t_info *info, t_coder *coder, t_monitor *monitor, int n)
{
    int         i;
    pthread_t  monitor_t;

    i = 0;
    info->simulation_stop = 0;
    info->death_printed = 0;
    monitor->info = info;
    monitor->coders = coder;
    init_index(coder, n);
    if (!init_mutex(coder, info, n))
        return (0);
    create_coder_thread(coder, info, n);
    pthread_create(&monitor_t, NULL, monitor_routine, (void *)monitor);
    while (i < n)
    {
        pthread_join(coder[i].thread, NULL);
        i++;
    }
    set_simulation_stop(info);
    pthread_join(monitor_t, NULL);
    return (1);
}