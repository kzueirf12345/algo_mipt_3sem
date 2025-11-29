#include <iostream>
#include <unordered_set>
#include <cstdint>

struct Node {
    Node* next[2];
    ssize_t cnt;

    Node() 
        : cnt(0) 
    { 
        next[0] = next[1] = nullptr; 
    }
};

void InsertTrie(uint32_t x);
void EraseTrie(uint32_t x);
uint32_t QueryTrie(uint32_t x);

static Node* root = new Node();

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::unordered_set<uint32_t> present = {};

    size_t n = 0;
    std::cin >> n;

    for (size_t i = 0; i < n; ++i) {
        size_t type = 0;
        uint32_t val = 0;
        std::cin >> type >> val;

        if (type == 1) 
        {
            if (present.insert(val).second) 
            {
                InsertTrie(val);
            }
        } 
        else if (type == 2) 
        {
            if (present.erase(val)) 
            {
                EraseTrie(val);
            }
        } 
        else 
        {
            if (present.empty()) 
            {
                std::cout << 0 << '\n';
            } 
            else 
            {
                size_t ans = QueryTrie(val);
                std::cout << ans << '\n';
            }
        }
    }

    return 0;
}

void InsertTrie(uint32_t x)
{
    Node* cur = root;
    for (ssize_t i = 31; i >= 0; --i) 
    {
        size_t b = (x >> i) & 1u;
        if (!cur->next[b]) 
        {
            cur->next[b] = new Node();
        }
        cur = cur->next[b];
        ++cur->cnt;
    }
}

void EraseTrie(uint32_t x)
{
    Node* cur = root;
    for (ssize_t i = 31; i >= 0; --i) 
    {
        size_t b = (x >> i) & 1u;
        Node* nxt = cur->next[b];
        if (!nxt) 
        {
            return;
        }
        --nxt->cnt;
        cur = nxt;
    }
}

uint32_t QueryTrie(uint32_t x)
{
    Node* cur = root;
    uint32_t res = 0;

    for (ssize_t i = 31; i >= 0; --i) 
    {
        size_t b = (x >> i) & 1u;
        size_t want = b ^ 1;
        if (cur->next[want] && cur->next[want]->cnt > 0) 
        {
            res |= (1u << i);
            cur = cur->next[want];
        } 
        else 
        {
            cur = cur->next[b];
        }
    }
    return res ^ x;
}
