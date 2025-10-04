// #pragma GCC optimize("O3, loop-unroll")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n, q;
    std::cin >> n >> q;
    
    std::vector<long long> arr(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }
    
    std::vector<long long> diff(n + 1, 0);
    
    for (size_t i = 0; i < q; ++i) {
        size_t l, r;
        long long x;
        std::cin >> l >> r >> x;
        
        diff[l]     += x;
        diff[r + 1] -= x;
    }
    
    long long cur_add = 0;
    for (size_t i = 0; i < n; ++i) {
        cur_add += diff[i];
        std::cout << arr[i] + cur_add << " ";
    }
    std::cout << "\n";
    
    return EXIT_SUCCESS;
}