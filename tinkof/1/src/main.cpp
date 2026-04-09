#include <iostream>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n, x;

    std::cin >> n >> x;

    std::string str;

    std::cin >> str;

    for (const char ch : str) {
        if (ch == 'x') {
            ++x;
        }
        else if (ch == 'o' && x != 0) {
            --x;
        }
    }

    std::cout << x << std::endl;

    return 0;
}