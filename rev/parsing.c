#include "main.h"

void assign_info(int *inf, t_info *info)
{
    info->number_of_coders = inf[0];
    info->time_to_burnout = inf[1];
    info->time_to_compile = inf[2];
    info->time_to_debug = inf[3];
    info->time_to_refactor = inf[4];
    info->number_of_compiles_required = inf[5];
    info->dongle_cooldown = inf[6];
}

int check_scheduler(char *s, t_info *info)
{
    if (strcmp(s, "fifo") == 0)
    {
        info->scheduler = FIFO;
        return (1);
    }
    else if (strcmp(s, "edf") == 0)
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
    while (i < 8)
    {
        j = 0;
        if (!av[i][j])
            return (0);
        if (av[i][j] == '+' && av[i][j + 1])
            j++;
        while (av[i][j])
        {
            if (!(av[i][j] >= '0' && av[i][j] <= '9'))
                return (0);
            j++;
        }
        i++;
    }
    if (!check_scheduler(av[8], info))
        return (0);
    return (1);
}

int ft_atoi(char *s, int *flag)
{
    long result;

    result = 0;
    if (*s == '+')
        s++;
    while (*s >= '0' && *s <= '9')
    {
        result = result * 10 + (*s - '0');
        if (result > 2147483647)
        {
            *flag = 0;
            return (0);
        }
        s++;
    }
    return ((int)result);
}

int convert(char **av, t_info *info)
{
    int i;
    int n;
    int flag;
    int inf[7];

    i = 1;
    while (i < 8)
    {
        flag = 1;
        n = ft_atoi(av[i], &flag);
        if (!flag)
            return (0);
        if (i <= 6 && n <= 0)
            return (0);
        if (i == 7 && n < 0)
            return (0);
        inf[i - 1] = n;
        i++;
    }
    assign_info(inf, info);
    return (1);
}