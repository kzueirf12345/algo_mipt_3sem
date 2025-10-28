// b_tree/dumb/dumb.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "b_tree/dumb/dumb.h"
#include "b_tree/structs.h"
#include "utils/utils.h"
#include "logger/src/logger.h"

/* -------------------------------------------------------------------------
   HTML intro 
   ------------------------------------------------------------------------- */
static const char* const HTML_INTRO_ =
    "\n<!DOCTYPE html>\n"
    "<html lang='en'>\n"
        "<head>\n"
            "<meta charset='UTF-8'>\n"
            "<title>B-TREE DUMB</title>\n"
        "</head>\n"
        "<body>\n"
            "<pre>\n";


#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* b_tree_dumb_strerror(const enum BTreeDumbError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(B_TREE_DUMB_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(B_TREE_DUMB_ERROR_FAILURE);
    default:
        return "UNKNOWN_B_TREE_DUMB_ERROR";
    }
    return "UNKNOWN_B_TREE_DUMB_ERROR";
}
#undef CASE_ENUM_TO_STRING_

/* -------------------------------------------------------------------------
   Global dumper state
   ------------------------------------------------------------------------- */
static struct 
{
    char* out_name;

    char* html_name;
    char* dot_name;
    char* svg_name;
    char* graph_count_name;

    FILE* dot_file;
    FILE* html_file;

    size_t graph_count;
} DUMBER_ = {};

static void DUMBER_is_init_lasserts_(void)
{
    lassert(DUMBER_.html_name,          "DUMBER_ is not init");
    lassert(DUMBER_.html_file,          "DUMBER_ is not init");
    lassert(DUMBER_.dot_name,           "DUMBER_ is not init");
    lassert(DUMBER_.dot_file,           "DUMBER_ is not init");
    lassert(DUMBER_.svg_name,           "DUMBER_ is not init");
    lassert(DUMBER_.graph_count_name,   "DUMBER_ is not init");
}

enum BTreeDumbError set_graph_count_          (void);
enum BTreeDumbError write_graph_count_in_file_(void);

enum BTreeDumbError b_tree_dumb_ctor(void)
{
    lassert(!DUMBER_.html_name         || !DUMBER_.html_file, "");
    lassert(!DUMBER_.dot_name          || !DUMBER_.dot_file,  "");
    lassert(!DUMBER_.svg_name,                                "");
    lassert(!DUMBER_.graph_count_name,                        "");
    lassert(!DUMBER_.out_name,                                "");

    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_set_out_file("./log/b_tree_dumb"));
    return B_TREE_DUMB_ERROR_SUCCESS;
}

enum BTreeDumbError b_tree_dumb_dtor(void)
{
    DUMBER_is_init_lasserts_();

    B_TREE_DUMB_ERROR_HANDLE(write_graph_count_in_file_());

