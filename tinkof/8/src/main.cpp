#include <iostream>
#include <vector>
#include <algorithm>

using ll = long long;

struct Node {
    ll sum;
    int count;
};

class SegmentTree {

public:

    size_t n;
    std::vector<Node> tree;
    std::vector<ll> leaf_costs;

    SegmentTree(size_t size) {
        n = 1;
        while (n < size) {
            n *= 2;
        }

        tree.assign(2 * n, {0, 0});

        leaf_costs.assign(n, 0);
    }

    void update(size_t pos, ll cost, int delta) {
        leaf_costs[pos] = cost;

        size_t idx = pos + n;

        tree[idx].sum += (ll)delta * cost;
        tree[idx].count += delta;

        while (idx > 1) {
            idx /= 2;
            tree[idx].sum = tree[2 * idx].sum + tree[2 * idx + 1].sum;
            tree[idx].count = tree[2 * idx].count + tree[2 * idx + 1].count;
        }
    }

    ll get_sum(int k) {
        if (k <= 0) {
            return 0;
        }

        if (k >= tree[1].count) {
            return tree[1].sum;
        }

        size_t idx = 1;
        ll res = 0;

        while (idx < n) {
            if (k <= tree[2 * idx].count) {
                idx = 2 * idx;
            } 
            else {
                res += tree[2 * idx].sum;
                k -= tree[2 * idx].count;
                idx = 2 * idx + 1;
            }
        }

        if (tree[idx].count > 0 && k > 0) {
            res += leaf_costs[idx - n];
        }

        return res;
    }

    ll get_cost(int k) {
        if (k <= 0 || k > tree[1].count) {
            return 0;
        }

        size_t idx = 1;

        while (idx < n) {
            if (k <= tree[2 * idx].count) {
                idx = 2 * idx;
            } 
            else {
                k -= tree[2 * idx].count;
                idx = 2 * idx + 1;
            }
        }

        return leaf_costs[idx - n];
    }

    int total_active() const {
        return tree[1].count;
    }

};

struct Query {
    char type;
    int w;
    ll c;
    int x;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t q;
    std::cin >> q;

    std::vector<Query> queries(q);
    std::vector<std::pair<ll, int>> items1, items2;

    for (size_t i = 0; i < q; ++i) {
        std::cin >> queries[i].type;
        if (queries[i].type == '+') {
            std::cin >> queries[i].w >> queries[i].c;

            if (queries[i].w == 1) {
                items1.push_back({queries[i].c, i});
            }
            else {
                items2.push_back({queries[i].c, i});
            }
        } 
        else {
            std::cin >> queries[i].x;
        }
    }

    auto cmp = [](const std::pair<ll, int>& a, const std::pair<ll, int>& b) {
        if (a.first != b.first) {
            return a.first > b.first;
        }

        return a.second < b.second;
    };

    std::sort(items1.begin(), items1.end(), cmp);
    std::sort(items2.begin(), items2.end(), cmp);


    std::vector<int> pos_in_tree(q, -1);

    for (size_t i = 0; i < items1.size(); ++i) pos_in_tree[items1[i].second] = (int)i;
    for (size_t i = 0; i < items2.size(); ++i) pos_in_tree[items2[i].second] = (int)i;

    SegmentTree st1(items1.size()), st2(items2.size());

    for (size_t i = 0; i < q; ++i) {

        if (queries[i].type == '+') {
            if (queries[i].w == 1) {
                st1.update(pos_in_tree[i], queries[i].c, 1);
            }
            else {
                st2.update(pos_in_tree[i], queries[i].c, 1);
            }
        } 
        else {
            int x = queries[i].x;
            ll ans = 0;

            for (int p = 0; p <= 1; ++p) {
                int max_k = std::min(x, st1.total_active());

                if (max_k < p) {
                    continue;
                }

                int best_k = p;
                int low = 0;
                int high = (max_k - p) / 2 - 1;

                while (low <= high) {
                    int mid = low + (high - low) / 2;
                    int k = p + 2 * mid;

                    ll gain1 = st1.get_cost(k + 1) + st1.get_cost(k + 2);

                    int remaining_w2 = (x - k) / 2;

                    ll loss2 = st2.get_cost(remaining_w2);

                    if (gain1 >= loss2) {
                        best_k = k + 2;
                        low = mid + 1;
                    } 
                    else {
                        high = mid - 1;
                    }
                }

                ll current_cost = st1.get_sum(best_k) + st2.get_sum((x - best_k) / 2);

                if (current_cost > ans) {
                    ans = current_cost;
                }
            }

            std::cout << ans << std::endl;
        }

    }

    return 0;
}