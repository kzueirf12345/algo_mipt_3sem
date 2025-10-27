#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>

double MergeBrokers(const std::vector<size_t>& capitals);

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t departmentCount = 0;
    std::cin >> departmentCount;

    std::vector<size_t> capitals(departmentCount);
    for (size_t i = 0; i < departmentCount; ++i) {
        std::cin >> capitals[i];
    }

    double minCost = MergeBrokers(capitals);

    std::cout << std::fixed << std::setprecision(2) << minCost << "\n";

    return 0;
}

double MergeBrokers(const std::vector<size_t>& capitals)
{
    std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> minHeap;
    for (size_t capital : capitals) {
        minHeap.push(capital);
    }

    double totalCost = 0.0;

    while (minHeap.size() > 1) {
        const size_t first  = minHeap.top(); minHeap.pop();
        const size_t second = minHeap.top(); minHeap.pop();

        const double mergeCost = 0.01 * static_cast<double>(first + second);
        totalCost += mergeCost;

        minHeap.push(first + second);
    }

    return totalCost;
}
