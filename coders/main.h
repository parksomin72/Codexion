#ifndef MAIN_H
# define MAIN_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <stddef.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>

# define FIFO 0
# define EDF 1

typedef struct s_info t_info;
typedef struct s_coder t_coder;

typedef struct s_request
{
    int     coder_id;
    long    request_time;
    long    deadline;
    t_coder *coder;
}   t_request;

typedef struct s_heap
{
    t_request  *arr;
    int         size;
    int         capacity;
}   t_heap;

typedef struct s_dongle
{
    int             dongle_available;
    long            last_release_time;
    pthread_mutex_t m;
    pthread_cond_t  cond;
    t_heap          queue;
}   t_dongle;

struct s_info
{
    int             number_of_coders;
    int             time_to_burnout;
    int             time_to_compile;
    int             time_to_debug;
    int             time_to_refactor;
    int             number_of_compiles_required;
    int             dongle_cooldown;
    int             scheduler;
    int             simulation_stop;
    long            start_time;
    pthread_mutex_t print_m;
    int             death_printed;
    pthread_mutex_t stop_m;
    t_dongle        *dongles;
};

struct s_coder
{
    int             id;
    int             left_index;
    int             right_index;
    long            last_compile_start;
    int             compile_count;
    pthread_t       thread;
    pthread_mutex_t state_m;
    t_info          *info;
};

typedef struct s_monitor
{
    t_info  *info;
    t_coder *coders;
}   t_monitor;

/* Prototypes */
// int init_info(t_info *info, t_coder *coder, t_monitor *monitor, int n)

int     check(char **av, t_info *info);
int     convert(char **av, t_info *coders_info);
long    get_time_ms(void);
t_request make_request(t_coder *coder);

#endif