/* 
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 * Modified to store strings, 2018
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *q =  (queue_t*)malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    //if memory allocated failed, then return a null queue
    if (q == NULL) {
        return NULL;
    }
    //if memory allocated successfully, return an empty queue
    q->head = NULL;
    q->tail = q->head;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    //1. corner case1 : this queue is a NULL queue
    if (q == NULL) {
        return;
    }
    //build a tmp ele for query each ele in queue
    struct ELE *tmp_ele = q->head;
    /* How about freeing the list elements and the strings? */
    while(tmp_ele != NULL) {
        struct ELE *cur = tmp_ele;
        tmp_ele = tmp_ele->next;
        free(cur->value);
        cur->value = NULL;
        free(cur);
    }
    /* Free queue structure */
    free(q);
    q = NULL;
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    /* What should you do if the q is NULL? */
    //if q is NULL
    if (q == NULL) {
        return false;
    }
    newh = (list_ele_t*)malloc(sizeof(list_ele_t));
    //new node could not allocate space
    if (newh == NULL) {
        return false;
    }
    /* Don't forget to allocate space for the string and copy it */
    //allocate a space for the string and copy it
    char *newS;
    newS = (char*)malloc(strlen(s)+1);
    if (newS == NULL) {
        return false;
    }
    strncpy(newS, s, strlen(s));
    if (strlen(newS) != strlen(s)) {
        return false;
    }
    newh->value = newS;
    /* What if either call to malloc returns NULL? */
    newh->next = q->head;
    q->head = newh;
    q->size = q->size + 1;
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */
    list_ele_t *newh;
    //if q is NULL
    if (q == NULL) {
        return false;
    }
    newh = (list_ele_t*)malloc(sizeof(list_ele_t));
    //new node could not be allocated
    if (newh == NULL) {
        return false;
    }
    //allocate a space for the string and copy it
    char *newS;
    newS = (char*)malloc(strlen(s)+1);
    if(newS == NULL) {
        return false;
    }
    strncpy(newS, s, strlen(s));
    if (strlen(newS) != strlen(s)) {
        return false;
    }
    newh->value = newS;
    newh->next = NULL;
    if(q->tail == NULL){
        q->tail = newh;
    } else {
        q->tail->next = newh;
        q->tail = newh;
    }
    q->size = q->size + 1;
    return true;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If sp is non-NULL and an element is removed, copy the removed string to *sp
  (up to a maximum of bufsize-1 characters, plus a null terminator.)
  The space used by the list element and the string should be freed.
*/
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* You need to fix up this code. */
    //if queue is NULL or empty
    if (q == NULL || q->size == 0) {
        return false;
    }
    list_ele_t *curhead;
    curhead = q->head;
    q->head = q->head->next;
    q->size = q->size - 1;
    strncpy(sp, curhead->value, bufsize-1);
    //释放空间后置为NULL
    free(curhead->value);
    free(curhead);
    curhead->value = NULL;
    curhead = NULL;
    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    //假如queue是个空的或者NULL
    if (q == NULL || q->size == 0) {
        return 0;
    }
    return q->size;
}

/*
  Reverse elements in queue
  No effect if q is NULL or empty
  This function should not allocate or free any list elements
  (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
  It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* You need to write the code for this function */
    //if this queue is empty of NULL
    if (q == NULL || q->size == 0){
        return;
    }
    list_ele_t *prev;
    list_ele_t *curr = q->head;
    while (curr != NULL) {
        list_ele_t *nextTemp = curr->next;
        curr->next = prev;
        prev = curr;
        curr = nextTemp;
    }
    q->tail = q->head;
    q->head = prev;
}

