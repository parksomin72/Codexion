#include <pthread.h>
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

    pthread_mutex_t print_m;
    t_dongle *dongles;
} t_info;

typedef struct s_coder {
    int id;
    int left_index;
    int right_index;
    long last_compile_start;
    int compile_count;
    pthread_t thread;
    t_info *info;
} t_coder;


void init_index(t_coder *coder, int n) {
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
    pthread_mutex_lock(&coder->info->print_m);
    printf("%d %s\n", coder->id, msg);
    pthread_mutex_unlock(&coder->info->print_m);
}

void debug_and_refactor(t_coder *coder)
{
    print_msg(coder, "is debugging");
    usleep(coder->info->time_to_debug * 1000);
    
    print_msg(coder, "is refactoring");
    usleep(coder->info->time_to_refactor * 1000);
}

void *routine(void *arg) {
    t_coder *coder = (t_coder *)arg;


    while (coder->compile_count < coder->info->number_of_compiles_required)
    {
        if (coder->id % 2 == 0)
        {
            pthread_mutex_lock(&coder->info->dongles[coder->left_index].m);
            pthread_mutex_lock(&coder->info->dongles[coder->right_index].m);
    
            pthread_mutex_lock(&coder->info->print_m);
            printf("%d has taken a dongle\n", coder->id);
            printf("%d has taken a dongle\n", coder->id);
            printf("%d is compiling\n", coder->id);
            pthread_mutex_unlock(&coder->info->print_m);
    
            usleep(coder->info->time_to_compile * 1000);
            coder->compile_count++;
            
            pthread_mutex_unlock(&coder->info->dongles[coder->right_index].m);
            pthread_mutex_unlock(&coder->info->dongles[coder->left_index].m);
            debug_and_refactor(coder);
        }
    
        else {
            pthread_mutex_lock(&coder->info->dongles[coder->right_index].m);
            pthread_mutex_lock(&coder->info->dongles[coder->left_index].m);
    
            pthread_mutex_lock(&coder->info->print_m);
            printf("%d has taken a dongle\n", coder->id);
            printf("%d has taken a dongle\n", coder->id);
            printf("%d is compiling\n", coder->id);
            pthread_mutex_unlock(&coder->info->print_m);
    
            usleep(coder->info->time_to_compile * 1000);        
            coder->compile_count++;
            
            pthread_mutex_unlock(&coder->info->dongles[coder->left_index].m);
            pthread_mutex_unlock(&coder->info->dongles[coder->right_index].m);
            debug_and_refactor(coder);
        }
    }

    return NULL;
}

void init_info(t_info *info, t_coder *coder, int n)
{
    int i;

    
    init_index(coder, n);
    i = 0;
    while (i < n)
    {
        pthread_mutex_init(&info->dongles[i].m, NULL);
        i++;
    }
    i = 0;
    while (i < n)
    {
        coder[i].id = i + 1;
        coder[i].compile_count = 0;
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
    coder = malloc(sizeof(t_coder) * n);
    info.dongles = malloc(sizeof(t_dongle) * n);

    if (!coder || !info.dongles) {
        return (1);
    }

    pthread_mutex_init(&info.print_m, NULL);
    init_info(&info, coder, n);
    
    pthread_mutex_destroy(&info.print_m);
    for (int i = 0; i < n; i++) {
        pthread_mutex_destroy(&info.dongles[i].m);
    }

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