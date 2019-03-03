#include "forward_list.h"

#include <stdlib.h>


int find_comparisons_count = 0;
int delete_comparisons_count = 0;

void print_forward_list(struct forward_list *head)
{
    printf("[");
    char sep[] = "\0 ";
    while (head != NULL)
    {
        printf("%s%d", sep, head->value);
        head = head->next;
        sep[0] = ',';
    }
    printf("]");
}

bool is_empty(struct forward_list **head)
{
    if (head == NULL || *head == NULL)
        return true;
    return false;
}

void insert_first(struct forward_list ** const head, const int value)
{
    if (head == NULL)
        return;
    struct forward_list *new_element = malloc(sizeof(*new_element));
    new_element->value = value;
    new_element->next = *head;
    *head = new_element;
}


struct forward_list** findMTF(struct forward_list **head, const int value)
{
    if (head == NULL)
        return NULL;

    struct forward_list **current = head;
    static int i = 0;
    i++;
    while (*current != NULL)
    {
        //printf("in findMTF %d: ", i); print_forward_list(*current); printf("\n");
        //print_forward_list(*head); printf("\n");
        find_comparisons_count++;
        if ((*current)->value == value)
        {
            struct forward_list *found = *current;
            *current= found->next;
            found->next = *head;
            *head = found;
            return head;
        }
        current = &((*current)->next);
    }

    return NULL;
}

struct forward_list** findTRANS(struct forward_list ** const head, const int value)
{
    if (head == NULL)
        return NULL;

    if ((*head)->value == value)
    {
        return head;
    }
    struct forward_list **previous = head;
    struct forward_list **current = &((*head)->next);
    while (*current != NULL)
    {
        find_comparisons_count++;
        if ((*current)->value == value)
        {
            struct forward_list *found = *current;
            *current = found->next;
            found->next = *previous;
            *previous = found;
            return previous;
        }
        previous = current;
        current = &((*current)->next);
    }

    return NULL;
}

void delete_first(struct forward_list ** const head)
{
    if (head == NULL)
        return;
    struct forward_list *first = *head;
    *head = first->next;
    free(first);
}

void delete_value(struct forward_list **head, const int value)
{
    if (head == NULL)
        return;
    while (*head != NULL)
    {
        delete_comparisons_count++;
        if ((*head)->value == value)
        {
            delete_first(head);
            return;
        }
        head = &((*head)->next);
    }
}
