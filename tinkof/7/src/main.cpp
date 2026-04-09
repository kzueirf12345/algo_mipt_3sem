#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using ll = long long;

struct Edge {
    ll u, v;
    ll w;
};

bool compareEdges(const Edge& a, const Edge& b) {
    return a.w < b.w;
}

struct DSU {
    std::vector<ll> parent;

    DSU(ll n) {
        parent.resize(n + 1);
        iota(parent.begin(), parent.end(), 0ll);
    }

    ll find(ll i) {
        if (parent[i] == i) {
            return i;
        }

        return parent[i] = find(parent[i]);
    }

    bool unite(ll i, ll j) {
        ll root_i = find(i);
        ll root_j = find(j);

        if (root_i != root_j) {
            parent[root_i] = root_j;
            return true;
        }

        return false;
    }
};

ll get_mst(ll total_nodes, const std::vector<Edge>& sorted_edges) {
    DSU dsu(total_nodes);
    ll total_cost = 0;
    ll edges_added = 0;

    for (const auto& e : sorted_edges) {
        if (dsu.unite(e.u, e.v)) {
            total_cost += e.w;
            edges_added++;
        }
    }

    if (edges_added == total_nodes - 1) {
        return total_cost;
    }

    return -1;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    ll n, m;
    std::cin >> n >> m;

    std::vector<ll> a(n + 1), b(n + 1);

    for (size_t i = 1; i <= n; i++) {
        std::cin >> a[i];
    }

    for (size_t i = 1; i <= n; i++) {
        std::cin >> b[i];
    }

    std::vector<Edge> road_edges(m);
    for (size_t i = 0; i < m; i++) {
        std::cin >> road_edges[i].u >> road_edges[i].v >> road_edges[i].w;
    }

    std::vector<Edge> airport_edges(n), port_edges(n);
    for (size_t i = 1; i <= n; i++) {
        airport_edges[i - 1] = {(ll)i, n + 1, a[i]};
        port_edges[i - 1] = {(ll)i, n + 2, b[i]};
    }

    ll ans = -1;

    std::vector<Edge> e1 = road_edges;
    std::sort(e1.begin(), e1.end(), compareEdges);

    ll res1 = get_mst(n, e1);

    if (res1 != -1) {
        ans = res1;
    }

    std::vector<Edge> e2 = road_edges;
    e2.insert(e2.end(), airport_edges.begin(), airport_edges.end());

    std::sort(e2.begin(), e2.end(), compareEdges);

    ll res2 = get_mst(n + 1, e2);

    if (res2 != -1) {
        ans = (ans == -1) ? res2 : std::min(ans, res2);
    }

    std::vector<Edge> e3 = road_edges;

    for (size_t i = 1; i <= n; i++) {
        e3.push_back({(ll)i, n + 1, b[i]});
    }

    std::sort(e3.begin(), e3.end(), compareEdges);

    ll res3 = get_mst(n + 1, e3);

    if (res3 != -1) {
        ans = (ans == -1) ? res3 : std::min(ans, res3);
    }

    std::vector<Edge> e4 = road_edges;

    e4.insert(e4.end(), airport_edges.begin(), airport_edges.end());
    e4.insert(e4.end(), port_edges.begin(), port_edges.end());

    std::sort(e4.begin(), e4.end(), compareEdges);

    ll res4 = get_mst(n + 2, e4);

    if (res4 != -1) {
        ans = (ans == -1) ? res4 : std::min(ans, res4);
    }

    std::cout << ans << std::endl;

    return 0;
}