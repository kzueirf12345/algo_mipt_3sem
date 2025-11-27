#include <cassert>
#include <iostream>
#include <string>
#include <random>
#include <chrono>

struct Node {
    std::string name;
    int64_t salary;
    uint64_t prio;
    size_t size;
    Node* left;
    Node* right;

    Node(const std::string& name_, int64_t salary_, uint64_t prio_) 
        :   name(name_), 
            salary(salary_), 
            prio(prio_), 
            size(1), 
            left(nullptr), 
            right(nullptr) 
    {}
};

size_t Size(Node* t);
void UpdateSize(Node* t);
void Split(Node* t, size_t k, Node*& l, Node*& r);
Node* Merge(Node* a, Node* b);
Node* NewNode(const std::string& name, int64_t salary);
Node* InsertAt(Node* root, size_t pos, Node* item);
Node* EraseAt(Node* root, size_t pos);
Node* Kth(Node* root, size_t k);
void  Dtor(Node* root);

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t N = 0, M = 0;
    std::cin >> N >> M;

    Node* root = nullptr;

    for (size_t i = 0; i < N; ++i) {
        std::string name;
        int64_t sal;
        std::cin >> name >> sal;
        Node* node = NewNode(name, sal);
        root = Merge(root, node);
    }

    for (size_t i = 0; i < M; ++i) {
        size_t type = 0;
        std::cin >> type;
        if (type == 0) {
            size_t pos = 0;
            std::string name = {};
            int64_t sal = 0;
            std::cin >> pos >> name >> sal;

            Node* node = NewNode(name, sal);
            root = InsertAt(root, pos, node);
        } else if (type == 1) {
            size_t pos = 0;
            std::cin >> pos;

            root = EraseAt(root, pos);
        } else if (type == 2) {
            size_t pos = 0;
            std::cin >> pos;

            Node* res = Kth(root, pos);
            if (res) {
                std::cout << res->name << " " << res->salary << "\n";
            }
        }
    }

    Dtor(root);

    return 0;
}

static std::mt19937 rng((uint64_t)std::chrono::steady_clock::now().time_since_epoch().count());

size_t Size(Node* t) {
    return t ? t->size : 0;
}

void UpdateSize(Node* t) {
    if (t) {
        t->size = 1 + Size(t->left) + Size(t->right);
    }
}

void Split(Node* t, size_t k, Node*& l, Node*& r) {
    if (!t) {
        l = r = nullptr;
        return;
    }
    size_t leftSize = Size(t->left);
    if (k <= leftSize) {
        Split(t->left, k, l, t->left);
        r = t;
        UpdateSize(r);
    } else {
        Split(t->right, k - leftSize - 1, t->right, r);
        l = t;
        UpdateSize(l);
    }
}

Node* Merge(Node* a, Node* b) {
    if (!a) return b;
    if (!b) return a;

    if (a->prio < b->prio) {
        a->right = Merge(a->right, b);
        UpdateSize(a);
        return a;
    } else {
        b->left = Merge(a, b->left);
        UpdateSize(b);
        return b;
    }
}

Node* NewNode(const std::string& name, int64_t salary) {
    uint64_t p = rng();
    return new Node(name, salary, p);
}

Node* InsertAt(Node* root, size_t pos, Node* item) {
    Node* left = nullptr;
    Node* right = nullptr;
    Split(root, pos - 1, left, right);
    Node* merged = Merge(left, item);
    return Merge(merged, right);
}

Node* EraseAt(Node* root, size_t pos) {
    Node* left = nullptr;
    Node* erased = nullptr;
    Node* right = nullptr;

    Split(root, pos - 1, left, erased);
    Split(erased, 1, erased, right);

    if (erased) {
        delete erased;
    }

    return Merge(left, right);
}

Node* Kth(Node* root, size_t k) {
    Node* cur = root;
    while (cur) {
        size_t leftSize = Size(cur->left);

        if (k == leftSize + 1) {
            return cur;
        }

        if (k <= leftSize) {
            cur = cur->left;
        } else {
            k -= leftSize + 1;
            cur = cur->right;
        }
    }
    return nullptr;
}

void Dtor(Node* root) {
    if (!root) {
        return;
    }

    Dtor(root->left);
    Dtor(root->right);
    delete root;
}