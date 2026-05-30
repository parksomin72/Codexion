#include "main.h"

int parent_index(int i)
{
    return ((i - 1) / 2);
}

int left_child(int i)
{
    return (2 * i + 1);
}

int right_child(int i)
{
    return (2 * i + 2);
}