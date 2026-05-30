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
void init_index(t_coder *coder, int n);
void *routine(void *arg);
void *monitor_routine(void *arg);
void has_one_coder(t_coder *coder);
void swap_request(t_request *a, t_request *b);
void release_one_dongle(t_coder *coder, int index);
void release_dongles(t_coder *coder);
void heap_pop(t_heap *heap, int scheduler);
void check_simulation_state(t_coder *coder);
void set_simulation_stop(t_info *info);
void destroy(t_info *info, t_coder *coder, int n);
void create_coder_thread(t_coder *coder, t_info *info, int n);
void print_msg(t_coder *coder, char *msg);
void debug_and_refactor(t_coder *coder);
void wake_all_dongles(t_info *info);
void heap_bubble_down(t_heap *heap, int i, int scheduler);
void remove_request(t_heap *heap, int coder_id, int scheduler);

int parent_index(int i);
int left_child(int i);
int right_child(int i);

int     check(char **av, t_info *info);
int     convert(char **av, t_info *coders_info);
int cooldown_not_finished(t_dongle *dongle, t_info *info);
int init_info(t_info *info, t_coder *coder, t_monitor *monitor, int n);
int init_mutex(t_coder *coder, t_info *info, int n);
int request_has_priority(t_request a, t_request b, int scheduler);
int is_my_turn(t_heap *heap, int coder_id);
int heap_push(t_heap *heap, t_request request, int scheduler);
int simulation_running(t_coder *coder);
int allocate(t_info *info, t_coder **coder, int *n);
int has_more_compiles(t_coder *coder);
int all_coders_done(t_coder *coder, t_info *info);
int take_one_dongle(t_coder *coder, int index);
int take_dongles(t_coder *coder);



long    get_time_ms();
t_request make_request(t_coder *coder);

#endif