#include <stdlib.h>
#include <stdio.h>

#include "b_tree/verification/verification.h"
#include "flags/flags.h"
#include "logger/src/logger.h"
#include "utils/utils.h"
#include "b_tree/funcs/funcs.h"
#include "utils/concole.h"

static unsigned init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
static unsigned dtor_all(flags_objs_t* const flags_objs);

int main(const int argc, char* const argv[]) 
{
    flags_objs_t flags_objs = {};
    MAIN_INT_ERROR_HANDLE(init_all(&flags_objs, argc, argv));
    
    b_tree_t tree = {};
    B_TREE_INT_ERROR_HANDLE(
        b_tree_ctor(&tree, 100), 
        dtor_all(&flags_objs);
    );

    // logg(LOG_LEVEL_DETAILS_ERROR, "smert");

    for (size_t i = tree.t * 2; i > 2; --i)
    {
        B_TREE_INT_ERROR_HANDLE(
            b_tree_insert(&tree, (int)i),
            dtor_all(&flags_objs);
        );
    }
    
    // B_TREE_INT_ERROR_HANDLE(
    //     b_tree_insert(&tree, (int)10),
    //     dtor_all(&flags_objs);
    // );
    // B_TREE_INT_ERROR_HANDLE(
    //     b_tree_insert(&tree, (int)4),
    //     dtor_all(&flags_objs);
    // );



    // fprintf(stderr, "inserted num: %d\n", tree.root->keys[0]);

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

    LOGG_ERROR_HANDLE(logger_ctor());
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL));
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename));
    
    return EXIT_SUCCESS;
}
#undef LOGOUT_FILENAME
#undef   DUMB_FILENAME