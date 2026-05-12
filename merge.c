#include "coders.h"

long get_time_ms()
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
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

void take_dongles(t_coder *coder)
{
    if (coder->id % 2 == 0)
    {
        pthread_mutex_lock(&coder->info->dongles[coder->left_index].m);
        pthread_mutex_lock(&coder->info->dongles[coder->right_index].m);
    }
    else
    {
        pthread_mutex_lock(&coder->info->dongles[coder->right_index].m);
        pthread_mutex_lock(&coder->info->dongles[coder->left_index].m);
    }
}

void release_dongles(t_coder *coder)
{
    if (coder->id % 2 == 0)
    {
        pthread_mutex_unlock(&coder->info->dongles[coder->right_index].m);
        pthread_mutex_unlock(&coder->info->dongles[coder->left_index].m);
    }
    else
    {
        pthread_mutex_unlock(&coder->info->dongles[coder->left_index].m);
        pthread_mutex_unlock(&coder->info->dongles[coder->right_index].m);
    }
}

int has_more_compiles(t_coder *coder)
{
    int n;

    pthread_mutex_lock(&coder->state_m);
    n = coder->compile_count;
    pthread_mutex_unlock(&coder->state_m);
    if (n < coder->info->number_of_compiles_required)
        return (1);
    return (0);
}

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


void print_msg(t_coder *coder, char *msg)
{
    long time_now;
    
    time_now = get_time_ms() - coder->info->start_time;
    
    pthread_mutex_lock(&coder->info->print_m);
    if (strcmp("burned out", msg) == 0 && !coder->info->death_printed) {
        printf("%ld %d %s\n", time_now, coder->id, msg);
        coder->info->death_printed = 1;
    }
    else if (!coder->info->death_printed)
    printf("%ld %d %s\n", time_now, coder->id, msg);
    pthread_mutex_unlock(&coder->info->print_m);
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

void debug_and_refactor(t_coder *coder)
{
    print_msg(coder, "is debugging");
    usleep(coder->info->time_to_debug * 1000);
    if (!simulation_running(coder)) {
            return ;
    }
    
    print_msg(coder, "is refactoring");
    usleep(coder->info->time_to_refactor * 1000);
    if (!simulation_running(coder)) {
            return ;
    }
}


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
        usleep(1000);
    }
    return (NULL);
}

void *routine(void *arg)
{
    t_coder *coder = (t_coder *)arg;
    
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
        if (!simulation_running(coder))
            return NULL;
        debug_and_refactor(coder);
    }
    return NULL;
}

void init_mutex(t_coder *coder, t_info *info, int n)
{
    int i;

    i = 0;
    pthread_mutex_init(&info->stop_m, NULL);
    while (i < n)
    {
        pthread_mutex_init(&info->dongles[i].m, NULL);
        pthread_mutex_init(&coder[i].state_m, NULL);
        i++;
    }
}

void create_coder_thread(t_coder *coder, t_info *info, int n)
{
    int i;

    i = 0;
    info->death_printed = 0;
    while (i < n)
    {
        coder[i].id = i + 1;
        coder[i].compile_count = 0;
        coder[i].last_compile_start = 0;
        coder[i].info = info;
        pthread_create(&coder[i].thread, NULL, routine, (void *)&coder[i]);
        i++;
    }
}

void init_info(t_info *info, t_coder *coder, t_monitor *monitor, int n)
{
    int i;
    pthread_t monitor_t;

    i = 0;
    init_index(coder, n);
    init_mutex(coder, info, n);
    create_coder_thread(coder, info, n);
    pthread_create(&monitor_t, NULL, monitor_routine, (void *)monitor);
    while (i < n)
    {
        pthread_join(coder[i].thread, NULL);
        i++;
    }
    set_simulation_stop(info);
    pthread_join(monitor_t, NULL);
}

