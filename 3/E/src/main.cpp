#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        size_t h = 0;
        for (int x : v) {
            h ^= std::hash<int>()(x);
        }
        return h;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t N, M, K;
    std::cin >> N >> M >> K;

    std::unordered_set<std::vector<int>, VectorHash> referenceSets;

    for (size_t i = 0; i < N; ++i) {
        std::vector<int> s(M);
        for (size_t j = 0; j < M; ++j) std::cin >> s[j];
        std::sort(s.begin(), s.end());
        referenceSets.insert(s);
    }

    for (size_t i = 0; i < K; ++i) {
        std::vector<int> query(M);
        for (size_t j = 0; j < M; ++j) std::cin >> query[j];
        std::sort(query.begin(), query.end());
        std::cout << (referenceSets.count(query) ? 1 : 0) << std::endl;
    }

    return 0;
}
