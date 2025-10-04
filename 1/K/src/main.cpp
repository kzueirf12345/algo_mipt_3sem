#include <iostream>
#include <stack>
#include <string>

enum OpenBreket {
    OPEN_FIGURE = '{',
    OPEN_RECTAN = '[',
    OPEN_CIRCLE = '(',
    OPEN_TRIANG = '<',
};

constexpr size_t BRAKET_TYPES_CNT = 4;
constexpr char  OpenBrakets[BRAKET_TYPES_CNT] = {'{', '[', '(', '<'};
constexpr char CloseBrakets[BRAKET_TYPES_CNT] = {'}', ']', ')', '>'};

int main() {

    std::string str;

    std::cin >> str;

    std::stack<char> stack;
    for (const char& ch : str) {

        for (size_t i = 0; i < BRAKET_TYPES_CNT; ++i) {
            if (ch == OpenBrakets[i]) {
                stack.push(CloseBrakets[i]);
                break;
            } else if (ch == CloseBrakets[i]) {
                if (!stack.empty() && stack.top() == CloseBrakets[i]) {
                    stack.pop();
                    break;
                } else {
                    std::cout << "NO\n";
                    return 0;
                }
            }
        }
    }

    std::cout << (stack.empty() ? "YES" : "NO") << std::endl ;

    return 0;
}