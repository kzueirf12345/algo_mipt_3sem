#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/types.h>

// FIXME NOT OK

enum State {
    DEFAULT,
    IN_SINGLE_LINE_COMMENT,
    IN_MULTI_LINE_COMMENT,
    IN_STRING_SINGLE,
    IN_STRING_DOUBLE
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    State state = DEFAULT;
    std::string cur_line;
    std::string result;
    
    while (std::getline(std::cin, cur_line)) {
        for (size_t ind = 0; ind < cur_line.size(); ++ind) {
            const char chr = cur_line[ind];
            const char next_chr = (ind + 1 < cur_line.size() ? cur_line[ind + 1] : '\0');

            switch (state) {
                case DEFAULT:
                    if (chr == '/' && next_chr == '/') {
                        result += '\n';
                        state = IN_SINGLE_LINE_COMMENT;
                        ind = cur_line.size();
                    }
                    else if (chr == '/' && next_chr == '*') {
                        result += ' ';
                        state = IN_MULTI_LINE_COMMENT;
                        ++ind;
                    }
                    else if (chr == '\'') {
                        result += chr;
                        state = IN_STRING_SINGLE;
                    }
                    else if (chr == '\"') {
                        result += chr;
                        state = IN_STRING_DOUBLE;
                    }
                    else {
                        result += chr;
                    }
                    break;
                    
                case IN_SINGLE_LINE_COMMENT:
                    break;
                    
                case IN_MULTI_LINE_COMMENT:
                    if (chr == '*' && next_chr == '/') {
                        state = DEFAULT;
                        ++ind;
                    }
                    break;
                    
                case IN_STRING_SINGLE:
                    result += chr;
                    if (chr == '\'') {
                        size_t backslash_cnt = 0;
                        for (ssize_t j = (ssize_t)ind - 1; j >= 0 && cur_line[j] == '\\'; --j) {
                            ++backslash_cnt;
                        }

                        if (backslash_cnt % 2 == 0) {
                            state = DEFAULT;
                        }
                    }
                    break;
                    
                case IN_STRING_DOUBLE:
                    result += chr;
                    if (chr == '\"') {
                        size_t backslash_cnt = 0;
                        for (ssize_t j = (ssize_t)ind - 1; j >= 0 && cur_line[j] == '\\'; --j) {
                            ++backslash_cnt;
                        }

                        if (backslash_cnt % 2 == 0) {
                            state = DEFAULT;
                        }
                    }
                    break;

                default:
                    return EXIT_FAILURE;
            }
        }
        
        if (state == IN_SINGLE_LINE_COMMENT) {
            state = DEFAULT;
        }
        
        if (state !=  IN_MULTI_LINE_COMMENT
         && state != IN_SINGLE_LINE_COMMENT) {
            result += '\n';
        }
    }
    
    std::cout << result;
    
    return 0;
}