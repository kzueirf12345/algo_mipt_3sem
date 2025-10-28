#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct LetterFrequency
{
    char letter;
    int count;
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string inputStr;
    std::cin >> inputStr;

    std::vector<int> letterCounts(26, 0);

    for (char ch : inputStr) {
        letterCounts[ch - 'A']++;
    }

    std::vector<LetterFrequency> frequencies;
    for (int i = 0; i < 26; ++i) {
        if (letterCounts[i] > 0)
        {
            frequencies.push_back({static_cast<char>('A' + i), letterCounts[i]});
        }
    }

    std::sort(
        frequencies.begin(), 
        frequencies.end(), 
        [](const LetterFrequency& a, const LetterFrequency& b) {
            if (a.count == b.count)
                return a.letter < b.letter;
            return a.count > b.count;
        }
    );

    for (const auto& lf : frequencies) {
        std::cout << lf.letter << " " << lf.count << "\n";
    }

    return 0;
}
