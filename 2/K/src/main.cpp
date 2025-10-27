#include <iostream>
#include <numeric>
#include <vector>
#include <algorithm>

bool CanDivide(const std::vector<size_t>& chapters, size_t maxVolume, size_t k);
size_t FindMinimalVolume(const std::vector<size_t>& chapters, size_t k);

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t k = 0;
    size_t n = 0;
    std::vector<size_t> chapters;

    std::cin >> k >> n;
    chapters.resize(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> chapters[i];
    }

    size_t minimalVolume = FindMinimalVolume(chapters, k);

    std::cout << minimalVolume << std::endl;

    return 0;
}

bool CanDivide(const std::vector<size_t>& chapters, size_t maxVolume, size_t k)
{
    size_t volumesUsed = 1;
    size_t currentVolume = 0;

    for (size_t i = 0; i < chapters.size(); ++i) {
        if (chapters[i] > maxVolume) {
            return false;
        }

        if (currentVolume + chapters[i] <= maxVolume) {
            currentVolume += chapters[i];
        } else {
            ++volumesUsed;
            currentVolume = chapters[i];
        }
    }

    return volumesUsed <= k;
}

size_t FindMinimalVolume(const std::vector<size_t>& chapters, size_t k)
{
    size_t left = *std::max_element(chapters.begin(), chapters.end());
    size_t right = std::accumulate(chapters.begin(), chapters.end(), 0ull);

    size_t answer = right;

    while (left <= right) {
        size_t mid = left + (right - left) / 2;

        if (CanDivide(chapters, mid, k)) {
            answer = mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return answer;
}
