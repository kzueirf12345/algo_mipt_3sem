#include <iostream>
#include <vector>
#include <algorithm>

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
    
    Point operator+(const Point& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

int main() {
    size_t N;
    std::cin >> N;
    
    std::vector<Point> points(N);
    for (size_t i = 0; i < N; ++i) {
        std::cin >> points[i].x >> points[i].y >> points[i].z;
    }
    
    std::sort(points.begin(), points.end());
    
    Point center_double = points[0] + points[N - 1];
    
    for (size_t i = 0; i < N; ++i) {
        size_t j = N - 1 - i;
        if (points[i] + points[j] != center_double) {
            std::cout << "No\n";
            return 0;
        }
    }
    
    std::cout << "Yes\n";
    return 0;
}