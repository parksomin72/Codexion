#include "main.h"

int check_scheduler(char *av, t_info *info)
{
    if (strcmp(av, "fifo") == 0)
    {
        info->scheduler = FIFO;
        return (1);
    }
    else if (strcmp(av, "edf") == 0)
    {
        info->scheduler = EDF;
        return (1);
    }
    return (0);
}

int check(char **av, t_info *info)
{
    int i;
    int j;

    i = 1;
    while (i <= 7) {
        j = 0;
        if (!av[i][j])
            return (0);
        if (av[i][j] == '+')
            j++;
        while (av[i][j]) {
            if (!(av[i][j] >= '0' && av[i][j] <= '9'))
                return (0);
            j++;
        }
        i++;
    }

    if (!check_scheduler(av[i], info))
        return (0);
    return (1);
}


int ft_atoi(char *s, int *flag)
{
    long res;

    res = 0;
    if (*s == '+')
        s++;
    while (*s >= '0' && *s <= '9')
    {
        res = res * 10 + (*s - '0');
        if (res > 2147483647) {
            *flag = 1;
            return (0);
        }
        s++;
    }
    return ((int)res);
}

int convert(char **av, t_info *coders_info)
{
    int i;
    int flag;
    int n;
    int info[7];

    i = 0;
    flag = 0;
    n = 0;
    while (i < 7) {
        n = ft_atoi(av[i + 1], &flag);
        if (flag == 1 || (i != 6 && n <= 0))
            return (0);
        info[i] = n;
        i++;
    }

    coders_info->number_of_coders = info[0];
    coders_info->time_to_burnout = info[1];
    coders_info->time_to_compile = info[2];
    coders_info->time_to_debug = info[3];
    coders_info->time_to_refactor = info[4];
    coders_info->number_of_compiles_required = info[5];
    coders_info->dongle_cooldown = info[6];
    return (1);
}