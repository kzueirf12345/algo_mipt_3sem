#include <iostream>
#include <vector>
#include <algorithm>

size_t FindBestIndex(const std::vector<int>& a, const std::vector<int>& b);

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n = 0, m = 0, length = 0;
    std::cin >> n >> m >> length;

    std::vector<std::vector<int>> a(n, std::vector<int>(length));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < length; ++j) {
            std::cin >> a[i][j];
        }
    }

    std::vector<std::vector<int>> b(m, std::vector<int>(length));
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < length; ++j) {
            std::cin >> b[i][j];
        }
    }

    size_t queryCount = 0;
    std::cin >> queryCount;

    for (size_t q = 0; q < queryCount; ++q) {
        size_t i, j;
        std::cin >> i >> j;
        --i; --j;
        size_t result = FindBestIndex(a[i], b[j]);
        std::cout << (result + 1) << std::endl;
    }

    return 0;
}


/*!SECTION
функция max(a_ik, b_jk) является U-образной. Считай мы делаем поиск 0 производной 
*/
size_t FindBestIndex(const std::vector<int>& a, const std::vector<int>& b)
{
    size_t left = 0;
    size_t right = a.size() - 1;

    while (left < right) {
        size_t mid = (left + right) / 2;
        if (a[mid] < b[mid]) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // Иногда попадаем не ровно
    size_t bestIndex = left;
    if (bestIndex > 0) {
        size_t k1 = bestIndex - 1;
        if (std::max(a[k1], b[k1]) < std::max(a[bestIndex], b[bestIndex])) {
            bestIndex = k1;
        }
    }

    return bestIndex;
}
