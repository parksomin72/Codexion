#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define FIFO 0
#define EDF 1


typedef struct s_dongle {
    pthread_mutex_t m;
} t_dongle;

typedef struct s_info {
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    int scheduler;
    int simulation_stop;
    long start_time;

    pthread_mutex_t print_m;
    pthread_mutex_t stop_m;
    t_dongle *dongles;
} t_info;

typedef struct s_coder {
    int id;
    int left_index;
    int right_index;
    long last_compile_start;
    int compile_count;
    pthread_t thread;
    pthread_mutex_t state_m;
    t_info *info;
} t_coder;



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

void print_msg(t_coder *coder, char *msg)
{
    long time_now;

    time_now = get_time_ms() - coder->info->start_time;

    pthread_mutex_lock(&coder->info->print_m);
    printf("%ld %d %s\n", time_now, coder->id, msg);
    pthread_mutex_unlock(&coder->info->print_m);
}

void debug_and_refactor(t_coder *coder)
{
    print_msg(coder, "is debugging");
    usleep(coder->info->time_to_debug * 1000);
    
    print_msg(coder, "is refactoring");
    usleep(coder->info->time_to_refactor * 1000);
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

void check_simulation_state(t_coder *coder)
{
    long last;
    long now;
    
    pthread_mutex_lock(&coder->state_m);
    last = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_m);
    
    now = get_time_ms() - coder->info->start_time;

    if (now - last >= coder->info->time_to_burnout)
        set_simulation_stop(coder->info);
     
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
            debug_and_refactor(coder);
    }
    
    return NULL;
}

void init_info(t_info *info, t_coder *coder, int n)
{
    int i;

    
    i = 0;
    init_index(coder, n);
    pthread_mutex_init(&info->stop_m, NULL);
    while (i < n)
    {
        pthread_mutex_init(&info->dongles[i].m, NULL);
        pthread_mutex_init(&coder[i].state_m, NULL);
        i++;
    }
    i = 0;
    while (i < n)
    {
        coder[i].id = i + 1;
        coder[i].compile_count = 0;
        coder[i].last_compile_start = 0;
        coder[i].info = info;
        pthread_create(&coder[i].thread, NULL, routine, (void *)&coder[i]);
        i++;
    }
    
    i = 0;
    while (i < n)
    {
        pthread_join(coder[i].thread, NULL);
        i++;
    }
}

int main() {
    int n;

    scanf("%d", &n);
    t_info info;
    t_coder *coder;
    
    info.number_of_coders = n;
    info.time_to_compile = 100;
    info.time_to_debug = 100;
    info.time_to_refactor = 100;
    info.number_of_compiles_required = 1;
    info.simulation_stop = 0;
    coder = malloc(sizeof(t_coder) * n);
    info.dongles = malloc(sizeof(t_dongle) * n);
    
    if (!coder || !info.dongles) {
        return (1);
    }
    
    pthread_mutex_init(&info.print_m, NULL);
    info.start_time = get_time_ms();
    init_info(&info, coder, n);
    
    pthread_mutex_destroy(&info.print_m);
    for (int i = 0; i < n; i++) {
        pthread_mutex_destroy(&info.dongles[i].m);
        pthread_mutex_destroy(&coder[i].state_m);
    }
    pthread_mutex_destroy(&info.stop_m);

    free(info.dongles);
    free(coder);

    return (0);
}


































// typedef struct s_data {
//     int x;
//     pthread_mutex_t m;
// } t_data;

// void *routine(void *args) {
//     t_data *arg = (t_data *)args;
//     pthread_mutex_lock(&arg->m);
//     for(int i = 0; i < 1000; i++)
//         arg->x++;
//     pthread_mutex_unlock(&arg->m);
//     return NULL;
// }

// int main() {
//     t_data s;
//     s.x = 0;
//     pthread_t t1, t2;
//     pthread_mutex_init(&s.m, NULL);

//     pthread_create(&t1, NULL, routine, (void *)&s);
//     pthread_create(&t2, NULL, routine, (void *)&s);
//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
//     printf("final = %d\n", s.x);
//     pthread_mutex_destroy(&s.m);
// }