#include <cstddef>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>

// FIXME not work in last test

int main() {
    size_t N, M;
    std::cin >> N >> M;
    std::unordered_map<int32_t, std::vector<size_t>> elementSets;

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            int32_t elem;
            std::cin >> elem;
            elementSets[elem].push_back(i);
        }
    }

    std::unordered_map<uint64_t, size_t> intersectionCount;
    size_t maxIntersection = 0;

    for (const auto& [elem, indices] : elementSets) {
        for (size_t i = 0; i < indices.size(); ++i) {
            for (size_t j = i + 1; j < indices.size(); ++j) {
                size_t a = indices[i], b = indices[j];
                if (a > b) {
                    std::swap(a, b);
                }
                uint64_t key = ((uint64_t)a << 32) | b;
                size_t cnt = ++intersectionCount[key];

                if (cnt > maxIntersection) {
                    maxIntersection = cnt;
                }
            }
        }
    }

    std::cout << maxIntersection << std::endl;
    return 0;


}
