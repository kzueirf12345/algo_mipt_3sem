#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string input = {};
    std::getline(std::cin, input);
    std::vector<std::string> strStack = {};
    std::vector<char> opStack = {};

    for (size_t pos = 0; pos < input.size(); ++pos) {
        char ch = input[pos];
        if (ch == '(') {
            opStack.push_back('(');
        } else if (ch == ',') {
            opStack.push_back(',');
        } else if (ch == ')') {
            while (!opStack.empty() && opStack.back() == ',') {
                opStack.pop_back();
                
                std::string right = std::move(strStack.back()); 
                strStack.pop_back();
                std::string left = std::move(strStack.back()); 
                strStack.pop_back();
                
                strStack.push_back(left + right);
            }
            
            if (!opStack.empty() && opStack.back() == '(') {
                opStack.pop_back();
            }
            
            if (pos + 1 < input.size() && input[pos + 1] == 'R') {
                if (!strStack.empty()) {
                    std::string &top = strStack.back();
                    std::reverse(top.begin(), top.end());
                }
                ++pos;
            }
        } else {
            std::string token = {};
            while (pos < input.size()) {
                char c = input[pos];
                if (c == '(' || c == ')' || c == ',' || c == 'R') {
                    break;
                }
                token.push_back(c);
                ++pos;
            }
            strStack.push_back(std::move(token));

            if (pos < input.size() && input[pos] == 'R') {
                std::string &top = strStack.back();
                std::reverse(top.begin(), top.end());
                ++pos;
            }
            --pos;
        }
    }

    std::cout << strStack.back() << '\n';

    return 0;
}