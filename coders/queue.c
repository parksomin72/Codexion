#include "main.h"

int heap_push(t_heap *heap, t_request request, int scheduler)
{
    int i;

    if (heap->size >= heap->capacity)
        return (0);
    heap->arr[heap->size] = request;
    i = heap->size;
    heap->size++;
    while (i > 0
        && request_has_priority(heap->arr[i], heap->arr[i - 1], scheduler))
    {
        swap_request(&heap->arr[i], &heap->arr[i - 1]);
        i--;
    }
    return (1);
}

int is_my_turn(t_heap *heap, int coder_id)
{
    if (heap->size == 0)
        return (0);
    if (heap->arr[0].coder_id == coder_id)
        return (1);
    return (0);
}

void pop_front(t_heap *heap)
{
    int i;

    i = 0;
    if (heap->size == 0)
        return ;
    while (i < heap->size - 1)
    {
        heap->arr[i] = heap->arr[i + 1];
        i++;
    }
    heap->size--;
}