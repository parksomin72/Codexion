#include "main.h"

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

int all_coders_done(t_coder *coder, t_info *info)
{
    int i;
    int count;

    i = 0;
    while (i < info->number_of_coders){
        pthread_mutex_lock(&coder[i].state_m);
        count = coder[i].compile_count;
        pthread_mutex_unlock(&coder[i].state_m);
        if (count < info->number_of_compiles_required)
            return (0);
        i++;
    }
    return (1);
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

int main(int ac, char **av) {
    int n;
    t_info info;
    t_coder *coder;
    t_monitor monitor;
    
    if (ac != 9 || !check(av, &info) ||  !convert(av, &info))
    {
        write(2, "Error\n", 6);
        return (1);
    }
    if (!allocate(&info, &coder, &n))
        return (1);
    pthread_mutex_init(&info.print_m, NULL);
    info.start_time = get_time_ms();
    if (!init_info(&info, coder, &monitor, n))
    {
        free(info.dongles);
        free(coder);
        return (1);
    }
    destroy(&info, coder, n);
    free(info.dongles);
    free(coder);
    return (0);
}