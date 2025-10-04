#include <iostream>
#include <stack>
#include <vector>

int main() {
    size_t n;
    std::cin >> n;

    std::vector<size_t> heights(n + 1);

    for (size_t i = 0; i < n; ++i) {
        std::cin >> heights[i];
    }

    heights[n] = 0;

    std::stack<size_t> stack;

    size_t max_area = 0;

    for (size_t i = 0; i < n + 1; ++i) {
        while (!stack.empty() && heights[i] < heights[stack.top()]) {
            size_t height = heights[stack.top()];

            stack.pop();

            size_t width = i - (stack.empty() ? 0 : stack.top() + 1);

            max_area = std::max(max_area, height * width);
        }
        stack.push(i);
    }

    std::cout << max_area << std::endl;

    return 0;
}