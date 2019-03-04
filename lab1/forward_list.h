#pragma once

#include <stdbool.h>
#include <stddef.h>

/**
 * Struct representing the list.
 * 
 * Users should keep the list in a `struct forward_list*` pointer (initially must be NULL!),
 * and pass address of that pointer to functions operating on the list.
 */
struct forward_list;

// prints the whole list
void print_forward_list(struct forward_list *head);

// gets value of an element; NULL on out of range, returned pointer value is tied to the element inside the list
int* get_value(struct forward_list *head, size_t index);

// checks whether the list is empty
bool is_empty(struct forward_list *head);

// inserts element at the head of the list
void insert_first(struct forward_list **head, int value);

// searches for an element; if it is found, moves it to the beginning
struct forward_list** findMTF(struct forward_list **head, int value);

// searches for an element; if it is found, moves it one place closer to the head
struct forward_list** findTRANS(struct forward_list **head, int value);

// deletes the head of the list
void delete_first(struct forward_list **head);

// finds a value in the list and deletes it
void delete_value(struct forward_list **head, int value);


extern int find_comparisons_count;
extern int delete_comparisons_count;
