#include <iostream>
#include <random>
#include <chrono>
#include <vector>

struct Node {
    int value;
    int64_t sum;
    int64_t prio;
    size_t size;
    Node* left;
    Node* right;
    bool assigned;
    int assign;
    int add;

    Node(int v, int64_t p)
        : value(v), sum(v), prio(p), size(1),
          left(nullptr), right(nullptr),
          assigned(false), assign(0), add(0)
    {}
};

class ImplicitTreap {
private:
    Node* root;
    std::mt19937_64 rng;

    size_t Size(Node* t) {
        return t ? t->size : 0;
    }

    long long Sum(Node* t) {
        return t ? t->sum : 0;
    }

    void Update(Node* t) {
        if (!t) return;
        t->size = 1 + Size(t->left) + Size(t->right);
        t->sum = t->value + Sum(t->left) + Sum(t->right);
    }

    void ApplyAssign(Node* t, int x) {
        if (!t) return;
        t->assigned = true;
        t->assign = x;
        t->add = 0;
        t->value = x;
        t->sum = (int64_t)x * (int64_t)t->size;
    }

    void ApplyAdd(Node* t, int x) {
        if (!t) return;
        if (t->assigned) {
            t->assign += x;
            t->value = t->assign;
            t->sum = (int64_t)t->assign * (int64_t)t->size;
        } else {
            t->add += x;
            t->value += x;
            t->sum += (int64_t)x * (int64_t)t->size;
        }
    }


    void Push(Node* t) {
        if (!t) return;
        if (t->add != 0) {
            if (t->left) ApplyAdd(t->left, t->add);
            if (t->right) ApplyAdd(t->right, t->add);
            t->add = 0;
        }
        if (t->assigned) {
            if (t->left) ApplyAssign(t->left, t->assign);
            if (t->right) ApplyAssign(t->right, t->assign);
            t->assigned = false;
        }
    }


    void Split(Node* t, size_t k, Node*& l, Node*& r) {
        if (!t) {
            l = r = nullptr;
            return;
        }
        Push(t);
        size_t leftSize = Size(t->left);
        if (k <= leftSize) {
            Split(t->left, k, l, t->left);
            r = t;
            Update(r);
        } else {
            Split(t->right, k - leftSize - 1, t->right, r);
            l = t;
            Update(l);
        }
    }

    Node* Merge(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        if (a->prio < b->prio) {
            Push(a);
            a->right = Merge(a->right, b);
            Update(a);
            return a;
        } else {
            Push(b);
            b->left = Merge(a, b->left);
            Update(b);
            return b;
        }
    }

    Node* NewNode(int v) {
        return new Node(v, static_cast<int64_t>(rng()));
    }

    void Dtor(Node* t) {
        if (!t) return;
        Dtor(t->left);
        Dtor(t->right);
        delete t;
    }

    void Collect(Node* t, std::vector<int>& out) {
        if (!t) return;
        Push(t);
        Collect(t->left, out);
        out.push_back(t->value);
        Collect(t->right, out);
    }

public:

    ImplicitTreap()
        : root(nullptr),
          rng((uint64_t)std::chrono::steady_clock::now().time_since_epoch().count())
    {}

    ~ImplicitTreap() {
        Dtor(root);
    }

    void BuildFromVector(const std::vector<int>& a) {
        root = nullptr;
        for (size_t i = 0; i < a.size(); ++i) {
            Node* node = NewNode(a[i]);
            root = Merge(root, node);
        }
    }

    void InsertAt(size_t pos, int x) {
        Node* item = NewNode(x);
        Node* l = nullptr;
        Node* r = nullptr;
        Split(root, pos, l, r);
        root = Merge(Merge(l, item), r);
    }

    void EraseAt(size_t pos) {
        Node* l = nullptr;
        Node* m = nullptr;
        Node* r = nullptr;
        Split(root, pos, l, m);
        Split(m, 1, m, r);
        if (m) delete m;
        root = Merge(l, r);
    }

    long long RangeSum(size_t lpos, size_t rpos) {
        Node* l = nullptr;
        Node* m = nullptr;
        Node* r = nullptr;
        Split(root, lpos, l, m);
        Split(m, rpos - lpos + 1, m, r);
        long long ans = Sum(m);
        root = Merge(Merge(l, m), r);
        return ans;
    }

    void RangeAssign(int x, size_t lpos, size_t rpos) {
        Node* l = nullptr;
        Node* m = nullptr;
        Node* r = nullptr;
        Split(root, lpos, l, m);
        Split(m, rpos - lpos + 1, m, r);
        if (m) ApplyAssign(m, x);
        root = Merge(Merge(l, m), r);
    }

    void RangeAdd(int x, size_t lpos, size_t rpos) {
        Node* l = nullptr;
        Node* m = nullptr;
        Node* r = nullptr;
        Split(root, lpos, l, m);
        Split(m, rpos - lpos + 1, m, r);
        if (m) ApplyAdd(m, x);
        root = Merge(Merge(l, m), r);
    }

    std::vector<int> ToVector() {
        std::vector<int> out;
        out.reserve(Size(root));
        Collect(root, out);
        return out;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t N = 0;
    std::cin >> N;

    std::vector<int> a;
    a.reserve(N);
    for (size_t i = 0; i < N; ++i) {
        int v;
        std::cin >> v;
        a.push_back(v);
    }

    ImplicitTreap treap;
    treap.BuildFromVector(a);

    size_t type;
    while (std::cin >> type) {
        if (type == 1) {
            size_t L, R;
            std::cin >> L >> R;
            long long res = treap.RangeSum(L, R);
            std::cout << res << "\n";
        } else if (type == 2) {
            int X;
            size_t pos;
            std::cin >> X >> pos;
            treap.InsertAt(pos, X);
        } else if (type == 3) {
            size_t pos;
            std::cin >> pos;
            treap.EraseAt(pos);
        } else if (type == 4) {
            int X;
            size_t L, R;
            std::cin >> X >> L >> R;
            treap.RangeAssign(X, L, R);
        } else if (type == 5) {
            int X;
            size_t L, R;
            std::cin >> X >> L >> R;
            treap.RangeAdd(X, L, R);
        } else {
            break;
        }
    }

    std::vector<int> res = treap.ToVector();
    for (size_t i = 0; i < res.size(); ++i) {
        if (i) std::cout << " ";
        std::cout << res[i];
    }
    std::cout << "\n";

    return 0;
}
