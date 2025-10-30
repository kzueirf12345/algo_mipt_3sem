#include <iostream>
#include <vector>
#include <algorithm>

inline size_t FindBestIndex(const int* a, const int* b, size_t length)
{
    size_t left = 0;
    size_t right = length - 1;

    while (left < right)
    {
        size_t mid = (left + right) >> 1;

        if (a[mid] < b[mid])
            left = mid + 1;
        else
            right = mid;
    }

    if (left > 0)
    {
        size_t k1 = left - 1;
        if (std::max(a[k1], b[k1]) < std::max(a[left], b[left]))
            return k1;
    }

    return left;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n = 0, m = 0, length = 0;
    std::cin >> n >> m >> length;

    std::vector<std::vector<int>> a(n, std::vector<int>(length));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < length; ++j)
            std::cin >> a[i][j];

    std::vector<std::vector<int>> b(m, std::vector<int>(length));
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < length; ++j)
            std::cin >> b[i][j];

    size_t queryCount = 0;
    std::cin >> queryCount;

    for (size_t q = 0; q < queryCount; ++q)
    {
        size_t i, j;
        std::cin >> i >> j;
        --i; --j;

        const int* pa = a[i].data();
        const int* pb = b[j].data();

        size_t result = FindBestIndex(pa, pb, length);
        std::cout << (result + 1) << '\n';
    }

    return 0;
}
