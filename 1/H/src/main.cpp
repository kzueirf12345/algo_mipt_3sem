#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/types.h>

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
        cur_line += '\n';
        for (size_t ind = 0; ind < cur_line.size(); ++ind) {
            const char chr = cur_line[ind];
            const char next_chr = (ind + 1 < cur_line.size() ? cur_line[ind + 1] : '\0');

            switch (state) {
                case DEFAULT:
                    if (chr == '/' && next_chr == '/') {
                        state = IN_SINGLE_LINE_COMMENT;
                        result += '\n';
                        ind = cur_line.size();
                    }
                    else if (chr == '/' && next_chr == '*') {
                        state = IN_MULTI_LINE_COMMENT;
                        result += ' ';
                        ++ind;
                    }
                    else if (chr == '\'') {
                        state = IN_STRING_SINGLE;
                        result += chr;
                    }
                    else if (chr == '\"') {
                        state = IN_STRING_DOUBLE;
                        result += chr;
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
                    if (chr == '\\' && next_chr != '\0') {
                        result += next_chr;
                        ++ind;
                    } else if (chr == '\'') {
                        state = DEFAULT;
                    }
                    break;
                    
                case IN_STRING_DOUBLE:
                    result += chr;
                    if (chr == '\\' && next_chr != '\0') {
                        result += next_chr;
                        ++ind;
                    } else if (chr == '"') {
                        state = DEFAULT;
                    }
                    break;

                default:
                    return EXIT_FAILURE;
            }
            
        }
        if (state == IN_SINGLE_LINE_COMMENT) {
            state = DEFAULT;
        }
    }
    
    std::cout << result;
    
    return 0;
}