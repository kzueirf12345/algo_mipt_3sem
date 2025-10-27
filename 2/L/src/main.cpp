#include <iostream>
#include <vector>
#include <cmath>

constexpr inline static size_t MAX_ATTACKS = 1e9;

bool CanKillAll(const std::vector<size_t>& hp, size_t p, size_t q, size_t attacks);
size_t FindMinAttacks(const std::vector<size_t>& hp, size_t p, size_t q);

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n = 0, p = 0, q = 0;
    std::vector<size_t> hp = {};

    std::cin >> n >> p >> q;
    hp.resize(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> hp[i];
    }

    size_t result = FindMinAttacks(hp, p, q);

    std::cout << result << std::endl;
    return 0;
}

bool CanKillAll(const std::vector<size_t>& hp, size_t p, size_t q, size_t attacks)
{
    if (p == q) {
        for (size_t i = 0; i < hp.size(); ++i) {
            if (hp[i] > attacks * p) {
                return false;
            }
        }
        return true;
    }
    
    size_t totalNeeded = 0;
    for (size_t i = 0; i < hp.size(); ++i) {
        if (hp[i] > attacks * q) {
            size_t remaining = hp[i] - attacks * q;
            size_t needMain = (remaining + (p - q) - 1) / (p - q); // ceil(rem / (p-q))
            totalNeeded += needMain;
            if (totalNeeded > attacks) {
                return false;
            }
        }
    }
    return true;
}

size_t FindMinAttacks(const std::vector<size_t>& hp, size_t p, size_t q)
{
    size_t left = 0;
    size_t right = MAX_ATTACKS; 
    size_t answer = right;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        if (CanKillAll(hp, p, q, mid)) {
            answer = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return answer;
}
