#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_info {
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    char scheduler[10];
} t_info;

int check(char **av)
{
    int i;
    int j;

    i = 1;
    while (i < 8)
    {
        j = 0;
        if (!av[i][j])
            return (0);
        while (av[i][j])
        {
            if (!(av[i][j] >= '0' && av[i][j] <= '9'))
                return (0);
            j++;
        }
        i++;
    }
    if (strcmp(av[i], "fifo") != 0 && strcmp(av[i], "edf") != 0)
        return (0);

    return (1);
}

char *ft_strcpy(char *src, t_info *coder)
{
    int i;

    i = 0;
    while (src[i])
    {
        coder->scheduler[i] = src[i];
        i++;
    }
    coder->scheduler[i] = '\0';
    return (coder->scheduler);
}

int fill(t_info *coder, char **av)
{
    char *tmp;
    int n;
    int info[7];
    int i;

    i = 0;
    while (i < 7)
    {
        n = atoi(av[i + 1]); 
        if (n <= 0 || n >= 2147483647)
            return (0);
        info[i] = n;
        i++;
    }
    coder->number_of_coders = info[0];
    coder->time_to_burnout = info[1];
    coder->time_to_compile = info[2];
    coder->time_to_debug = info[3];
    coder->time_to_refactor = info[4];
    coder->number_of_compiles_required = info[5];
    coder->dongle_cooldown = info[6];
    tmp = av[8];
    ft_strcpy(tmp, coder);
    return (1);
}


#include <stdio.h>
int main(int ac, char **av)
{
    t_info coder;

    if (ac != 9 || !check(av) || !fill(&coder, av))
        return (1);

    printf("%d\n", coder.number_of_coders);
    printf("%d\n", coder.time_to_burnout);
    printf("%d\n", coder.time_to_compile);
    printf("%d\n", coder.time_to_debug);
    printf("%d\n", coder.time_to_refactor);
    printf("%d\n", coder.number_of_compiles_required);
    printf("%d\n", coder.dongle_cooldown);
    printf("%s\n", coder.scheduler);

    return (0);
}