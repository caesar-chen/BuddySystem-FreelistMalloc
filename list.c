//Shizhe Chen
#include <stdlib.h>
#include "./list.h"
#include "my_malloc.h"

static NODE* create_node(int data);

static NODE* create_node(int data)
{
    NODE* nd = (NODE*) my_malloc(sizeof(NODE));
    if (nd == NULL) {
      return NULL;
    }
    nd->prev = NULL;
    nd->next = NULL;
    nd->data = data;
    return nd;
}
/**
 * pushFront
 * Takes in ptr to a list and data to add
 * Should add new node containing the data to the head of the list, and increment size
 */
void pushFront(LIST *list, int data)
{
	NODE* newnode = create_node(data);
    if (newnode == NULL) {
      return;
    }
    newnode->next = list->head;
    if (newnode->next != NULL) {
      newnode->next->prev = newnode;
    }
    list->head = newnode;
    list->size = list->size + 1;
    if (list->size == 1)
    {
      list->tail = newnode;
    }
}

/**
 * pushFront
 * Takes in ptr to a list and data to add
 * Should add new node containing the data to the tail of the list, and increment size
 */
void pushBack(LIST *list, int data)
{
	NODE* newnode = create_node(data);
    if (newnode == NULL) {
      return;
    }
    newnode->prev = list->tail;
    if (newnode->prev != NULL) {
      newnode->prev->next = newnode;
    }
    list->tail = newnode;
    list->size = list->size + 1;
    if (list->size == 1)
    {
      list->head = newnode;
    }
}

/**
 * popFront
 * Takes in ptr to a list
 * Remove and free node at the front of the list, and decrement size
 * Return the value of that node
 * Return 0 if the size of the list is 0
 */
int popFront(LIST *list)
{
	if (list->size == 0)
    {
      return -1;
    }
    if (list->size == 1)
    {
      NODE* reme = list->head;
      list->head = NULL;
      list->tail = NULL;
      list->size = 0;
      my_free(reme);
      return 0;
    }
    NODE* reme = list->head;
    list->head = list->head->next;
    list->head->prev = NULL;
    list->size = list->size - 1;
    my_free(reme);
    return 0;
}

/**
 * popBack
 * Takes in ptr to a list
 * Remove and free node at the back of the list, and decrement size
 * Return the value of that node
 * Return 0 if the size of the list is 0
 */
int popBack(LIST *list)
{
	if (list->size == 0)
    {
      return -1;
    }
    if (list->size == 1)
    {
      NODE* reme = list->tail;
      list->head = NULL;
      list->tail = NULL;
      list->size = 0;
      my_free(reme);
      return 0;
    }
    NODE* reme = list->tail;
    list->tail = list->tail->prev;
    list->tail->next = NULL;
    list->size = list->size - 1;
    my_free(reme);
    return 0;
}


