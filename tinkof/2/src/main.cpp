#include <iostream>

using ull = unsigned long long;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n;

    std::cin >> n;

    ull max_x = 0;
    ull p2 = 1;

    while (p2 <= n) {
        ull p3 = 1;

        while (p2 * p3 <= n) {
            max_x = std::max(max_x, p2 * p3);
            
            if (p3 > n / 3) break;

            p3 *= 3;
        }

        if (p2 > n / 2) break;

        p2 *= 2;
    }

    std::cout << max_x << std::endl;
    
    return 0;
}