#include <endian.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define ERROR_HANDLE(call_func, ...)                                                                \
    do {                                                                                            \
        int error_handler = call_func;                                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Errno: %d\n",                                     \
                            errno);                                                                 \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

typedef struct list{
    int data;
    struct list* next;
} list_t;


list_t* list_new           (int elem);
list_t* list_insert        (list_t* head, int elem);
list_t* list_find          (list_t* head, int elem);
list_t* list_erase         (list_t* head, int elem); 
list_t* list_insert_after  (list_t* head, list_t* where, list_t* what); 
list_t* list_insert_before (list_t* head, list_t* where, list_t* what); 
list_t* list_delete        (list_t* head); 
list_t* list_next          (list_t* curr);
void    list_print         (const list_t* head); 

// int main() {
//     list_t* list = list_new(228666);
//     for (int i = 0; i < 10; ++i) {
//         list_insert(list, i);
//     }
//     list_print(list);



//     return EXIT_SUCCESS;
// }

list_t* list_find_prev_(list_t* head, int elem);

list_t* list_new (int elem) {
    list_t* list = calloc(1, sizeof(list_t));

    if (!list) {
        return NULL;
    }

    list->data = elem;
    list->next = NULL;

    return list;
}

list_t* list_insert(list_t* head, int elem) {

    list_t* last_node = head;
    for(; last_node->next != NULL; last_node = last_node->next);
    last_node->next = list_new(elem);

    return head;
}

list_t* list_find(list_t* head, int elem) {
    if (!head) {
        return NULL;
    }

    if (head->data == elem) {
        return head;
    }

    return list_find(head->next, elem);
}

list_t* list_find_prev_(list_t* head, int elem) {
    if (!head || !head->next) {
        return NULL;
    }

    if (head->next->data == elem) {
        return head;
    }

    return list_find_prev_(head->next, elem);
}

list_t* list_erase(list_t* head, int elem) {

    if (!head) {
        return head;
    }

    if (head->data == elem) {
        list_t* new_head = head->next;
        head->next = NULL;
        free(head);
        return new_head;
    }

    if (!head->next) {
        return head;
    }

    list_t* prev_found_node = list_find_prev_(head, elem);
    if (!prev_found_node) {
        return head;
    }
    list_t* found_node      = prev_found_node->next;
    list_t* next_found_node = found_node->next;

    found_node->next = NULL;
    prev_found_node->next = next_found_node;
    free(found_node);

    return head;
} 

list_t* list_insert_after(list_t* head, list_t* where, list_t* what) {
    if (!where || !what) {
        return head;
    }

    if (!head) {
        return what;
    }

    list_t* where2 = where->next;
    where->next = what;

    list_t* what_end = what;
    for(; what_end->next != NULL; what_end = what_end->next);
    
    what_end->next = where2;

    return head;
}

list_t* list_insert_before(list_t* head, list_t* where, list_t* what) {
    if (!where || !what) {
        return head;
    }

    if (!head) {
        return what;
    }

    list_t* what_end = what;
    for(; what_end->next != NULL; what_end = what_end->next);

    if (head == where) {
        what_end->next = head;
        return what;
    }

    list_t* prev_where = head;
    for(; prev_where->next != where; prev_where = prev_where->next);


    prev_where->next = what;
    what_end->next   = where;

    return head;
}

list_t* list_delete(list_t* head) {
    if (!head) {
        return NULL;
    }

    list_t* next_head = head->next;
    free(head);

    return list_delete(next_head);
}

list_t* list_next (list_t* curr) {
    if (!curr) {
        return NULL;
    }

    return curr->next;
}

void list_print(const list_t* head) {
    if (head == NULL) {
        printf("[]\n");
        return;
    }

    printf("[");

    const list_t* cur = head;
    for (; cur->next != NULL; cur = cur->next) {
        printf("%d, ", cur->data);
    }

    printf("%d]\n", cur->data);
}