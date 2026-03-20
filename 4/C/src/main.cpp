#include <iostream>
#include <limits>
#include <vector>
#include <cmath>
#include <cstdlib>

const int64_t HEURISTIC_WEIGHT_DISTANCE = 25;
const int64_t HEURISTIC_WEIGHT_DURABILITY = 5;
const int64_t HEURISTIC_ZERO_DURABILITY_BONUS = -2281337;
const int64_t HASH_MAP_SIZE = 10007;

struct Coord {
    int64_t x;
    int64_t y;

    bool operator==(const Coord& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const Coord& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct Candidate {
    int64_t x;
    int64_t y;
    int64_t durability;
    int64_t dist_to_base;
};

class VisitedSet {
private:
    std::vector<std::vector<Coord>> buckets;

    int64_t hashFunc(int64_t x, int64_t y) const {
        long long h = ((long long)x * 73856093LL) ^ ((long long)y * 19349663LL);
        return (int64_t)(std::abs(h) % HASH_MAP_SIZE);
    }

public:
    VisitedSet() : buckets(HASH_MAP_SIZE) {}

    bool contains(int64_t x, int64_t y) const {
        int64_t h = hashFunc(x, y);
        const std::vector<Coord>& bucket = buckets[h];
        for (std::size_t i = 0; i < bucket.size(); ++i) {
            if (bucket[i].x == x && bucket[i].y == y) {
                return true;
            }
        }
        return false;
    }

    void insert(int64_t x, int64_t y) {
        if (!contains(x, y)) {
            int64_t h = hashFunc(x, y);
            Coord c = {x, y};
            buckets[h].push_back(c);
        }
    }
};

int64_t calcManhattanDistance(int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

int64_t calculateScore(int64_t durability, int64_t distance) {
    if (durability == 0) {
        return HEURISTIC_ZERO_DURABILITY_BONUS;
    }
    return (durability * HEURISTIC_WEIGHT_DURABILITY) + (distance * HEURISTIC_WEIGHT_DISTANCE);
}

int main() {

    int64_t baseX, baseY, pickaxeDurability;
    if (!(std::cin >> baseX >> baseY >> pickaxeDurability)) {
        return EXIT_SUCCESS;
    }

    if (baseX == 0 && baseY == 0) {
        return EXIT_SUCCESS;
    }

    VisitedSet visited;
    std::vector<Candidate> frontier;

    visited.insert(0, 0);

    int64_t dx_start[4] = {0, 0, 1, -1};
    int64_t dy_start[4] = {1, -1, 0, 0};

    for (int64_t i = 0; i < 4; ++i) {
        int64_t dur;
        if (!(std::cin >> dur)) {
            return 0;
        }
        int64_t nx = 0 + dx_start[i];
        int64_t ny = 0 + dy_start[i];
        
        if (!visited.contains(nx, ny)) {
            Candidate c;
            c.x = nx;
            c.y = ny;
            c.durability = dur;
            c.dist_to_base = calcManhattanDistance(nx, ny, baseX, baseY);
            frontier.push_back(c);
            visited.insert(nx, ny);
        }
    }

    while (!frontier.empty()) {
        int64_t bestIndex = -1;
        int64_t bestScore = std::numeric_limits<int64_t>::max();

        for (std::size_t i = 0; i < frontier.size(); ++i) {
            if (frontier[i].durability > pickaxeDurability) {
                continue;
            }

            int64_t score = calculateScore(frontier[i].durability, frontier[i].dist_to_base);

            if (frontier[i].durability == 0) {
                bestIndex = (int64_t)i;
                bestScore = score;
                break;
            }

            if (score < bestScore) {
                bestScore = score;
                bestIndex = (int64_t)i;
            }
        }

        if (bestIndex == -1) {
            return 228666;
        }

        Candidate chosen = frontier[bestIndex];
        
        frontier[bestIndex] = frontier[frontier.size() - 1];
        frontier.pop_back();

        std::cout << chosen.x << " " << chosen.y << std::endl;

        if (chosen.x == baseX && chosen.y == baseY) {
            return EXIT_SUCCESS;
        }

        pickaxeDurability -= chosen.durability;

        int64_t dx_neighbors[4] = {0, 0, 1, -1};
        int64_t dy_neighbors[4] = {1, -1, 0, 0};

        for (int64_t i = 0; i < 4; ++i) {
            int64_t dur;
            if (!(std::cin >> dur)) {
                return EXIT_SUCCESS;
            }
            
            int64_t nx = chosen.x + dx_neighbors[i];
            int64_t ny = chosen.y + dy_neighbors[i];

            if (nx == baseX && ny == baseY) {
                std::cout << nx << " " << ny << std::endl;
                return EXIT_SUCCESS;
            }

            if (!visited.contains(nx, ny)) {
                Candidate c;
                c.x = nx;
                c.y = ny;
                c.durability = dur;
                c.dist_to_base = calcManhattanDistance(nx, ny, baseX, baseY);
                frontier.push_back(c);
                visited.insert(nx, ny);
            }
        }
    }

    return 0;
}