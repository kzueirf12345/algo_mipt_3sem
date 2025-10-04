// #pragma GCC optimize("O3, loop-unroll")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <climits>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n;
    std::cin >> n;
    
    long long ans = LLONG_MIN;
    long long cur_max = 0;
    
    for (size_t i = 0; i < n; ++i) {
        long long num;
        std::cin >> num;
        
        cur_max = std::max(num, cur_max + num);
        ans     = std::max(ans, cur_max);
    }
    
    std::cout << ans << "\n";
    
    return EXIT_SUCCESS;
}