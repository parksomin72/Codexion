// #include <pthread.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>

// typedef struct s_data {
//     int dongle;
//     pthread_mutex_t m;
//     int time;
//     int num_coder;
// } t_data;

// typedef struct s_info {
//     int taken;
//     int id;
//     t_data *data;
// } t_info;

// void *routine(void *arg) {
//     t_info *coder = (t_info *)arg;
//     pthread_mutex_lock(&coder->data->m);
//     if (coder->data->dongle == 1) {
//         coder->data->dongle = 0;
//         coder->taken = 1;
//         printf("coder %d take the dongle\n", coder->id);
//         printf("coder %d is compiling\n", coder->id);
//     }
//     pthread_mutex_unlock(&coder->data->m);
    
//     if (coder->taken == 1) {
//         usleep(coder->data->time);
        
//         pthread_mutex_lock(&coder->data->m);
//         coder->data->dongle = 1;
//         printf("coder %d released the dongle\n", coder->id);
//         pthread_mutex_unlock(&coder->data->m);
//     }
//     return NULL;
// }

// int main(int ac, char **av) {
//     if (ac != 3) {
//         return (1);
//     }

//     t_data data;
//     int n = atoi(av[1]);
//     data.dongle = 1;
    
//     data.time = atoi(av[2]);
//     data.num_coder = n;
//     pthread_mutex_init(&data.m, NULL);
    
//     t_info coder[n];
//     pthread_t th[n];
//     for (int i = 0; i < n; i++) {
//         coder[i].taken = 0;
//         coder[i].data = &data;
//         coder[i].id = i + 1;
//         pthread_create(&th[i], NULL, routine, (void *)&coder[i]);
//     }
//     for (int i = 0; i < n; i++) {
//         pthread_join(th[i], NULL);
//     }

//     pthread_mutex_destroy(&data.m);
// }




#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_info {
    int dongle;
    int time;
    pthread_mutex_t m;
} t_info;

typedef struct s_data {
    int id;
    int taken;
    t_info *info;
} t_data;

void *routine(void *arg) {
    t_data *data = (t_data *)arg;

    while (data->taken == 0) {
        pthread_mutex_lock(&data->info->m);
        if (data->info->dongle == 1) {
            data->info->dongle = 0;
            data->taken = 1;
            printf("\nthe coder %d took the dongle\n", data->id);
        }
        pthread_mutex_unlock(&data->info->m);

        if (data->taken == 0)
            usleep(1000);
    }

    if (data->taken == 1)
        printf("coder %d is compiling\n", data->id);
    usleep(data->info->time);

    pthread_mutex_lock(&data->info->m);
    data->info->dongle = 1;
    printf("coder %d released dongle\n", data->id);
    pthread_mutex_unlock(&data->info->m);
    return NULL;
}


int main(int ac, char **av) {
    if (ac != 3) {
        write(2, "Error\n", 6);
        return (1);
    }

    int n = atoi(av[1]);
    t_info info;
    t_data data[n];
    pthread_mutex_init(&info.m, NULL);
    pthread_t coder[n];
    info.dongle = 1;
    info.time = atoi(av[2]);
    for (int i = 0; i < n; i++)
    {
        data[i].info = &info;
        data[i].id = i + 1;
        data[i].taken = 0;
        pthread_create(&coder[i], NULL, routine, (void *)&data[i]);
    }
    for (int i = 0; i < n; i++) {
        pthread_join(coder[i], NULL);
    }

    pthread_mutex_destroy(&info.m);
    return (0);
}