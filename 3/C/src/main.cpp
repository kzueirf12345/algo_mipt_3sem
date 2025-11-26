#include <iostream>
#include <vector>
#include <unordered_map>

struct Point {
    int64_t x, y, z;
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct PointHash {
    std::size_t operator()(const Point& p) const {
        std::size_t h1 = std::hash<int64_t>{}(p.x);
        std::size_t h2 = std::hash<int64_t>{}(p.y);
        std::size_t h3 = std::hash<int64_t>{}(p.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    size_t n;
    std::cin >> n;

    if (n % 2 == 1) {
        std::cout << "No" << std::endl;
        return 0;
    }
    
    std::vector<Point> points(n);
    std::unordered_map<Point, size_t, PointHash> count;


    size_t secondPointInd = 0;
    
    for (size_t i = 0; i < n; i++) {
        std::cin >> points[i].x >> points[i].y >> points[i].z;
        points[i].x *= 2;
        points[i].y *= 2;
        points[i].z *= 2;
        count[points[i]]++;

        if (i != 0 && secondPointInd == 0 && !(points[i] == points[0])) {
            secondPointInd = i;
        }
    }
    
    if (secondPointInd == 0) {
        std::cout << "Yes" << std::endl;
        return 0;

    }
    
    Point center;
    center.x = (points[0].x + points[secondPointInd].x) / 2;
    center.y = (points[0].y + points[secondPointInd].y) / 2;
    center.z = (points[0].z + points[secondPointInd].z) / 2;
    
    for (size_t i = 0; i < points.size(); i++) {
        if (count[points[i]] == 0) continue;
        
        Point sym;
        sym.x = 2 * center.x - points[i].x;
        sym.y = 2 * center.y - points[i].y;
        sym.z = 2 * center.z - points[i].z;
        
        auto it = count.find(sym);
        if (it == count.end() || it->second == 0) {
            std::cout << "No" << std::endl;
            return 0;
        }
        
        count[points[i]]--;
        count[sym]--;
    }
    
    std::cout << "Yes" << std::endl;
    return 0;
}