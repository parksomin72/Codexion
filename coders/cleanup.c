#include "main.h"

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

void destroy(t_info *info, t_coder *coder, int n)
{
    int i;

    i = 0;
    pthread_mutex_destroy(&info->print_m);
    while (i < n)
    {
        pthread_cond_destroy(&info->dongles[i].cond);
        free(info->dongles[i].queue.arr);
        pthread_mutex_destroy(&info->dongles[i].m);
        pthread_mutex_destroy(&coder[i].state_m);
        i++;
    }
    pthread_mutex_destroy(&info->stop_m);
}

int allocate(t_info *info, t_coder **coder, int *n)
{
    *n = info->number_of_coders;
    *coder = malloc(sizeof(t_coder) * (*n));
    info->dongles = malloc(sizeof(t_dongle) * (*n));
    
    if (!*coder || !info->dongles)
    {
        free(*coder);
        free(info->dongles);
        return (0);
    }
    return (1);
}