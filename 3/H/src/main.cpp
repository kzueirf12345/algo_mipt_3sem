#include <iostream>
#include <unordered_map>
#include <cmath>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t N = 0;
    std::cin >> N;

    std::unordered_map<int,int> pos;
    pos.reserve(2 * N);

    for (size_t i = 0; i < N; ++i) {
        int x, y;
        std::cin >> x >> y;

        int px = pos[x] ? pos[x] : x;
        int py = pos[y] ? pos[y] : y;

        std::cout << std::abs(px - py) << '\n';

        pos[x] = py;
        pos[y] = px;
    }
}
