#include <iostream>
#include <queue>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t queryCount = 0;
    std::cin >> queryCount;

    std::priority_queue<int> maxHeap = {};
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap = {};

    for (size_t i = 0; i < queryCount; ++i) {
        int type = 0;
        std::cin >> type;

        if (type == 0) {
            int val = 0;
            std::cin >> val;

            if (maxHeap.empty() || val <= maxHeap.top()) {
                maxHeap.push(val);
            } else {
                minHeap.push(val);
            }

            if (maxHeap.size() > minHeap.size() + 1) {
                minHeap.push(maxHeap.top());
                maxHeap.pop();
            } else if (minHeap.size() > maxHeap.size()) {
                maxHeap.push(minHeap.top());
                minHeap.pop();
            }

        } else if (type == 1) {
            std::cerr << "KEK\n";
            std::cout << maxHeap.top() << "\n";
        }
    }
    return 0;
}

