// #pragma GCC optimize("O3, loop-unroll")
// #pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n, m;
    std::cin >> n >> m;
    
    std::vector<long long> arr(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }
    
    std::sort(arr.begin(), arr.end());
    
    for (size_t i = 0; i < m; i++) {
        long long num;
        std::cin >> num;
        
        bool found = false;
        size_t left = 0, right = n - 1;
        std::pair<long long, long long> best_pair;
        
        while (left < right) {
            long long sum = arr[left] + arr[right];
            
            if (sum == num) {
                if (!found || arr[left] < best_pair.first) {
                    best_pair = {arr[left], arr[right]};
                }
                found = true;
                ++left;
            }
            else if (sum < num) {
                ++left;
            }
            else {
                --right;
            }
        }
        
        if (found) {
            std::cout << best_pair.first << " " << best_pair.second << "\n";
        }
        else {
            std::cout << "Not found\n";
        }
    }
    
    return EXIT_SUCCESS;
}