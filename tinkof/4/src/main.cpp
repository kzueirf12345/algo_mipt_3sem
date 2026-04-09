#include <iostream>

using ull = unsigned long long;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ull n;
    std::cin >> n;

    ull sum = 0;
    ull i = 1;
    
    while (i <= n) {
        ull q = n / i;
        ull last = n / q;
        
        sum += q * (last - i + 1);
        
        i = last + 1;
    }
    
    std::cout << sum << std::endl;

    
    return 0;
}