    if (fclose(DUMBER_.html_file))
    {
        perror("Can't close html_file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    if (fclose(DUMBER_.dot_file))
    {
        perror("Can't close dot_file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    free(DUMBER_.out_name);         IF_DEBUG(DUMBER_.out_name           = NULL;)
    free(DUMBER_.html_name);        IF_DEBUG(DUMBER_.html_name          = NULL;)
    free(DUMBER_.dot_name);         IF_DEBUG(DUMBER_.dot_name           = NULL;)
    free(DUMBER_.svg_name);         IF_DEBUG(DUMBER_.svg_name           = NULL;)
    free(DUMBER_.graph_count_name); IF_DEBUG(DUMBER_.graph_count_name   = NULL;)

    return B_TREE_DUMB_ERROR_SUCCESS;
}

/* ------------------ graph count management ------------------ */

static bool is_set_graph_count_ = false;
enum BTreeDumbError set_graph_count_(void) //NOTE - non assertable
{
    is_set_graph_count_ = true;

    if (access(DUMBER_.graph_count_name, F_OK))
    {
        errno = 0;
        DUMBER_.graph_count = 0;
        return B_TREE_DUMB_ERROR_SUCCESS;
    }

    FILE* const graph_count_file = fopen(DUMBER_.graph_count_name, "rb");
    if (!graph_count_file)
    {
        perror("Can't open graph_count_file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    if (fscanf(graph_count_file, "%zu", &DUMBER_.graph_count) <= 0)
    {
        perror("Can't fscanf graph_count");
        fclose(graph_count_file);
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    if (fclose(graph_count_file))
    {
        perror("Can't close graph_count_file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    return B_TREE_DUMB_ERROR_SUCCESS;
}

enum BTreeDumbError write_graph_count_in_file_(void)
{
    DUMBER_is_init_lasserts_();

    FILE* const graph_count_file = fopen(DUMBER_.graph_count_name, "wb");
    if (!graph_count_file)
    {
        perror("Can't open graph_count_file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    if (fprintf(graph_count_file, "%zu", DUMBER_.graph_count) <= 0)
    {
        perror("Can't fprintf graph_count");
        fclose(graph_count_file);
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    if (fclose(graph_count_file))
    {
        perror("Can't close graph_count_file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    return B_TREE_DUMB_ERROR_SUCCESS;
}

/* ------------------ set filenames / open files ------------------ */

enum BTreeDumbError b_tree_dumb_set_out_file_(char*  const filename, FILE** const file, 
                                              char** const old_filename, const char* const mode,
                                              const char*  const file_extension);

enum BTreeDumbError b_tree_dumb_set_out_filename_(char*  const filename, 
                                                  const char*  const file_extension,
                                                  char** const old_filename);

enum BTreeDumbError b_tree_dumb_set_out_file(char* const filename)
{
    lassert(filename, "");

    B_TREE_DUMB_ERROR_HANDLE(
        b_tree_dumb_set_out_file_(filename, &DUMBER_.html_file, &DUMBER_.html_name, "ab", ".html")
    );
    B_TREE_DUMB_ERROR_HANDLE(
        b_tree_dumb_set_out_file_(filename, &DUMBER_.dot_file,  &DUMBER_.dot_name,  "wb", ".dot")
    );

    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_set_out_filename_(filename, "",     &DUMBER_.out_name));
    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_set_out_filename_(filename, ".svg", &DUMBER_.svg_name));
    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_set_out_filename_(filename, "_graph_count.txt", 
                                                          &DUMBER_.graph_count_name));

    B_TREE_DUMB_ERROR_HANDLE(set_graph_count_());

    return B_TREE_DUMB_ERROR_SUCCESS;
}

enum BTreeDumbError b_tree_dumb_set_out_file_(char*  const filename, FILE** const file, 
                                              char** const old_filename, const char* const mode,
                                              const char*  const file_extension)
{
    lassert(filename, "");
    lassert(file, "");
    lassert(old_filename, "");
    lassert(file_extension, "");

    B_TREE_DUMB_ERROR_HANDLE(b_tree_dumb_set_out_filename_(filename, file_extension, old_filename));

    if (*file && fclose(*file))
    {  
        perror("Can't close file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }
    
    if (!(*file = fopen(*old_filename, mode))){
        perror("Can't open file");
        return B_TREE_DUMB_ERROR_FAILURE;
    }
    
    return B_TREE_DUMB_ERROR_SUCCESS;
}


enum BTreeDumbError b_tree_dumb_set_out_filename_(char*  const filename, 
                                                 const char*  const file_extension,
                                                 char** const old_filename)
{
    lassert(filename, "");
    lassert(file_extension, "");

    free(*old_filename);

    *old_filename = calloc(FILENAME_MAX, sizeof(char));

    if (!*old_filename)
    {
        perror("Can't calloc old_filename");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    if (snprintf(*old_filename, FILENAME_MAX , "%s%s", filename, file_extension) <= 0)
    {
        perror("Can't snprintf old_filename");
        return B_TREE_DUMB_ERROR_FAILURE;
    }

    return B_TREE_DUMB_ERROR_SUCCESS;
}

/* ------------------ helpers ------------------ */

#define DUMB_AND_FPRINTF_(format, ...)                                                              \
        do {                                                                                        \
            fprintf(DUMBER_.html_file, format, ##__VA_ARGS__);                                      \
            fprintf(stderr,       format, ##__VA_ARGS__);                                           \
        } while(0)

static const char* handle_invalid_ptr_(const void* const check_ptr)
{
    switch (is_invalid_ptr(check_ptr))
    {
    case PTR_STATES_VALID:
        return NULL;
    case PTR_STATES_NULL:
        return "NULL";
    case PTR_STATES_INVALID:
        return "INVALID";
    case PTR_STATES_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }

    return "UNKNOWN";
}
#undef DUMB_AND_FPRINTF_

/* -------------------------------------------------------------------------
   B-tree specific .dot generation
   ------------------------------------------------------------------------- */

static size_t b_node_count_ = 0;

static int create_b_tree_dot_(const b_tree_t* const tree, const elem_to_str_t elem_to_str);
static int create_b_tree_svg_(void);
static int insert_b_tree_svg_(void);
static int create_b_tree_dot_recursive_(const b_tree_node_t* const node, const elem_to_str_t elem_to_str);

void b_tree_dumb (const b_tree_t* const tree, elem_to_str_t elem_to_str)
{
    if (!elem_to_str)
    {
        elem_to_str = data_to_str;
    }

    if (!DUMBER_.html_file || !DUMBER_.dot_file)
    {
        fprintf(stderr, "DUMPER not initialized. Call b_tree_dumb_set_out_file first\n");
        return;
    }

    if (is_empty_file(DUMBER_.html_file) == 0) fprintf(DUMBER_.html_file, HTML_INTRO_);

    fprintf(DUMBER_.html_file, "</pre><hr /><pre>\n");   

    fprintf(DUMBER_.html_file, "\n==B-TREE DUMB==\nDate: %s\nTime: %s\n\n", __DATE__, __TIME__);

    const char* tree_buf = handle_invalid_ptr_(tree);

    if (tree_buf)
    {
        fprintf(DUMBER_.html_file, "tree[%s]\n", tree_buf);
        fprintf(stderr, "tree[%s]\n", tree_buf);
        return;
    }

    fprintf(DUMBER_.html_file, "tree[%p]\n\n", tree);

    if (tree)
    {
        fprintf(DUMBER_.html_file, "tree->root = %p\n", tree->root);
        fprintf(DUMBER_.html_file, "tree->t = %zu\n", tree->t);
    }

    if (!is_set_graph_count_ && set_graph_count_())
    {
        fprintf(stderr, "Can't set graph_count_\n");
        return;
    }

    if (create_b_tree_dot_(tree, elem_to_str))
    {
        fprintf(DUMBER_.html_file, "Can't create B-tree dot\n");
        return;
    }

    if (create_b_tree_svg_())
    {
        fprintf(DUMBER_.html_file, "Can't create B-tree svg\n");
        return;
    }

    if (insert_b_tree_svg_())
    {
        fprintf(DUMBER_.html_file, "Can't insert B-tree svg\n");
        return;
    }

    ++DUMBER_.graph_count;

    fprintf(DUMBER_.html_file, "</pre><hr /><pre>\n");
}

static int create_b_tree_dot_(const b_tree_t* const tree, const elem_to_str_t elem_to_str)
{
    if (is_invalid_ptr(tree))          return -1;

    fprintf(DUMBER_.dot_file, "digraph BTree {\n"
                              "rankdir=TB;\n"
                              "node [shape=record];\n");

    b_node_count_ = 0;

    if (create_b_tree_dot_recursive_(tree->root, elem_to_str))
    {
        fprintf(stderr, "create_b_tree_dot_recursive_ failed\n");
        return -1;
    }

    fprintf(DUMBER_.dot_file, "}\n");
    return 0;
}

static int create_b_tree_dot_recursive_(const b_tree_node_t* const node, const elem_to_str_t elem_to_str)
{
    if (is_invalid_ptr(node))           return  0;

    const size_t my_id = b_node_count_++;

    fprintf(DUMBER_.dot_file, "node%zu [label = \"", my_id);

    fprintf(DUMBER_.dot_file, "<f0> ");
    for (size_t i = 0; i < node->keys_cnt; ++i)
    {
        if (elem_to_str)
        {
            char keybuf[FILENAME_MAX] = {0};
            int rc = elem_to_str(&node->keys[i], sizeof(node->keys[i]), keybuf, sizeof(keybuf));
            if (rc == 0)
                fprintf(DUMBER_.dot_file, "%s", keybuf);
            else
                fprintf(DUMBER_.dot_file, "%d", node->keys[i]);
        }
        else
        {
            fprintf(DUMBER_.dot_file, "%d", node->keys[i]);
        }

        fprintf(DUMBER_.dot_file, " | <f%zu> ", i + 1);
    }

    fprintf(DUMBER_.dot_file, "\"];\n");

    if (!node->is_leaf && node->children)
    {
        for (size_t i = 0; i <= node->keys_cnt; ++i)
        {
            b_tree_node_t* child = node->children[i];
            if (!child) continue;

            const size_t expected_child_id = b_node_count_;

            fprintf(DUMBER_.dot_file, "node%zu:f%zu -> node%zu;\n", my_id, i, expected_child_id);

            if (create_b_tree_dot_recursive_(child, elem_to_str))
            {
                fprintf(stderr, "Failed to create dot for child\n");
                return -1;
            }
        }
    }

    return 0;
}

static int create_b_tree_svg_(void)
{
    if (fclose(DUMBER_.dot_file))
    {
        perror("Can't fclose dot file");
        return -1;
    }

    static const size_t CREATE_SVG_CMD_SIZE = 512;
    char* create_svg_cmd = calloc(CREATE_SVG_CMD_SIZE, sizeof(char));

    if (!create_svg_cmd)
    {
        fprintf(stderr, "Can't calloc create_svg_cmd\n");
        return -1;
    }

    if (snprintf(create_svg_cmd, CREATE_SVG_CMD_SIZE, 
                 "dot -Tsvg %s -o %s%zu.svg >/dev/null 2>&1", 
                 DUMBER_.dot_name, DUMBER_.out_name, DUMBER_.graph_count) <= 0)
    {
        fprintf(stderr, "Can't snprintf create_svg_cmd\n");
        free(create_svg_cmd); create_svg_cmd = NULL;
        return -1;
    }
    
    if (system(create_svg_cmd))
    {
        fprintf(stderr, "Can't call system create svg\n");
        free(create_svg_cmd); create_svg_cmd = NULL;
        return -1;
    }

    free(create_svg_cmd); create_svg_cmd = NULL;

    if (!(DUMBER_.dot_file = fopen(DUMBER_.dot_name, "wb")))
    {
        perror("Can't fopen dot file");
        return -1;
    }

    return 0;
}

static int insert_b_tree_svg_(void)
{
    const char* filename_without_path = DUMBER_.out_name;
    while (strchr(filename_without_path, '/') != NULL)
    {
        filename_without_path = strchr(filename_without_path, '/') + 1;
    }

    fprintf(DUMBER_.html_file, "<img src=%s%zu.svg width=800>\n", 
            filename_without_path, DUMBER_.graph_count);

    return 0;
}
