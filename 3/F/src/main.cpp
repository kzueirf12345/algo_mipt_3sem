#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cstdint>

struct Item {
    int32_t value;
    uint16_t setId;
};

int main() {
    uint32_t N = 0, M = 0;
    if (scanf("%u %u", &N, &M) != 2) {
        return EXIT_FAILURE;
    }

    std::vector<Item> allItems;
    allItems.reserve(N * M);

    for (uint16_t s = 0; s < N; ++s) {
        for (uint32_t j = 0; j < M; ++j) {
            Item it = {.value = 0, .setId = s};
            if (scanf("%d", &it.value) != 1) {
                return EXIT_FAILURE;
            }
            allItems.push_back(it);
        }
    }

    std::sort(
        allItems.begin(), 
        allItems.end(),
        [](const Item &a, const Item &b) {
            return a.value < b.value;
        }
    );

    std::vector<uint16_t> pairCnt(N * N, 0);
    uint16_t res = 0;

    size_t pos = 0;
    const size_t items_cnt = allItems.size();

    while (pos < items_cnt) {
        const int32_t curVal = allItems[pos].value;
        size_t start = pos;

        while (pos < items_cnt && allItems[pos].value == curVal) {
            ++pos;
        }

        // тут pos - последний элемент со значением curVal

        for (size_t i = start; i < pos; ++i) {
            for (size_t j = i + 1; j < pos; ++j) {
                uint16_t a = allItems[i].setId;
                uint16_t b = allItems[j].setId;

                if (a > b) {
                    std::swap(a, b);
                }

                uint32_t index = (uint32_t)a * N + b;

                ++pairCnt[index];

                if (pairCnt[index] > res) {
                    res = pairCnt[index];
                }
            }
        }
    }

    printf("%hu\n", res);
    return 0;
}
