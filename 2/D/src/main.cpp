#include <iostream>
#include <vector>
#include <cstdint>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    uint64_t arr1Size = 0;
    std::cin >> arr1Size;

    std::vector<uint32_t> arr1(arr1Size);
    for (std::size_t i = 0; i < arr1Size; ++i)
    {
        std::cin >> arr1[i];
    }

    size_t ind1 = 0;
    size_t mergedPos = 1;
    uint64_t oddPosSum = 0;

    for (size_t ind2 = 0; ind2 < arr1Size; ++ind2)
    {
        uint32_t elem2;
        std::cin >> elem2;

        while (ind1 < arr1Size && arr1[ind1] <= elem2)
        {
            if (mergedPos % 2 == 1)
                oddPosSum += arr1[ind1];
            
            ++ind1;
            ++mergedPos;
        }

        if (mergedPos % 2 == 1)
            oddPosSum += elem2;
        ++mergedPos;
    }

    while (ind1 < arr1Size)
    {
        if (mergedPos % 2 == 1)
            oddPosSum += arr1[ind1];
        ++ind1;
        ++mergedPos;
    }

    std::cout << (oddPosSum % 1000000000) << "\n";

    return 0;
}
