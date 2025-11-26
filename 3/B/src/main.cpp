#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

struct PlayerInfo {
    int bestScore = -1;
    size_t time = 0;
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t M = 0, N = 0;
    std::cin >> M >> N;

    std::unordered_map<std::string, PlayerInfo> mp = {};

    std::string name = {};
    int score = 0;

    for (size_t t = 0; t < N; ++t) {
        std::cin >> name >> score;

        auto &info = mp[name];

        if (score > info.bestScore) {
            info.bestScore = score;
            info.time = t;
        }
    }

    std::vector<std::pair<std::string, PlayerInfo>> players;
    players.reserve(mp.size());

    for (auto &p : mp) {
        if (p.second.bestScore >= 0) {
            players.emplace_back(p.first, p.second);
        }
    }

    std::sort(
        players.begin(), 
        players.end(),
        [](const auto& a, const auto& b) {
            if (a.second.bestScore != b.second.bestScore)
                return a.second.bestScore > b.second.bestScore;
            return a.second.time < b.second.time;
        }
    );

    for (auto &p : players) {
        std::cout << p.first << '\n';
    }

    return 0;
}
