#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct s_data {
    int x;
    pthread_mutex_t m;
}t_data;
void *routine(void *arg) {
    t_data *data = (t_data *)arg;

    pthread_mutex_lock(&(data->m));
    data->x++;
    printf("coder %d compiling\n", data->x);
    pthread_mutex_unlock(&(data->m));
    return NULL;
}

int main(int ac, char **av) {
    if (ac != 2) {
        printf("Error\n");
        return (1);
    } 

    pthread_t t1, t2;
    t_data j;
    int arg = atoi(av[1]);
    pthread_mutex_init(&j.m, NULL);
    j.x = arg;
    pthread_create(&t1, NULL, routine, (void *)&j);
    pthread_create(&t2, NULL, routine, (void *)&j);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    pthread_mutex_destroy(&j.m);

}