#include <cassert>
#include <cstdlib>
#include <random>
#include <chrono>
#include <cstdio>
#include <cstring>

static inline constexpr const size_t NAME_SIZE_MAX = 31;

struct Node {
    char name[NAME_SIZE_MAX];
    int64_t salary;
    uint64_t prio;
    size_t size;
    Node* left;
    Node* right;

    Node(const char* name_, int64_t salary_, uint64_t prio_) 
        : 
            salary(salary_), 
            prio(prio_), 
            size(1), 
            left(nullptr), 
            right(nullptr) 
    {
        std::strcpy(name, name_);
    }
};

size_t Size(Node* t);
void UpdateSize(Node* t);
void Split(Node* t, size_t k, Node*& l, Node*& r);
Node* Merge(Node* a, Node* b);
Node* NewNode(const char* name, int64_t salary);
Node* InsertAt(Node* root, size_t pos, Node* item);
Node* EraseAt(Node* root, size_t pos);
Node* Kth(Node* root, size_t k);
void Dtor(Node* root);

int main() {
    size_t N = 0, M = 0;
    char name[31] = {};
    int64_t sal = {};
    
    if (scanf("%zu %zu", &N, &M) != 2) {
        return EXIT_FAILURE;
    }

    Node* root = nullptr;

    for (size_t i = 0; i < N; ++i) {
        if (scanf("%30s %ld", name, &sal) != 2) {
            return EXIT_FAILURE;
        }
        Node* node = NewNode(name, sal);
        root = Merge(root, node);
    }

    for (size_t i = 0; i < M; ++i) {
        size_t type = 0;
        size_t pos = 0;
        
        if (scanf("%zu", &type) != 1) {
            return EXIT_FAILURE;
        }
        
        if (type == 0) {
            if (scanf("%zu %30s %ld", &pos, name, &sal) != 3) {
                return EXIT_FAILURE;
            }
            Node* node = NewNode(name, sal);
            root = InsertAt(root, pos, node);
        } else if (type == 1) {
            if (scanf("%zu", &pos) != 1) {
                return EXIT_FAILURE;
            }
            root = EraseAt(root, pos);
        } else if (type == 2) {
            if (scanf("%zu", &pos) != 1) {
                return EXIT_FAILURE;
            }
            Node* res = Kth(root, pos);
            if (res) {
                printf("%s %ld\n", res->name, res->salary);
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

Node* NewNode(const char* name, int64_t salary) {
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