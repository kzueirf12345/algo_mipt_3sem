#include <endian.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define ERROR_HANDLE(call_func, ...)                                                                \
    do {                                                                                            \
        int error_handler = call_func;                                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

typedef struct set {
    size_t capacity;
    size_t size;
    uint8_t *bits;
} set_t;

struct set *set_new(size_t capacity);
int set_insert(struct set *s, size_t elem);
int set_erase(struct set *s, size_t elem); 
int set_find(struct set const *s, size_t elem);
struct set *set_delete(struct set *s); 
int set_empty(struct set const *s); 
ssize_t set_findfirst(struct set const *s, size_t start); 
size_t set_size(struct set const *s); 
void set_print(struct set const *s);



// int main() {
//     list_t* list = list_new(228666);
//     for (int i = 0; i < 10; ++i) {
//         list_insert(list, i);
//     }
//     list_print(list);



//     return EXIT_SUCCESS;
// }