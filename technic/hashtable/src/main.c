#include <endian.h>
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

#define DATABASE_FILENAME "database.txt"
#define DATABASE_FILENAME_SIZE 14
#define MAX_DATA_SIZE 4097

typedef struct Node {
    size_t offset;

    struct Node* next;
    char* key;
    size_t key_size;
    size_t val_size;
} node_t;

#define BUCKETS_NUM 10000

typedef struct HashTable {
    node_t* buckets[BUCKETS_NUM];

    FILE* database;
} hash_table_t;

void hash_table_ctor(hash_table_t* table, const char* database_filename);
void hash_table_dtor(hash_table_t* table);

void hash_table_insert(hash_table_t* table, char* key, char* val);
void hash_table_delete(hash_table_t* table, char* key);
void hash_table_update(hash_table_t* table, char* key, char* val);
void hash_table_print (hash_table_t* table, char* key);

#define MAX_COMMAND_SIZE_ 20
int main() 
{
    hash_table_t hash_table = {};
    hash_table_ctor(&hash_table, DATABASE_FILENAME);

    size_t commands_cnt = 0;
    if (scanf("%lu", &commands_cnt) != 1) 
    {
        fprintf(stderr, "Can't scanf commands_cnt\n");
        return EXIT_FAILURE;
    }

    char command[MAX_COMMAND_SIZE_] = {};

    for (size_t command_num = 0; command_num < commands_cnt; ++command_num) 
    {
        if (scanf("%s", command) != 1) 
        {
            fprintf(stderr, "Can't scanf command\n");
            return EXIT_FAILURE;
        }

        if (strncmp(command, "ADD", MAX_COMMAND_SIZE_) == 0) 
        {
            char key[MAX_DATA_SIZE] = {};
            char val[MAX_DATA_SIZE] = {};

            if (scanf("%s %s", key, val) != 2) 
            {
                fprintf(stderr, "Can't scanf ADD input\n");
                return EXIT_FAILURE;
            }

            hash_table_insert(&hash_table, key, val);
        } 
        else if (strncmp(command, "DELETE", MAX_COMMAND_SIZE_) == 0) 
        {
            char key[MAX_DATA_SIZE] = {};

            if (scanf("%s", key) != 1) {
                fprintf(stderr, "Can't scanf DELETE input\n");
                return EXIT_FAILURE;
            }

            hash_table_delete(&hash_table, key);
        }
        else if (strncmp(command, "UPDATE", MAX_COMMAND_SIZE_) == 0) 
        {
            char key[MAX_DATA_SIZE] = {};
            char val[MAX_DATA_SIZE] = {};

            if (scanf("%s %s", key, val) != 2) 
            {
                fprintf(stderr, "Can't scanf UPDATE input\n");
                return EXIT_FAILURE;
            }

            hash_table_update(&hash_table, key, val);
        }
        else if (strncmp(command, "PRINT", MAX_COMMAND_SIZE_) == 0) 
        {
            char key[MAX_DATA_SIZE] = {};

            if (scanf("%s", key) != 1) {
                fprintf(stderr, "Can't scanf PRINT input\n");
                return EXIT_FAILURE;
            }

            hash_table_print(&hash_table, key);
        }
        else 
        {
            fprintf(stderr, "Unknown command\n");
            return EXIT_FAILURE;
        }

    }

    hash_table_dtor(&hash_table);

    return EXIT_SUCCESS;
}
#undef MAX_COMMAND_SIZE_

void hash_table_ctor(hash_table_t* table, const char* database_filename) 
{
    assert(table);
    assert(database_filename);

    table->database = fopen(database_filename, "w+b");
}

void hash_table_dtor(hash_table_t* table) 
{
    assert(table);

    fclose(table->database);

    for (size_t i = 0; i < BUCKETS_NUM; ++i) {
        node_t* bucket = table->buckets[i];

        while (bucket) {
            node_t* temp = bucket;
            bucket = bucket->next;

            free(temp->key);
            free(temp);
        }
    }
}

static size_t hash_table_bucket_ind(const char* str) {
    size_t hash = 0;
    for (size_t i = 0 ; str[i] != '\0' ; ++i) {
        hash = 31 * hash + (size_t)str[i];
    }
    return hash % BUCKETS_NUM;
}

