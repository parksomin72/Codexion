#include "main.h"

void heap_bubble_down(t_heap *heap, int i, int scheduler)
{
    int left;
    int right;
    int best;

    while (1)
    {
        left = left_child(i);
        right = right_child(i);
        best = i;
        if (left < heap->size
            && request_has_priority(heap->arr[left], heap->arr[best], scheduler))
            best = left;
        if (right < heap->size
            && request_has_priority(heap->arr[right], heap->arr[best], scheduler))
            best = right;
        if (best == i)
            break ;
        swap_request(&heap->arr[i], &heap->arr[best]);
        i = best;
    }
}

int heap_push(t_heap *heap, t_request request, int scheduler)
{
    int i;
    int parent;

    if (heap->size >= heap->capacity)
        return (0);
    i = heap->size;
    heap->arr[i] = request;
    heap->size++;
    while (i > 0)
    {
        parent = parent_index(i);
        if (!request_has_priority(heap->arr[i], heap->arr[parent], scheduler))
            break ;
        swap_request(&heap->arr[i], &heap->arr[parent]);
        i = parent;
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

void heap_pop(t_heap *heap, int scheduler)
{
    if (heap->size == 0)
        return ;
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heap_bubble_down(heap, 0, scheduler);
}