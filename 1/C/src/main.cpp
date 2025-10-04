#include <iostream>

char GetChar(size_t n, size_t pos);

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    size_t n;
    size_t k, l;
    std::cin >> n >> k >> l;
    
    for (size_t pos = k; pos <= l; ++pos) {
        std::cout << GetChar(n, pos);
    }
    std::cout << "\n";
    
    return 0;
}

char GetChar(size_t n, size_t pos) {
    if (n == 1) {
        return 'a';
    }
    
    size_t prev_len = (1ull << (n - 1)) - 1;
    
    if (pos == 1) {
        return 'a' + (char)n - 1;
    }
    else if (pos - 1 <= prev_len) {
        return GetChar(n - 1, pos - 1);
    }
    else {
        return GetChar(n - 1, pos - 1 - prev_len);
    }
}