void hash_table_insert(hash_table_t* table, char* key, char* val) 
{
    assert(table);

    const size_t bucket_ind = hash_table_bucket_ind(key);
    const size_t key_size = strlen(key);
    const size_t val_size = strlen(val);

    for (node_t* node = table->buckets[bucket_ind]; node; node = node->next) {
        if (node->key_size == key_size && strncmp(node->key, key, key_size) == 0) {
            printf("ERROR\n");
            return;
        }
    }

    node_t* new_node = calloc(1, sizeof(node_t));
    new_node->key = strdup(key);
    new_node->key_size = key_size;
    new_node->val_size = val_size;
    new_node->next = table->buckets[bucket_ind];
    table->buckets[bucket_ind] = new_node;

    fseek(table->database, 0, SEEK_END);
    new_node->offset = (size_t)ftell(table->database);

    fwrite(&new_node->key_size, sizeof(new_node->key_size), 1, table->database);
    fwrite(&new_node->val_size, sizeof(new_node->val_size), 1, table->database);

    fwrite(key, sizeof(*key), key_size, table->database);
    fwrite(val, sizeof(*val), val_size, table->database);

    fflush(table->database);
}

void hash_table_delete(hash_table_t* table, char* key) 
{
    assert(table);

    const size_t bucket_ind = hash_table_bucket_ind(key);
    const size_t key_size = strlen(key);

    node_t* curNode = table->buckets[bucket_ind];
    node_t* prevNode = NULL;

    while (curNode) 
    {
        if (curNode->key_size == key_size && strncmp(curNode->key, key, key_size) == 0) 
        {
            if (prevNode) 
            {
                prevNode->next = curNode->next;
            } 
            else 
            {
                table->buckets[bucket_ind] = curNode->next;
            }

            free(curNode->key);
            free(curNode);
            return;
        }

        prevNode = curNode;
        curNode = curNode->next;
    }

    printf("ERROR\n");
    return;
}

void hash_table_update(hash_table_t* table, char* key, char* val) 
{
    assert(table);

    const size_t bucket_ind = hash_table_bucket_ind(key);
    const size_t key_size = strlen(key);
    const size_t new_val_size = strlen(val);

    node_t* node = table->buckets[bucket_ind];
    for (; node; node = node->next) {
        if (node->key_size == key_size && strncmp(node->key, key, key_size) == 0) {
            break;
        }
    }

    if (!node) {
        printf("ERROR\n");
        return;
    }

    if (new_val_size > node->val_size) 
    {
        fseeko(table->database, 0, SEEK_END);
        size_t new_offset = (size_t)ftell(table->database);

        fwrite(&key_size,     sizeof(key_size),     1, table->database);
        fwrite(&new_val_size, sizeof(new_val_size), 1, table->database);

        fwrite(key, sizeof(*key), key_size,     table->database);
        fwrite(val, sizeof(*val), new_val_size, table->database);

        fflush(table->database);

        node->offset = new_offset;
    } 
    else 
    {
        fseeko(table->database, (__off_t)(node->offset + sizeof(size_t) * 2 + key_size), SEEK_SET);
        fwrite(val, sizeof(*val), new_val_size, table->database);

        const char nil = 0;
        for (size_t i = new_val_size; i < node->val_size; ++i) 
        {
            fwrite(&nil, sizeof(char), 1, table->database);
        }

        fflush(table->database);

    }

    node->val_size = new_val_size;
}

void hash_table_print (hash_table_t* table, char* key) 
{
    assert(table);

    const size_t bucket_ind = hash_table_bucket_ind(key);
    const size_t key_size = strlen(key);

    node_t* node = table->buckets[bucket_ind];
    for (; node; node = node->next) {
        if (node->key_size == key_size && strncmp(node->key, key, key_size) == 0) {
            break;
        }
    }

    if (!node) {
        printf("ERROR\n");
        return;
    }

    fseeko(table->database, (__off_t)(node->offset + sizeof(size_t) * 2 + key_size), SEEK_SET);

    char val[MAX_DATA_SIZE] = {};
    if (fread(val, sizeof(*val), node->val_size, table->database) != node->val_size) 
    {
        printf("ERROR\n");
        return;
    }
    val[node->val_size] = '\0';

    printf("%s %s\n", key, val);
}