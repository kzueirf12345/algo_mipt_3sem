#include <stdlib.h>
#include <stdio.h>

#include "b_tree/verification/verification.h"
#include "flags/flags.h"
#include "logger/src/logger.h"
#include "utils/utils.h"
#include "b_tree/funcs/funcs.h"
#include "b_tree/dumb/dumb.h"

static unsigned init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
static unsigned dtor_all(flags_objs_t* const flags_objs);

int main(const int argc, char* const argv[]) 
{
    flags_objs_t flags_objs = {};
    MAIN_INT_ERROR_HANDLE(init_all(&flags_objs, argc, argv));
    
    b_tree_t tree = {};
    B_TREE_INT_ERROR_HANDLE(
        b_tree_ctor(&tree, 4), 
        dtor_all(&flags_objs);
    );

    const int testKeys[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39};
    const size_t keyCount = sizeof(testKeys) / sizeof(testKeys[0]);

    fprintf(stderr, "\n=== INSERT %d ===\n", 228666);

    B_TREE_INT_ERROR_HANDLE(
        b_tree_insert(&tree, 228666),
        dtor_all(&flags_objs);
    );

    for (size_t i = 0; i < keyCount; ++i)
    {
        fprintf(stderr, "\n=== INSERT %d ===\n", testKeys[i]);

        B_TREE_INT_ERROR_HANDLE(
            b_tree_insert(&tree, testKeys[i]),
            dtor_all(&flags_objs);
        );

        b_tree_dumb(&tree, NULL);

        fprintf(stderr, "\n=== DELETE %d ===\n", testKeys[i]);

        B_TREE_INT_ERROR_HANDLE(
            b_tree_delete(&tree, testKeys[i]),
            dtor_all(&flags_objs);
        );

        b_tree_dumb(&tree, NULL);

        fprintf(stderr, "\n=== INSERT %d ===\n", testKeys[i]);

        B_TREE_INT_ERROR_HANDLE(
            b_tree_insert(&tree, testKeys[i]),
            dtor_all(&flags_objs);
        );

        b_tree_dumb(&tree, NULL);

    }

    // for (size_t i = 0; i < keyCount; ++i)
    // {
    //     size_t ind = keyCount - i - 1;
    //     fprintf(stderr, "\n=== DELETE %d ===\n", testKeys[ind]);

    //     B_TREE_INT_ERROR_HANDLE(
    //         b_tree_delete(&tree, testKeys[ind]),
    //         dtor_all(&flags_objs);
    //     );

    //     b_tree_dumb(&tree, NULL);
    // }

    // fprintf(stderr, "\nFinal tree structure dumped to dumb file.\n");

    /* ====================================== */

    B_TREE_INT_ERROR_HANDLE(
        b_tree_dtor(&tree),
        dtor_all(&flags_objs);
    );
    
    MAIN_INT_ERROR_HANDLE(dtor_all(&flags_objs));

    return EXIT_SUCCESS;
}

unsigned logger_init(char* const log_folder);

unsigned init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv)
{
    lassert(argc, "");
    lassert(!is_invalid_ptr(argv), "");

    FLAGS_ERROR_HANDLE(flags_objs_ctor (flags_objs));
    FLAGS_ERROR_HANDLE(flags_processing(flags_objs, argc, argv));

    if (logger_init(flags_objs->log_folder))
    {
        fprintf(stderr, "Can't logger init\n");
        flags_objs_dtor(flags_objs);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

unsigned dtor_all(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    LOGG_ERROR_HANDLE(                                                              logger_dtor(););
    FLAGS_ERROR_HANDLE(                                               flags_objs_dtor(flags_objs););
    B_TREE_DUMB_ERROR_HANDLE(                                                  b_tree_dumb_dtor(););

    return EXIT_SUCCESS;
}

#define LOGOUT_FILENAME "logout.log"
#define   DUMB_FILENAME "dumb"
unsigned logger_init(char* const log_folder)
{
    lassert(!is_invalid_ptr(log_folder), "");

    char logout_filename[FILENAME_MAX] = {};
    if (snprintf(logout_filename, FILENAME_MAX, "%s%s", log_folder, LOGOUT_FILENAME) <= 0)
    {
        perror("Can't snprintf logout_filename");
        return EXIT_FAILURE;
    }

    char dumb_filename[FILENAME_MAX] = {};
    if (snprintf(dumb_filename, FILENAME_MAX, "%s%s", log_folder, DUMB_FILENAME) <= 0)
    {
        perror("Can't snprintf dumb_filename");
        return EXIT_FAILURE;
    }

    LOGG_ERROR_HANDLE(logger_ctor());
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL));
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename));

    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_ctor());
    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_set_out_file(dumb_filename));
    
    return EXIT_SUCCESS;
}
#undef LOGOUT_FILENAME
#undef   DUMB_FILENAME
