#include <iostream>
#include <cstdint>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint64_t numerator = 0;
    std::uint64_t denominator = 0;
    std::cin >> numerator >> denominator;

    for (std::uint64_t position = 2; numerator != 0; ++position)
    {
        numerator *= position;
        std::uint64_t coefficient = numerator / denominator;
        std::cout << coefficient << " ";
        numerator = numerator % denominator;
    }

    std::cout << "\n";

    return 0;
}
