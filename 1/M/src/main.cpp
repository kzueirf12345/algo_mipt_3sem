#include <iostream>
#include <vector>
#include <climits>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n;
    std::cin >> n;
    
    std::vector<long long> arr1(n), arr2(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr1[i];
    }
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr2[i];
    }
    
    long long max_sum = LLONG_MIN;
    size_t best_i = 0, best_j = 0;
    size_t cur_maxi = 0;
    for (size_t j = 0; j < n; ++j) {
        if (arr1[j] > arr1[cur_maxi] || (arr1[j] == arr1[cur_maxi] && j < cur_maxi)) {
            cur_maxi = j;
        }
        
        long long cur_sum = arr1[cur_maxi] + arr2[j];
        
        if (cur_sum > max_sum) {
            max_sum = cur_sum;
            best_i = cur_maxi;
            best_j = j;
        }
        else if (cur_sum == max_sum) {
             if (cur_maxi  < best_i 
             || (cur_maxi == best_i && j < best_j)) {
                best_i = cur_maxi;
                best_j = j;
            }
        }
    }
    
    std::cout << best_i << " " << best_j  << "\n";
    
    return 0;
}