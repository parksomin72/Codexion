#include "main.h"

void swap_request(t_request *a, t_request *b)
{
    t_request tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

t_request make_request(t_coder *coder)
{
    t_request request;
    long      last_compile;

    pthread_mutex_lock(&coder->state_m);
    last_compile = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_m);

    request.coder_id = coder->id;
    request.request_time = get_time_ms() - coder->info->start_time;
    request.deadline = last_compile + coder->info->time_to_burnout;
    request.coder = coder;
    return (request);
}

int request_has_priority(t_request a, t_request b, int scheduler)
{
    if (scheduler == FIFO)
    {
        if (a.request_time < b.request_time)
            return (1);
        if (a.request_time == b.request_time && a.coder_id < b.coder_id)
            return (1);
        return (0);
    }
    if (scheduler == EDF)
    {
        if (a.deadline < b.deadline)
            return (1);
        if (a.deadline == b.deadline && a.request_time < b.request_time)
            return (1);
        if (a.deadline == b.deadline
            && a.request_time == b.request_time
            && a.coder_id < b.coder_id)
            return (1);
        return (0);
    }
    return (0);
}

void remove_request(t_heap *heap, int coder_id, int scheduler)
{
    int i;
    int parent;

    i = 0;
    while (i < heap->size)
    {
        if (heap->arr[i].coder_id == coder_id)
        {
            heap->arr[i] = heap->arr[heap->size - 1];
            heap->size--;
            while (i > 0)
            {
                parent = parent_index(i);
                if (!request_has_priority(heap->arr[i], heap->arr[parent], scheduler))
                    break ;
                swap_request(&heap->arr[i], &heap->arr[parent]);
                i = parent;
            }
            heap_bubble_down(heap, i, scheduler);
            return ;
        }
        i++;
    }
}