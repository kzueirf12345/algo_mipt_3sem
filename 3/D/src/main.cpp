#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct Coord {
    int x;
    int y;
};

struct HashCoord {
    size_t operator()(const Coord& c) const {
        return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) << 1);
    }
};

bool operator==(const Coord& a, const Coord& b);

bool operator==(const Coord& a, const Coord& b) {
    return a.x == b.x && a.y == b.y;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n = 0, m = 0, l = 0, k = 0;
    std::cin >> n >> m >> l >> k;

    std::unordered_set<Coord, HashCoord> alive;
    for (size_t i = 0; i < l; ++i) {
        Coord c;
        std::cin >> c.x >> c.y;
        alive.insert(c);
    }

    std::vector<std::pair<int, int>> dirs = {
        {-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}
    };

    for (size_t step = 0; step < k; ++step) {
        std::unordered_map<Coord, size_t, HashCoord> neighborCount;
        for (auto& c : alive) {
            for (auto& d : dirs) {
                int nx = c.x + d.first;
                int ny = c.y + d.second;
                if (nx >= 0 && nx < (int)n && ny >= 0 && ny < (int)m) {
                    Coord nc{nx, ny};
                    neighborCount[nc]++;
                }
            }
        }

        std::unordered_set<Coord, HashCoord> nextAlive;
        for (auto& [c, cnt] : neighborCount) {
            if (alive.find(c) != alive.end()) {
                if (cnt == 2 || cnt == 3) nextAlive.insert(c);
            } else {
                if (cnt == 3) nextAlive.insert(c);
            }
        }

        alive = std::move(nextAlive);
    }

    std::vector<Coord> result(alive.begin(), alive.end());
    std::sort(result.begin(), result.end(), 
        [](const Coord& a, const Coord& b){
            return a.x != b.x ? a.x < b.x : a.y < b.y;
        }
    );

    for (auto& c : result) {
        std::cout << c.x << " " << c.y << std::endl;
    }

    return 0;
}