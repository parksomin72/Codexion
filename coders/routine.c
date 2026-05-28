#include "main.h"

void *monitor_routine(void *arg)
{
    int i;
    t_monitor *monitor;
    
    monitor = (t_monitor *)arg;
    while (simulation_running(monitor->coders))
    {
        i = 0;
        while (i < monitor->info->number_of_coders)
        {
            check_simulation_state(&monitor->coders[i]);
            if (!simulation_running(&monitor->coders[i])) {
                return NULL;
            }
            i++;
        }
        if (all_coders_done(monitor->coders, monitor->coders->info)) {
            set_simulation_stop(monitor->coders->info);
            return (NULL);
        }
        usleep(1000);
    }
    return (NULL);
}

void has_one_coder(t_coder *coder)
{
    take_one_dongle(coder, 0);
    print_msg(coder, "has taken a dongle");
    while (simulation_running(coder))
        usleep(100);
    release_one_dongle(coder, 0);
}

void *routine(void *arg)
{
    t_coder *coder = (t_coder *)arg;

    if (coder->info->number_of_coders == 1)
    {
        has_one_coder(coder);
        return (NULL);
    }
    while (has_more_compiles(coder) && simulation_running(coder))
    {
        take_dongles(coder);
        print_msg(coder, "has taken a dongle");
        print_msg(coder, "has taken a dongle");

        pthread_mutex_lock(&coder->state_m);
        coder->last_compile_start = get_time_ms() - coder->info->start_time;
        pthread_mutex_unlock(&coder->state_m);
        print_msg(coder, "is compiling");
        
        usleep(coder->info->time_to_compile * 1000);
        
        pthread_mutex_lock(&coder->state_m);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->state_m);
        
        release_dongles(coder);
        
        if (!simulation_running(coder) || !has_more_compiles(coder)) {
            return NULL;
        }
        debug_and_refactor(coder);
    }
    return NULL;
}