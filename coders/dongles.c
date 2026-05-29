#include "main.h"

void wake_all_dongles(t_info *info)
{
    int i;

    i = 0;
    while (i < info->number_of_coders)
    {
        pthread_mutex_lock(&info->dongles[i].m);
        pthread_cond_broadcast(&info->dongles[i].cond);
        pthread_mutex_unlock(&info->dongles[i].m);
        i++;
    }
}

int take_one_dongle(t_coder *coder, int index)
{
    t_dongle   *dongle;
    t_request  request;

    dongle = &coder->info->dongles[index];
    request = make_request(coder);

    pthread_mutex_lock(&dongle->m);
    if (!heap_push(&dongle->queue, request, coder->info->scheduler))
    {
        pthread_mutex_unlock(&dongle->m);
        return (0);
    }
    while (simulation_running(coder)
    && (!dongle->dongle_available
    || cooldown_not_finished(dongle, coder->info)
    || !is_my_turn(&dongle->queue, coder->id)))
    {
        if (!dongle->dongle_available || !is_my_turn(&dongle->queue, coder->id))
            pthread_cond_wait(&dongle->cond, &dongle->m);
        else
        {
            pthread_mutex_unlock(&dongle->m);
            usleep(100);
            pthread_mutex_lock(&dongle->m);
        }
    }
    if (!simulation_running(coder))
    {
        remove_request(&dongle->queue, coder->id);
        pthread_mutex_unlock(&dongle->m);
        return (0);
    }
    pop_front(&dongle->queue);
    dongle->dongle_available = 0;
    pthread_mutex_unlock(&dongle->m);
    return (1);
}

void release_one_dongle(t_coder *coder, int index)
{
    t_dongle *dongle;

    dongle = &coder->info->dongles[index];

    pthread_mutex_lock(&dongle->m);

    dongle->dongle_available = 1;
    dongle->last_release_time = get_time_ms() - coder->info->start_time;

    pthread_cond_broadcast(&dongle->cond);

    pthread_mutex_unlock(&dongle->m);
}

int take_dongles(t_coder *coder)
{
    int first;
    int second;

    if (coder->id % 2 == 0)
    {
        first = coder->left_index;
        second = coder->right_index;
    }
    else
    {
        first = coder->right_index;
        second = coder->left_index;
    }
    if (!take_one_dongle(coder, first))
        return (0);
    if (!take_one_dongle(coder, second))
    {
        release_one_dongle(coder, first);
        return (0);
    }
    return (1);
}

void release_dongles(t_coder *coder)
{
    if (coder->id % 2 == 0)
    {
        release_one_dongle(coder, coder->right_index);
        release_one_dongle(coder, coder->left_index);        
    }
    else
    {
        release_one_dongle(coder, coder->left_index);
        release_one_dongle(coder, coder->right_index);
    }
}