#include <iostream>
#include <string>
#include <vector>
#include <map>

void print_subsets(const std::vector<char>& chars, const std::vector<size_t>& counts, 
                   size_t index, std::string& cur_str);

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string str = {};
    std::cin >> str;
    
    std::map<char, size_t> freq_map = {};
    for (char chr : str) {
        ++freq_map[chr];
    }
    
    std::vector<char> chars = {};
    std::vector<size_t> counts = {};
    
    for (const auto& pair : freq_map) {
         chars.push_back(pair.first);
        counts.push_back(pair.second);
    }
    
    std::string cur_str = {};
    print_subsets(chars, counts, 0, cur_str);
    
    return 0;
}

void print_subsets(const std::vector<char>& chars, const std::vector<size_t>& counts, 
                   size_t index, std::string& cur_str) {
    if (index == chars.size()) {
        std::cout << cur_str << "\n";
        return;
    }
    
    std::string original = cur_str;
    
    for (size_t k = 0; k <= counts[index]; ++k) {
        cur_str = original;
        for (size_t i = 0; i < k; ++i) {
            cur_str += chars[index];
        }
        print_subsets(chars, counts, index + 1, cur_str);
    }
    
    cur_str = original;
}