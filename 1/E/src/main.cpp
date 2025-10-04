#include <iostream>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n, k;
    std::cin >> n >> k;
    
    size_t result = 1;
    size_t current = 1;
    
    while (current != k) {
        if (current * 10 <= n) {
            current *= 10;
        }
        else {
            while (current % 10 == 9 || current >= n) {
                current /= 10;
            }
            ++current;
        }
        ++result;
    }
    
    std::cout << result << "\n";
    
    return 0;
}