#include "main.h"

long get_time_ms()
{
    struct timeval tv;
    long result;
    
    gettimeofday(&tv, NULL);
    result = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return (result);
}

int cooldown_not_finished(t_dongle *dongle, t_info *info)
{
    long now;

    now = get_time_ms() - info->start_time;
    if (now - dongle->last_release_time < info->dongle_cooldown)
        return (1);
    return (0);
}