#include <functional>
#include <iostream>
#include <queue>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t count = 0;
    size_t topCount = 0;
    std::priority_queue<long long, std::vector<long long>, std::greater<long long>> heap = {};

    std::cin >> count >> topCount;

    for (size_t i = 0; i < count; ++i) {
        long long elem;
        std::cin >> elem;

        if (heap.size() < topCount) {
            heap.push(elem);
        } else if (elem > heap.top()) {
            heap.pop();
            heap.push(elem);
        }
    }

    std::vector<long long> res(topCount);

    for (size_t i = 0; i < topCount; ++i) {
        res[i] = heap.top();
        heap.pop();
    }

    std::cerr << "start output\n";

    for (size_t i = 0; i < topCount; ++i) {
        std::cout << res[topCount - i - 1] << std::endl;
    }

    return 0;
}