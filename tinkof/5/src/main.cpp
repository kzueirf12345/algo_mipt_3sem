#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <vector>

using ull = unsigned long long;
using ll = long long;

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ull n, m;

    std::cin >> n >> m;

    std::vector<ull> cost(m);
    for (int i = 0; i < m; i++) {
        std::cin >> cost[i];
    }
    
    int total_nodes = n + m;
    std::vector<std::vector<std::pair<int, ull>>> adj(total_nodes + 1);

    for (int j = 0; j < m; ++j) {
        int k;
        std::cin >> k;
        int airline_node = n + 1 + j;
        
        for (int i = 0; i < k; ++i) {
            int city;
            std::cin >> city;
            adj[city].push_back({airline_node, 0});
            adj[airline_node].push_back({city, cost[j]});
        }
    }

    std::vector<ll> dist(total_nodes + 1, std::numeric_limits<ll>::max());
    dist[1] = 0;

    std::priority_queue<std::pair<long long, int>, 
                        std::vector<std::pair<long long, int>>, 
                        std::greater<std::pair<long long, int>>> pq;
    pq.push({0, 1});

    while (!pq.empty()) {
        ll d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;

        for (const auto& edge : adj[u]) {
            int v = edge.first;
            long long w = edge.second;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    
    for (size_t i = 1; i <= n; i++) {
        std::cout << dist[i] << " ";
    }
    std::cout << std::endl;
    
    
    return 0;
}