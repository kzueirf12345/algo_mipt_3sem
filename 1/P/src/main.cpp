// #pragma GCC optimize("O3, loop-unroll")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n, m;
    std::cin >> n >> m;
    
    std::vector<long long> arr(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }
    
    for (size_t i = 0; i < m; ++i) {
        long long target_sum;
        std::cin >> target_sum;
        
        bool found = false;
        size_t left = 0;
        long long cur_sum = 0;
        
        for (size_t right = 0; right < n; ++right) {
            cur_sum += arr[right];
            
            while (cur_sum > target_sum && left <= right) {
                cur_sum -= arr[left];
                ++left;
            }
            
            if (cur_sum == target_sum) {
                std::cout << (left + 1) << " " << (right + 2) << "\n";
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cout << "Not found\n";
        }
    }
    
    return EXIT_SUCCESS;
}