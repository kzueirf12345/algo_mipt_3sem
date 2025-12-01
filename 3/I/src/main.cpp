#include <iostream>
#include <unordered_map>
#include <vector>
// #include <unordered_set>
#include <set>

int main() {
    size_t N = 0, M = 0;
    std::cin >> N >> M;

    std::vector<std::set<long long>> sets(N + 1);
    std::unordered_map<long long, std::set<size_t>> belongs;

    for (size_t i = 0; i < M; ++i) {
        std::string cmd;
        std::cin >> cmd;

        if (cmd == "+") {
            long long x = 0; 
            size_t s = 0;
            std::cin >> x >> s;

            if (sets[s].insert(x).second) {
                belongs[x].insert(s);
            }
        } else if (cmd == "-") {
            long long x = 0; 
            size_t s = 0;
            std::cin >> x >> s;

            if (sets[s].erase(x)) {
                belongs[x].erase(s);
                if (belongs[x].empty()) {
                    belongs.erase(x);
                }
            }
        } else if (cmd == "=") {
            size_t s = 0;
            std::cin >> s;

            for (long long x : sets[s]) {
                belongs[x].erase(s);
                if (belongs[x].empty()) {
                    belongs.erase(x);
                }
            }
            sets[s].clear();

        } else if (cmd == "?") {
            size_t s = 0;
            std::cin >> s;

            if (sets[s].empty()) {
                std::cout << -1 << "\n";
            } else {
                for (long long x : sets[s]) {
                    std::cout << x << " ";
                }
                std::cout << "\n";
            }
        } else if (cmd == "??") {
            long long x = 0;
            std::cin >> x;

            if (belongs.count(x) == 0 || belongs[x].empty()) {
                std::cout << -1 << "\n";
            } else {
                for (size_t s : belongs[x]) {
                    std::cout << s << " ";
                }
                std::cout << "\n";
            }
        }
    }
}
