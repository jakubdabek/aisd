#include "forward_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int LIST_SIZE = 100;

struct fill_function
{
    const char *description;
    void (*fill)(struct forward_list **head, int list[], size_t size);
};

void fill_random(struct forward_list **head, int list[], size_t size);
void fill_ascending(struct forward_list **head, int list[], size_t size);
void fill_descending(struct forward_list **head, int list[], size_t size);

const struct fill_function fill_functions[] =
{
    { "random", fill_random },
    { "ascending", fill_ascending },
    { "descending", fill_descending },
};

struct find_function
{
    const char *description;
    struct forward_list** (*find)(struct forward_list**, int);
};

const struct find_function find_functions[] =
{
    { "findMTF", findMTF },
    { "findTRANS", findTRANS },
};

struct find_max_function
{
    const char *description;
    int(*find_max)(struct forward_list**, const struct find_function*);
};

int find_max_ascending(struct forward_list **head, const struct find_function *find_func);
int find_max_descending(struct forward_list **head, const struct find_function *find_func);
int find_max_random(struct forward_list **head, const struct find_function *find_func);

const struct find_max_function find_max_functions[] =
{
    { "ascending", find_max_ascending },
    { "descending", find_max_descending },
    { "random", find_max_random },
};

void test(
    const struct fill_function *fill_func,
    const struct find_max_function *find_max_func,
    const struct find_function *find_func
);

void update_mean_variance(const int count, double * const mean, double * const M2, const int newValue)
{
    const double delta = newValue - *mean;
    *mean += delta / (count + 1);
    const double delta2 = newValue - *mean;
    *M2 += delta * delta2;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
        LIST_SIZE = atoi(argv[1]);
    printf("List size: %d\n", LIST_SIZE);
    srand(time(NULL));
    
    for (int i = 0; i < sizeof(find_max_functions) / sizeof(find_max_functions[0]); i++)
    {
        for (int j = 0; j < sizeof(fill_functions) / sizeof(fill_functions[0]); j++)
        {
            for (int k = 0; k < sizeof(find_functions) / sizeof(find_functions[0]); k++)
            {
                const struct fill_function *fill_func = &fill_functions[j];
                const struct find_max_function *find_max_func = &find_max_functions[i];
                const struct find_function *find_func = &find_functions[k];
                printf("\nTesting %s with fill %s and find_max %s\n",
                        find_func->description,
                        fill_func->description,
                        find_max_func->description);
                
                double mean_find_comparisons = 0.0;
                double M2_find_comparisons = 0.0;
                double mean_delete_comparisons = 0.0;
                double M2_delete_comparisons = 0.0;
                const int num_iters = 1000;
                for (int iters = 0; iters < num_iters; iters++)
                {
                    find_comparisons_count = 0;
                    delete_comparisons_count = 0;
                    test(fill_func, find_max_func, find_func);

                    update_mean_variance(iters, &mean_find_comparisons, &M2_find_comparisons, find_comparisons_count);
                    update_mean_variance(iters, &mean_delete_comparisons, &M2_delete_comparisons, delete_comparisons_count);
                }
                printf("find comparisons: %8.2lf, stddev: %10.4lf, delete comparisons: %8.2lf, stddev: %10.4lf\n",
                        mean_find_comparisons, sqrt(M2_find_comparisons / num_iters),
                        mean_delete_comparisons, sqrt(M2_delete_comparisons / num_iters));
            }
        }
    }
}

void shuffle(int arr[], const size_t n)
{
    if (n > 1) 
    {
        for (size_t i = 0; i < n - 1; i++) 
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = arr[j];
            arr[j] = arr[i];
            arr[i] = t;
        }
    }
}

void fill_random(struct forward_list **head, int list[], const size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        list[i] = i;
    }
    shuffle(list, size);

    for (size_t i = 0; i < size; i++)
    {
        insert_first(head, list[i]);    
    }
}

void fill_ascending(struct forward_list **head, int list[], const size_t size)
{
    for (int i = size - 1; i >= 0; i--)
    {
        insert_first(head, i);    
    }
}

void fill_descending(struct forward_list **head, int list[], const size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        insert_first(head, i);    
    }
}


int find_max_ascending(struct forward_list **head, const struct find_function * const find_func)
{
    int max = *get_value(*head, 0);
    for (size_t i = 0; i < LIST_SIZE; i++)
    {
        if (find_func->find(head, i) != NULL && i > max)
        {
            max = i;
        }
    }
    return max;
}

int find_max_descending(struct forward_list **head, const struct find_function * const find_func)
{
    int max = *get_value(*head, 0);
    for (int i = LIST_SIZE - 1; i >= 0; i--)
    {
        if (find_func->find(head, i) != NULL && i > max)
        {
            max = i;
        }
    }
    return max;
}

int find_max_random(struct forward_list **head, const struct find_function * const find_func)
{
    int list[LIST_SIZE];
    for (size_t i = 0; i < LIST_SIZE; i++)
    {
        list[i] = i;
    }
    shuffle(list, LIST_SIZE);

    int max = *get_value(*head, 0);
    for (size_t i = 0; i < LIST_SIZE; i++)
    {
        if (find_func->find(head, list[i]) != NULL && list[i] > max)
        {
            max = list[i];
        }
    }
    return max;
}



void test(
    const struct fill_function * const fill_func,
    const struct find_max_function * const find_max_func,
    const struct find_function * const find_func
)
{
    struct forward_list *head = NULL;
    int values[LIST_SIZE];
    fill_func->fill(&head, values, LIST_SIZE);
    while (!is_empty(head))
    {
        //printf("Before find_max: "); print_forward_list(head); printf("\n");
        int max = find_max_func->find_max(&head, find_func);
        //printf("Before delete: "); print_forward_list(head); printf("\n");
        delete_value(&head, max);
    }
}

