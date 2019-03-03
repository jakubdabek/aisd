#pragma once

#include <stdbool.h>
#include <stdio.h>

struct forward_list
{
    int value;
    struct forward_list *next;
};

void print_forward_list(struct forward_list *head);

bool is_empty(struct forward_list **head);
void insert_first(struct forward_list **head, int value);
struct forward_list** findMTF(struct forward_list **head, int value);
struct forward_list** findTRANS(struct forward_list **head, int value);
void delete_first(struct forward_list **head);
void delete_value(struct forward_list **head, int value);

extern int find_comparisons_count;
extern int delete_comparisons_count;
