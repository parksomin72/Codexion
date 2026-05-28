#include "main.h"

int simulation_running(t_coder *coder)
{
    int n;

    pthread_mutex_lock(&coder->info->stop_m);
    n = coder->info->simulation_stop;
    pthread_mutex_unlock(&coder->info->stop_m);
    if (n)
        return (0);
    return (1);
}

void set_simulation_stop(t_info *info)
{
    pthread_mutex_lock(&info->stop_m);
    info->simulation_stop = 1;
    pthread_mutex_unlock(&info->stop_m);
}

void check_simulation_state(t_coder *coder)
{
    long last;
    long now;
    
    pthread_mutex_lock(&coder->state_m);
    last = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_m);
    
    now = get_time_ms() - coder->info->start_time;

    if (now - last >= coder->info->time_to_burnout) {
        print_msg(coder, "burned out");
        set_simulation_stop(coder->info);
    }
}