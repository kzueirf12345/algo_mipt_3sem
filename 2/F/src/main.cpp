#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    uint64_t storageCount = 0;
    std::cin >> storageCount;

    std::vector<uint64_t> capacities(storageCount);
    uint64_t totalCapacity = 0;
    uint64_t maxCapacity = 0;

    for (size_t i = 0; i < storageCount; ++i)
    {
        std::cin >> capacities[i];
        totalCapacity += capacities[i];
        if (capacities[i] > maxCapacity)
            maxCapacity = capacities[i];
    }

    uint64_t maxReliableBlocks = std::min(totalCapacity / 2, totalCapacity - maxCapacity);

    std::cout << maxReliableBlocks << std::endl;

    return 0;
}
