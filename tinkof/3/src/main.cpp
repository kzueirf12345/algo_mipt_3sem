#include <iostream>

using ull = unsigned long long;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string s;

    std::cin >> s;

    ull n;

    std::cin >> n;

    const size_t size = s.size();

    ull min_num = 0;
    for (size_t i = 0; i < size; i++) {
        min_num <<= 1;

        if (s[i] == '1') {
            min_num |= 1;
        }
    }
    
    if (min_num > n) {
        std::cout << -1 << std::endl;
        return 0;
    }
    
    ull result = min_num;
    
    for (size_t i = 0; i < size; i++) {
        if (s[i] == '?') {
            ull candidate = result | (1LL << (size - 1 - i));
            
            if (candidate <= n) {
                result = candidate;
            }
        }
    }
    
    std::cout << result << std::endl;

    
    return 0;
}