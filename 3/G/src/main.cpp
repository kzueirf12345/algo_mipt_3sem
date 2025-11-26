#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

int main() {
    size_t N, M;
    std::cin >> N >> M;
    std::vector<int64_t> requests(M);
    for (size_t i = 0; i < M; ++i) {
        std::cin >> requests[i];
    }

    std::unordered_map<int64_t, std::vector<size_t>> futurePositions;
    for (size_t i = 0; i < M; ++i) {
        futurePositions[requests[i]].push_back(i);
    }

    std::set<std::pair<size_t, int64_t>> cache;
    std::unordered_map<int64_t,size_t> inCache;
    size_t result = 0;

    for (size_t i = 0; i < M; ++i) {
        int64_t x = requests[i];
        futurePositions[x].erase(futurePositions[x].begin());

        size_t nextPos = (futurePositions[x].empty() ? M + 1 : futurePositions[x][0]);

        if (inCache.count(x)) {
            cache.erase({inCache[x], x});
            cache.insert({nextPos, x});
            inCache[x] = nextPos;
        } else {
            ++result;
            if (cache.size() == N) {
                auto it = --cache.end();
                inCache.erase(it->second);
                cache.erase(it);
            }
            cache.insert({nextPos, x});
            inCache[x] = nextPos;
        }
    }

    std::cout << result << std::endl;
}
