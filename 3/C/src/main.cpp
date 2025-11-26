#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

struct Point {
    long long x, y, z;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

struct PointHash {
    std::size_t operator()(const Point& p) const {
        std::size_t h1 = std::hash<long long>{}(p.x);
        std::size_t h2 = std::hash<long long>{}(p.y);
        std::size_t h3 = std::hash<long long>{}(p.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    int N;
    std::cin >> N;
    
    std::vector<Point> points(N);
    std::unordered_map<Point, int, PointHash> count;
    
    for (int i = 0; i < N; i++) {
        std::cin >> points[i].x >> points[i].y >> points[i].z;
        points[i].x *= 2;
        points[i].y *= 2;
        points[i].z *= 2;
        count[points[i]]++;
    }
    
    std::sort(points.begin(), points.end());
    
    Point center;
    center.x = (points[0].x + points[N-1].x) / 2;
    center.y = (points[0].y + points[N-1].y) / 2;
    center.z = (points[0].z + points[N-1].z) / 2;
    
    for (size_t i = 0; i < points.size(); i++) {
        if (count[points[i]] == 0) continue;
        
        Point sym;
        sym.x = 2 * center.x - points[i].x;
        sym.y = 2 * center.y - points[i].y;
        sym.z = 2 * center.z - points[i].z;
        
        auto it = count.find(sym);
        if (it == count.end() || it->second == 0) {
            std::cout << "No\n";
            return 0;
        }
        
        count[points[i]]--;
        count[sym]--;
    }
    
    std::cout << "Yes\n";
    return 0;
}