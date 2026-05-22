#ifndef _MAIN_H_
#define _MAIN_H_

#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FIFO 0
#define EDF 1


typedef struct s_dongle {
    int dongle_available;
    long last_release_time;
    pthread_mutex_t m;
    pthread_cond_t cond;
    // t_heap queue;
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
    
    pthread_mutex_t print_m; //protect print
    int death_printed;
    pthread_mutex_t stop_m; //protect stop simulation check
    t_dongle *dongles;
} t_info;

typedef struct s_coder {
    int id;
    int left_index;
    int right_index;
    long last_compile_start;
    int compile_count;
    pthread_t thread;
    pthread_mutex_t state_m; //protect check coder state
    t_info *info;
} t_coder;

typedef struct s_monitor {
    t_info *info;
    t_coder *coders;
} t_monitor;

typedef struct s_request {
    int coder_id;
    long request_time;
    long deadline;
    t_coder *coder;
} t_request;

/* Prototype of functions */
int check(char **av, t_info *info);
int convert(char **av, t_info *coders_info);
long get_time_ms();


#endif