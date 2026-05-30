#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define FIFO 0
#define EDF 1

typedef struct s_info {
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    int scheduler;
    
}t_info;

/* Prototype of functions*/
int check(char **av, t_info *info);
int convert(char **av, t_info *info);

#endif