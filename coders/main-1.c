#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

    if (strcmp(av[i], "fifo") != 0 && strcmp(av[i], "edf") != 0)
        return (0);
    return (1);
}

char *ft_strcpy(char *dest, char *src) {
    char *tmp;

    tmp = dest;
    while (*src != '\0') {
        *tmp++ = *src++;
    }
    *tmp = '\0';
    return (dest);
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
    ft_strcpy(coders_info->scheduler, av[8]);
    return (1);
}

int main(int ac, char **av)
{
    t_info coder_info;

    if (ac != 9 || !check(av) ||  !convert(av, &coder_info)) {
        write(2, "Error\n", 6);
        return (1);
    }

    printf("ok!\n"); // just for test now
}