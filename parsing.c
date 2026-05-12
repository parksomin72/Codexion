#include "coders.h"

int fall_scheduler(char *s, t_info *info)
{
    if (strcmp(s, "fifo") == 0)
        info->scheduler = 0;
    else if (strcmp(s, "edf") == 0)
        info->scheduler = 1;
    else
        return (0);
    return (1);
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
        while (av[i][j]) {
            if (!(av[i][j] >= '0' && av[i][j] <= '9'))
                return (0);
            j++;
        }
        i++;
    }
    if (!fall_scheduler(av[8], info))
        return (0);
    return (1);
}


int ft_atoi(char *s, int *flag)
{
    long res;

    res = 0;
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
        if (flag == 1 || n <= 0)
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

int allocate(t_info *info, t_coder *coder, int size)
{
    coder = malloc(sizeof(t_coder) * size);
    if (!coder)
        return (0);
    info.dongles = malloc(sizeof(t_dongles) * size);
    if (!info.dongles) {
        free(coder);
        return (0);
    }
    return (1);
}

int main(int ac, char **av)
{
    int n;
    t_info info;
    t_coder *coder;
    t_monitor monitor;

    if (ac != 9 || !check(av, &info) ||  !convert(av, &info)) {
        write(2, "Error\n", 6);
        return (1);
    }
    n = info.number_of_coders;
    printf("number_of_coders = %d\n", n);

}