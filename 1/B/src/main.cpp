#include <iostream>


int main() {

    size_t N = 0;
    std::cin >> N;

    size_t max_reachable = 0;
    
    for (size_t i = 0; i < N; ++i) {
        size_t coin = 0;
        std::cin >> coin;
        if (coin > max_reachable + 1) {
            break;
        }
        max_reachable += coin;
    }
    
    std::cout << max_reachable + 1 << std::endl;
}