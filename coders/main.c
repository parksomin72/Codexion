#include "main.h"

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

int heap_push(t_heap *heap, t_request request)
{
    if (heap->size >= heap->capacity)
        return (0);
    heap->arr[heap->size] = request;
    heap->size++;
    return (1);
}

int cooldown_not_finished(t_dongle *dongle, t_info *info)
{
    long now;

    now = get_time_ms() - info->start_time;
    if (now - dongle->last_release_time < info->dongle_cooldown)
        return (1);
    return (0);
}

void take_one_dongle(t_coder *coder, int index)
{
    t_dongle *dongle;

    dongle = &coder->info->dongles[index];
    pthread_mutex_lock(&dongle->m);
    while (!dongle->dongle_available
        || cooldown_not_finished(dongle, coder->info))
    {
        if (!dongle->dongle_available)
            pthread_cond_wait(&dongle->cond, &dongle->m);
        else
        {
            pthread_mutex_unlock(&dongle->m);
            usleep(100);
            pthread_mutex_lock(&dongle->m);
        }
    }
    dongle->dongle_available = 0;
    pthread_mutex_unlock(&dongle->m);
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

void take_dongles(t_coder *coder)
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

    take_one_dongle(coder, first);    
    take_one_dongle(coder, second);
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

int has_more_compiles(t_coder *coder)
{
    int n;

    pthread_mutex_lock(&coder->state_m);
    n = coder->compile_count;
    pthread_mutex_unlock(&coder->state_m);
    if (n < coder->info->number_of_compiles_required)
        return (1);
    return (0);
}

int simulation_running(t_coder *coder)
{
    int n;

    pthread_mutex_lock(&coder->info->stop_m);
    n = coder->info->simulation_stop;
    pthread_mutex_unlock(&coder->info->stop_m);
    if (n)
        return (0);
    return (1);
}

void set_simulation_stop(t_info *info)
{
    pthread_mutex_lock(&info->stop_m);
    info->simulation_stop = 1;
    pthread_mutex_unlock(&info->stop_m);
}


void print_msg(t_coder *coder, char *msg)
{
    long time_now;
    
    time_now = get_time_ms() - coder->info->start_time;
    
    pthread_mutex_lock(&coder->info->print_m);
    if (strcmp("burned out", msg) == 0 && !coder->info->death_printed) {
        printf("%ld %d %s\n", time_now, coder->id, msg);
        coder->info->death_printed = 1;
    }
    else if (!coder->info->death_printed)
    printf("%ld %d %s\n", time_now, coder->id, msg);
    pthread_mutex_unlock(&coder->info->print_m);
}

void check_simulation_state(t_coder *coder)
{
    long last;
    long now;
    
    pthread_mutex_lock(&coder->state_m);
    last = coder->last_compile_start;
    pthread_mutex_unlock(&coder->state_m);
    
    now = get_time_ms() - coder->info->start_time;

    if (now - last >= coder->info->time_to_burnout) {
        print_msg(coder, "burned out");
        set_simulation_stop(coder->info);
    }
}

void debug_and_refactor(t_coder *coder)
{
    print_msg(coder, "is debugging");
    usleep(coder->info->time_to_debug * 1000);
    if (!simulation_running(coder)) {
            return ;
    }
    
    print_msg(coder, "is refactoring");
    usleep(coder->info->time_to_refactor * 1000);
    if (!simulation_running(coder)) {
            return ;
    }
}

int all_coders_done(t_coder *coder, t_info *info)
{
    int i;
    int count;

    i = 0;
    while (i < info->number_of_coders){
        pthread_mutex_lock(&coder[i].state_m);
        count = coder[i].compile_count;
        pthread_mutex_unlock(&coder[i].state_m);
        if (count < info->number_of_compiles_required)
            return (0);
        i++;
    }
    return (1);
}

void *monitor_routine(void *arg)
{
    int i;
    t_monitor *monitor;
    
    monitor = (t_monitor *)arg;
    while (simulation_running(monitor->coders))
    {
        i = 0;
        while (i < monitor->info->number_of_coders)
        {
            check_simulation_state(&monitor->coders[i]);
            if (!simulation_running(&monitor->coders[i])) {
                return NULL;
            }
            i++;
        }
        if (all_coders_done(monitor->coders, monitor->coders->info)) {
            set_simulation_stop(monitor->coders->info);
            return (NULL);
        }
        usleep(1000);
    }
    return (NULL);
}

void has_one_coder(t_coder *coder)
{
    take_one_dongle(coder, 0);
    print_msg(coder, "has taken a dongle");
    while (simulation_running(coder))
        usleep(100);
    release_one_dongle(coder, 0);
}

void *routine(void *arg)
{
    t_coder *coder = (t_coder *)arg;

    if (coder->info->number_of_coders == 1)
    {
        has_one_coder(coder);
        return (NULL);
    }
    while (has_more_compiles(coder) && simulation_running(coder))
    {
        take_dongles(coder);
        print_msg(coder, "has taken a dongle");
        print_msg(coder, "has taken a dongle");

        pthread_mutex_lock(&coder->state_m);
        coder->last_compile_start = get_time_ms() - coder->info->start_time;
        pthread_mutex_unlock(&coder->state_m);
        print_msg(coder, "is compiling");
        
        usleep(coder->info->time_to_compile * 1000);
        
        pthread_mutex_lock(&coder->state_m);
        coder->compile_count++;
        pthread_mutex_unlock(&coder->state_m);
        
        release_dongles(coder);
        
        if (!simulation_running(coder) || !has_more_compiles(coder)) {
            return NULL;
        }
        debug_and_refactor(coder);
    }
    
    return NULL;
}


void create_coder_thread(t_coder *coder, t_info *info, int n)
{
    int i;
    
    i = 0;
    info->death_printed = 0;
    while (i < n)
    {
        coder[i].id = i + 1;
        coder[i].compile_count = 0;
        coder[i].last_compile_start = 0;
        coder[i].info = info;
        pthread_create(&coder[i].thread, NULL, routine, (void *)&coder[i]);
        i++;
    }
}

int init_mutex(t_coder *coder, t_info *info, int n)
{
    int i;

    i = 0;
    pthread_mutex_init(&info->stop_m, NULL);
    while (i < n)
    {
        info->dongles[i].dongle_available = 1;
        info->dongles[i].last_release_time = -info->dongle_cooldown;
        info->dongles[i].queue.capacity = info->number_of_coders;
        info->dongles[i].queue.size = 0;
        info->dongles[i].queue.arr = malloc(sizeof(t_request)
                * info->number_of_coders);
        if (!info->dongles[i].queue.arr)
            return (0);
        pthread_mutex_init(&info->dongles[i].m, NULL);
        pthread_cond_init(&info->dongles[i].cond, NULL);
        pthread_mutex_init(&coder[i].state_m, NULL);
        i++;
    }
    return (1);
}

void init_index(t_coder *coder, int n)
{
    int i;
    
    i = 0;
    while (i < n) {
        coder[i].left_index = i;
        coder[i].right_index = (i + 1) % n;
        i++;
    }
}

int init_info(t_info *info, t_coder *coder, t_monitor *monitor, int n)
{
    int         i;
    pthread_t  monitor_t;

    i = 0;
    info->simulation_stop = 0;
    info->death_printed = 0;
    monitor->info = info;
    monitor->coders = coder;
    init_index(coder, n);
    if (!init_mutex(coder, info, n))
        return (0);
    create_coder_thread(coder, info, n);
    pthread_create(&monitor_t, NULL, monitor_routine, (void *)monitor);
    while (i < n)
    {
        pthread_join(coder[i].thread, NULL);
        i++;
    }
    set_simulation_stop(info);
    pthread_join(monitor_t, NULL);
    return (1);
}

void destroy(t_info *info, t_coder *coder, int n)
{
    int i;

    i = 0;
    pthread_mutex_destroy(&info->print_m);
    while (i < n)
    {
        pthread_cond_destroy(&info->dongles[i].cond);
        free(info->dongles[i].queue.arr);
        pthread_mutex_destroy(&info->dongles[i].m);
        pthread_mutex_destroy(&coder[i].state_m);
        i++;
    }
    pthread_mutex_destroy(&info->stop_m);
}

int allocate(t_info *info, t_coder **coder, int *n)
{
    *n = info->number_of_coders;
    *coder = malloc(sizeof(t_coder) * (*n));
    info->dongles = malloc(sizeof(t_dongle) * (*n));
    
    if (!*coder || !info->dongles) {
        free(*coder);
        free(info->dongles);
        return (0);
    }
    return (1);
}

int main(int ac, char **av) {
    int n;
    t_info info;
    t_coder *coder;
    t_monitor monitor;
    
    if (ac != 9 || !check(av, &info) ||  !convert(av, &info))
    {
        write(2, "Error\n", 6);
        return (1);
    }
    if (!allocate(&info, &coder, &n))
        return (1);
    pthread_mutex_init(&info.print_m, NULL);
    info.start_time = get_time_ms();
    if (!init_info(&info, coder, &monitor, n))
    {
        free(info.dongles);
        free(coder);
        return (1);
    }
    destroy(&info, coder, n);
    free(info.dongles);
    free(coder);
    return (0);
}