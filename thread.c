#include <pthread.h>
#include <stdio.h>

typedef struct s_data {
    int x;
    pthread_mutex_t m;
} t_data;

void *routine(void *args) {
    t_data *arg = (t_data *)args;
    pthread_mutex_lock(&arg->m);
    for(int i = 0; i < 1000; i++)
        arg->x++;
    pthread_mutex_unlock(&arg->m);
    return NULL;
}

int main() {
    t_data s;
    s.x = 0;
    pthread_t t1, t2;
    pthread_mutex_init(&s.m, NULL);

    pthread_create(&t1, NULL, routine, (void *)&s);
    pthread_create(&t2, NULL, routine, (void *)&s);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("final = %d\n", s.x);
    pthread_mutex_destroy(&s.m);
}