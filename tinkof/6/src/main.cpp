#include <iostream>
#include <map>
#include <vector>

using ull = unsigned long long;
using ll = long long;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);


    size_t n;

    std::cin >> n;

    std::vector<ll> arr(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr[i];
    } 

    if (n == 1) {
        std::cout << 1 << std::endl;
        return 0;
    }

    const ll MOD = 998244353;

    std::vector<ll> pref(n - 1);
    ll cur_sum = 0;
    for (size_t i = 0; i < n - 1; ++i) {
        cur_sum += arr[i];
        pref[i] = cur_sum;
    }

    std::vector<ll> dp(n, 0);
    dp[0] = 1;
    
    std::map<ll, ll> last_occurrence;

    for (size_t i = 0; i < n - 1; ++i) {
        ll val = pref[i];
        
        dp[i + 1] = (dp[i] * 2ll) % MOD;
        
        if (last_occurrence.count(val)) {
            dp[i + 1] = (dp[i + 1] - dp[last_occurrence[val]] + MOD) % MOD;
        }
        
        last_occurrence[val] = i;
    }

    std::cout << dp[n - 1] << std::endl;
    
    return 0;
}