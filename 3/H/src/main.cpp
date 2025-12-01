#include <cstdlib>
#include <cstdio>
#include <unordered_map>
#include <cmath>

int main() {
    size_t N = 0;

    if (scanf("%zu", &N) != 1) {
        return EXIT_FAILURE;
    }

    std::unordered_map<int,int> pos;
    pos.reserve(2 * N);

    for (size_t i = 0; i < N; ++i) {
        int x, y;

        if (scanf("%d %d", &x, &y) != 2) {
            return EXIT_FAILURE;
        }

        int px = pos[x] ? pos[x] : x;
        int py = pos[y] ? pos[y] : y;

        printf("%u\n", std::abs(px - py));

        pos[x] = py;
        pos[y] = px;
    }
}
