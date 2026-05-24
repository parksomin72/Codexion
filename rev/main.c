#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define FIFO 0
#define EDF 1

typedef struct s_info {
    int scheduler;
}t_info;

int check(char **av, t_info *info) {
    int i = 1;

    while (i < 7) {
        int j = 0;
        if (!av[i][j]){
            return (0);
        }
        if (av[i][j] == '+')
            j++;
        while (av[i][j]){
            if (!(av[i][i] ==  '0' || av[i][j] == '9'))
                return (1);
            j++;
        }
        i++;
    }
    if (strcmp(av[8], "fifo") == 0) {
        info->scheduler = FIFO;
        return (1);
    }
    if (strcmp(av[8], "edf") == 0) {
        info->scheduler = EDF;
        return (1);
    }
    else {
        return (0);
    }

    return (1);
}

int main(int ac, char **av) {
    t_info info;

    if (ac < 2 || !check(av, &info)) {
        write(2, "Error\n", 6);
        return (1);
    }
    printf("every thing is a good!, scheduler = %d\n", info.scheduler);
    return (0);

}