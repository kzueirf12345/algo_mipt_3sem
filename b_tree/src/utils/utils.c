#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"

#ifndef B_TREE_INOUT_ELEM_CODE
#define B_TREE_INOUT_ELEM_CODE "%d"
#endif /*B_TREE_INOUT_ELEM_CODE*/
#ifndef B_TREE_INOUT_ELEM_T
#define B_TREE_INOUT_ELEM_T int
#endif /*B_TREE_INOUT_ELEM_T*/

#define B_TREE_INOUT_ELEM_SIZE sizeof(B_TREE_INOUT_ELEM_T)

int data_to_str(const void* const data, const size_t size, char* str,
                   const size_t str_size)
{
    if (is_invalid_ptr(data)) return -1;
    if (is_invalid_ptr(str))  return -1;
    if (!size)                return -1;

    char temp_str[B_TREE_INOUT_ELEM_SIZE  * 4] = {};
    for (size_t offset = 0; offset < size; 
         offset += (size - offset >= B_TREE_INOUT_ELEM_SIZE  ? B_TREE_INOUT_ELEM_SIZE  : sizeof(uint8_t)))
    {
        if (size - offset >= B_TREE_INOUT_ELEM_SIZE )
        {
            if (snprintf(temp_str, B_TREE_INOUT_ELEM_SIZE * 4, B_TREE_INOUT_ELEM_CODE,
                         *(const B_TREE_INOUT_ELEM_T*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return -1;
            }
        }
        else
        {
            if (snprintf(temp_str, sizeof(uint8_t) * 4, B_TREE_INOUT_ELEM_CODE, 
                         *(const uint8_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return -1;
            }
        }

        if (!strncat(str, temp_str, str_size))
        {
            perror("Can't stract str and temp_str");
            return -1;
        }
    }

    return 0;
}
#undef B_TREE_INOUT_ELEM_SIZE

enum PtrState is_invalid_ptr(const void* ptr)
{
    errno = 0;
    if (ptr == NULL)
    {
        return PTR_STATES_NULL;
    }

    char filename[] = "/tmp/chupapi_munyanya.XXXXXX";
    const int fd = mkstemp(filename);

    if (fd == -1) 
    {
        perror("Can't mkstemp file");
        return PTR_STATES_ERROR;
    }
    
    const ssize_t write_result = write(fd, ptr, 1);

    if (remove(filename))
    {
        perror("Can't remove temp file");
        return PTR_STATES_ERROR;
    }

    if (close(fd))
    {
        perror("Can't close temp file");
        return PTR_STATES_ERROR;
    }

    if (write_result == 1)
        return PTR_STATES_VALID;

    if (errno == EFAULT) 
    {
        errno = 0;
        return PTR_STATES_INVALID;
    }
    
    perror("Unpredictable errno state, after write into temp file");
    return PTR_STATES_ERROR;
}

int is_empty_file (FILE* file)
{
    if (is_invalid_ptr(file))
    {
        fprintf(stderr, "Is empty file nvalid\n");
        return -1;
    }

    int seek_temp = SEEK_CUR;

    if (fseek(file, 0, SEEK_END))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    const int res = ftell(file) > 2;

    if (fseek(file, 0, seek_temp))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    return res;
}