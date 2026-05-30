#include "main.h"

int main(int ac, char **av)
{
    t_info info;

    if (ac != 9 || !check(av, &info) || !convert(av, &info))
    {
        write(2, "Error\n", 6);
        return (1);
    }
    write(1, "OK!\n", 4);
    return (0);
}