#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>
#include <cstdint>
#include <clocale>

static const uint64_t MAX_NUM_VAL = static_cast<uint64_t>(-1);

enum NumberScale {
    SCALE_DIGIT, 
    SCALE_TEN, 
    SCALE_HUNDRED,
    SCALE_THOUSAND, 
    SCALE_MILLION, 
    SCALE_BILLION
};

enum GrammarCase {
    CASE_NOM = 1 << 0, 
    CASE_GEN = 1 << 1, 
    CASE_DAT = 1 << 2,
    CASE_ACC = 1 << 3, 
    CASE_INS = 1 << 4, 
    CASE_PRE = 1 << 5
};

struct DictionaryEntry {
    std::wstring text;
    uint64_t numeric_value;
    int32_t case_flags;
};

struct SuffixEntry {
    std::wstring text;
    int32_t case_flags;
};

static int get_char_index(wchar_t c) {
    if (c >= L'А' && c <= L'Я') {
        c += (L'а' - L'А');
    }
    else if (c == L'Ё') {
        c = L'ё';
    }
    else {
        c = static_cast<wchar_t>(towlower(static_cast<wint_t>(c)));
    }
    
    if (c == L'ё' || c == L'е') {
        return L'е' - L'а';
    }
    if (c >= L'а' && c <= L'я') {
        return c - L'а';
    }
    return -1;
}

struct StemTrieNode {
    uint64_t mapped_value = MAX_NUM_VAL;
    bool requires_suffix = false;
    int32_t allowed_cases = 0;
    int stem_length = 0;
    int sub_nodes[33];

    StemTrieNode() { 
        for (int & n : sub_nodes) n = -1; 
    }
};

struct SuffixTrieNode {
    int length = 0;
    int32_t case_flags = 0;
    int sub_nodes[33];

    SuffixTrieNode() { 
        for (int & n : sub_nodes) n = -1; 
    }
};

static void InsertStem(
    std::vector<StemTrieNode>& stem_arena, size_t node_idx, const std::wstring& text, size_t pos, uint64_t val, bool req_suf, int32_t grammar_cases
) {
    if (text.length() == pos) {
        return;
    }

    int char_index = get_char_index(text[pos]);
    if (char_index == -1) {
        return;
    }

    if (stem_arena[node_idx].sub_nodes[char_index] == -1) {
        stem_arena[node_idx].sub_nodes[char_index] = static_cast<int>(stem_arena.size());
        stem_arena.emplace_back();
    }
    int next_node = stem_arena[node_idx].sub_nodes[char_index];

    if (text.length() == pos + 1) {
        stem_arena[static_cast<size_t>(next_node)].mapped_value = val;
        stem_arena[static_cast<size_t>(next_node)].requires_suffix = req_suf;
        stem_arena[static_cast<size_t>(next_node)].allowed_cases = grammar_cases;
        stem_arena[static_cast<size_t>(next_node)].stem_length = static_cast<int>(text.length());
        return;
    }

    InsertStem(stem_arena, static_cast<size_t>(next_node), text, pos + 1, val, req_suf, grammar_cases);
}

static void InsertSuffix(std::vector<SuffixTrieNode>& suffix_arena, size_t node_idx, const std::wstring& text, ssize_t pos, int32_t grammar_cases) {
    if (pos < 0) {
        return;
    }

    int char_index = get_char_index(text[static_cast<size_t>(pos)]);
    if (char_index == -1) {
        return;
    }

    if (suffix_arena[node_idx].sub_nodes[char_index] == -1) {
        suffix_arena[node_idx].sub_nodes[char_index] = static_cast<int>(suffix_arena.size());
        suffix_arena.emplace_back();
    }
    int next_node = suffix_arena[node_idx].sub_nodes[char_index];

    if (pos == 0) {
        suffix_arena[static_cast<size_t>(next_node)].length = static_cast<int>(text.length());
        suffix_arena[static_cast<size_t>(next_node)].case_flags = grammar_cases;
        return;
    }

    InsertSuffix(suffix_arena, static_cast<size_t>(next_node), text, pos - 1, grammar_cases);
}

static StemTrieNode* SearchStemIter(std::vector<StemTrieNode>& stem_arena, const std::wstring& text, size_t start_idx, size_t end_idx) {
    int curr = 0;
    StemTrieNode* best_match = nullptr;

    for (size_t i = start_idx; i < end_idx; ++i) {
        int c = get_char_index(text[i]);

        if (c == -1) {
            break;
        }

        int next_node = stem_arena[static_cast<size_t>(curr)].sub_nodes[c];
        if (next_node == -1) {
            break;
        }
        curr = next_node;
        
        if (stem_arena[static_cast<size_t>(curr)].mapped_value != MAX_NUM_VAL && 
            ((end_idx - start_idx) - static_cast<size_t>(stem_arena[static_cast<size_t>(curr)].stem_length) < 4)
        ) {
            best_match = &stem_arena[static_cast<size_t>(curr)];
        }
    }

    return best_match;
}

static SuffixTrieNode* SearchSuffixIter(std::vector<SuffixTrieNode>& suffix_arena, const std::wstring& text, size_t start_idx, size_t end_idx) {
    int curr = 0;

    for (size_t i = end_idx - 1; i >= start_idx; --i) {
        int c = get_char_index(text[i]);
        if (c == -1) {
            break;
        }

        int next = suffix_arena[static_cast<size_t>(curr)].sub_nodes[c];
        if (next == -1) {
            break;
        }

        curr = next;
    }

    return &suffix_arena[static_cast<size_t>(curr)];
}

static bool IsMultiplier(uint64_t val) {
    return val == 1'000 || val == 1'000'000 || val == 1'000'000'000;
}

static NumberScale DetermineScale(uint64_t val) {
    if (val < 20)               return SCALE_DIGIT;
    if (val < 100)              return SCALE_TEN;
    if (val < 1'000)            return SCALE_HUNDRED;
    if (val < 1'000'000)        return SCALE_THOUSAND;
    if (val < 1'000'000'000)    return SCALE_MILLION;

    return SCALE_BILLION;
}

static void PrintStr(const std::wstring& s) { 
    if (!s.empty()) {
        fputws(s.c_str(), stdout);
    }
}

static void PrintNum(uint64_t n) { 
    fwprintf(stdout, L"%llu", (unsigned long long)n); 
}
static void PrintChar(wchar_t c) { 
    fputwc(c, stdout); 
}

int main() {
    std::vector<StemTrieNode> stem_arena;
    std::vector<SuffixTrieNode> suffix_arena;

    const std::vector<SuffixEntry> SUFFIX_LIST = {
        {L"ой",     CASE_NOM | CASE_GEN | CASE_DAT | CASE_ACC | CASE_INS | CASE_PRE},
        {L"ый",     CASE_NOM | CASE_ACC}, 
        {L"ий",     CASE_NOM | CASE_ACC}, 
        {L"ая",     CASE_NOM},
        {L"яя",     CASE_NOM}, 
        {L"ое",     CASE_NOM | CASE_ACC}, 
        {L"ее",     CASE_NOM | CASE_ACC},
        {L"ые",     CASE_NOM | CASE_ACC}, 
        {L"ие",     CASE_NOM | CASE_ACC}, 
        {L"ого",    CASE_GEN | CASE_ACC},
        {L"его",    CASE_GEN | CASE_ACC}, 
        {L"ей",     CASE_GEN | CASE_DAT | CASE_INS | CASE_PRE},
        {L"ых",     CASE_GEN | CASE_ACC | CASE_PRE}, 
        {L"их",     CASE_GEN | CASE_ACC | CASE_PRE},
        {L"ому",    CASE_DAT}, 
        {L"ему",    CASE_DAT}, 
        {L"ым",     CASE_DAT | CASE_INS},
        {L"им",     CASE_DAT | CASE_INS}, 
        {L"ую",     CASE_ACC}, 
        {L"юю",     CASE_ACC},
        {L"ыми",    CASE_INS}, 
        {L"ими",    CASE_INS}, 
        {L"ою",     CASE_INS},
        {L"ею",     CASE_INS}, 
        {L"ом",     CASE_PRE}, 
        {L"ем",     CASE_PRE}
    };

    const std::vector<DictionaryEntry> ORDINAL_STEMS = {
        {L"нулев", 0}, 
        {L"перв", 1}, 
        {L"втор", 2}, 
        {L"трет", 3}, 
        {L"треть", 3},
        {L"четвёрт", 4}, 
        {L"пят", 5}, 
        {L"шест", 6}, 
        {L"седьм", 7}, 
        {L"восьм", 8},
        {L"девят", 9}, 
        {L"десят", 10}, 
        {L"одиннадцат", 11}, 
        {L"двенадцат", 12},
        {L"тринадцат", 13}, 
        {L"четырнадцат", 14}, 
        {L"пятнадцат", 15},
        {L"шестнадцат", 16}, 
        {L"семнадцат", 17}, 
        {L"восемнадцат", 18},
        {L"девятнадцат", 19}, 
        {L"двадцат", 20}, 
        {L"тридцат", 30}, 
        {L"сороков", 40},
        {L"пятидесят", 50}, 
        {L"шестидесят", 60}, 
        {L"семидесят", 70},
        {L"восьмидесят", 80}, 
        {L"девяност", 90}, 
        {L"сот", 100},
        {L"тысячн", 1000}, 
        {L"миллионн", 1000000}, 
        {L"миллиардн", 1000000000},
    };

    const std::vector<DictionaryEntry> CARDINAL_NUMBERS = {
        {L"ноль",                 0, CASE_NOM | CASE_ACC}, 
        {L"ноля",                 0, CASE_GEN},
        {L"нолю",                 0, CASE_DAT}, 
        {L"нолём",                0, CASE_INS},
        {L"ноле",                 0, CASE_PRE}, 
        {L"один",                 1, CASE_NOM | CASE_ACC},
        {L"одно",                 1, CASE_NOM | CASE_ACC}, 
        {L"одна",                 1, CASE_NOM},
        {L"одни",                 1, CASE_NOM | CASE_ACC}, 
        {L"одного",               1, CASE_GEN | CASE_ACC},
        {L"одной",                1, CASE_GEN | CASE_DAT | CASE_INS | CASE_PRE}, 
        {L"одних",                1, CASE_GEN | CASE_ACC | CASE_PRE},
        {L"одному",               1, CASE_DAT}, 
        {L"одним",                1, CASE_DAT},
        {L"одну",                 1, CASE_ACC}, 
        {L"одним",                1, CASE_INS},
        {L"одними",               1, CASE_INS}, 
        {L"одном",                1, CASE_PRE},
        {L"два",                  2, CASE_NOM | CASE_ACC}, 
        {L"две",                  2, CASE_NOM | CASE_ACC},
        {L"двух",                 2, CASE_GEN | CASE_PRE}, 
        {L"двум",                 2, CASE_DAT},
        {L"двумя",                2, CASE_INS}, 
        {L"три",                  3, CASE_NOM | CASE_ACC},
        {L"трёх",                 3, CASE_GEN | CASE_PRE}, 
        {L"трём",                 3, CASE_DAT},
        {L"тремя",                3, CASE_INS}, 
        {L"четыре",               4, CASE_NOM | CASE_ACC},
        {L"четырёх",              4, CASE_GEN | CASE_PRE}, 
        {L"четырём",              4, CASE_DAT},
        {L"четырьмя",             4, CASE_INS}, 
        {L"пять",                 5, CASE_NOM | CASE_ACC},
        {L"пяти",                 5, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"пятью",                5, CASE_INS},
        {L"шесть",                6, CASE_NOM | CASE_ACC}, 
        {L"шести",                6, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"шестью",               6, CASE_INS}, 
        {L"семь",                 7, CASE_NOM | CASE_ACC},
        {L"семи",                 7, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"семью",                7, CASE_INS},
        {L"восемь",               8, CASE_NOM | CASE_ACC}, 
        {L"восьми",               8, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"восемью",              8, CASE_INS}, 
        {L"девять",               9, CASE_NOM | CASE_ACC},
        {L"девяти",               9, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"девятью",              9, CASE_INS},
        {L"десять",              10, CASE_NOM | CASE_ACC}, 
        {L"десяти",              10, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"десятью",             10, CASE_INS}, 
        {L"одиннадцать",         11, CASE_NOM | CASE_ACC},
        {L"одиннадцати",         11, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"одиннадцатью",        11, CASE_INS},
        {L"двенадцать",          12, CASE_NOM | CASE_ACC}, 
        {L"двенадцати",          12, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"двенадцатью",         12, CASE_INS}, 
        {L"тринадцать",          13, CASE_NOM | CASE_ACC},
        {L"тринадцати",          13, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"тринадцатью",         13, CASE_INS},
        {L"четырнадцать",        14, CASE_NOM | CASE_ACC}, 
        {L"четырнадцати",        14, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"четырнадцатью",       14, CASE_INS}, 
        {L"пятнадцать",          15, CASE_NOM | CASE_ACC},
        {L"пятнадцати",          15, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"пятнадцатью",         15, CASE_INS},
        {L"шестнадцать",         16, CASE_NOM | CASE_ACC}, 
        {L"шестнадцати",         16, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"шестнадцатью",        16, CASE_INS}, 
        {L"семнадцать",          17, CASE_NOM | CASE_ACC},
        {L"семнадцати",          17, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"семнадцатью",         17, CASE_INS},
        {L"восемнадцать",        18, CASE_NOM | CASE_ACC}, 
        {L"восемнадцати",        18, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"восемнадцатью",       18, CASE_INS}, 
        {L"девятнадцать",        19, CASE_NOM | CASE_ACC},
        {L"девятнадцати",        19, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"девятнадцатью",       19, CASE_INS},
        {L"двадцать",            20, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"двадцати",            20, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"двадцатью",           20, CASE_INS}, 
        {L"тридцать",            30, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"тридцати",            30, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"тридцатью",           30, CASE_INS},
        {L"сорок",               40, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"сорока",              40, CASE_GEN | CASE_DAT | CASE_INS | CASE_PRE},
        {L"пятьдесят",           50, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"пятидесяти",          50, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"пятьюдесятью",        50, CASE_INS}, 
        {L"шестьдесят",          60, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"шестидесяти",         60, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"шестьюдесятью",       60, CASE_INS},
        {L"семьдесят",           70, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"семидесяти",          70, CASE_GEN | CASE_DAT | CASE_PRE},
        {L"семьюдесятью",        70, CASE_INS}, 
        {L"восемьдесят",         80, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"восьмидесяти",        80, CASE_GEN | CASE_DAT | CASE_PRE}, 
        {L"восьмьюдесятью",      80, CASE_INS},
        {L"девяносто",           90, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"девяноста",           90, CASE_GEN | CASE_DAT | CASE_INS | CASE_PRE},
        {L"сто",                100, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"ста",                100, CASE_GEN | CASE_DAT | CASE_INS | CASE_PRE},
        {L"двести",             200, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"двухсот",            200, CASE_GEN},
        {L"двумстам",           200, CASE_DAT}, 
        {L"двумястами",         200, CASE_INS},
        {L"двухстах",           200, CASE_PRE}, 
        {L"триста",             300, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"трёхсот",            300, CASE_GEN}, 
        {L"трёмстам",           300, CASE_DAT},
        {L"тремястами",         300, CASE_INS}, 
        {L"трёхстах",           300, CASE_PRE},
        {L"четыреста",          400, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"четырёхсот",         400, CASE_GEN},
        {L"четырёмстам",        400, CASE_DAT}, 
        {L"четырьмястами",      400, CASE_INS},
        {L"четырёхстах",        400, CASE_PRE}, 
        {L"пятьсот",            500, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"пятисот",            500, CASE_GEN}, 
        {L"пятистам",           500, CASE_DAT},
        {L"пятьюстами",         500, CASE_INS}, 
        {L"пятистах",           500, CASE_PRE},
        {L"шестьсот",           600, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"шестисот",           600, CASE_GEN},
        {L"шестистам",          600, CASE_DAT}, 
        {L"шестьюстами",        600, CASE_INS},
        {L"шестистах",          600, CASE_PRE}, 
        {L"семьсот",            700, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"семисот",            700, CASE_GEN}, 
        {L"семистам",           700, CASE_DAT},
        {L"семьюстами",         700, CASE_INS}, 
        {L"семистах",           700, CASE_PRE},
        {L"восемьсот",          800, CASE_NOM | CASE_ACC | CASE_PRE}, 
        {L"восьмисот",          800, CASE_GEN},
        {L"восьмистам",         800, CASE_DAT}, 
        {L"восемьюстами",       800, CASE_INS},
        {L"восьмистах",         800, CASE_PRE}, 
        {L"девятьсот",          900, CASE_NOM | CASE_ACC | CASE_PRE},
        {L"девятисот",          900, CASE_GEN}, 
        {L"девятистам",         900, CASE_DAT},
        {L"девятьюстами",       900, CASE_INS}, 
        {L"девятистах",         900, CASE_PRE},
        {L"тысяча",            1000, CASE_NOM}, 
        {L"тысячи",            1000, CASE_NOM | CASE_GEN | CASE_ACC},
        {L"тысяче",            1000, CASE_DAT | CASE_PRE}, 
        {L"тысячу",            1000, CASE_ACC},
        {L"тысячей",           1000, CASE_INS}, 
        {L"тысяч",             1000, CASE_NOM | CASE_GEN},
        {L"тысячам",           1000, CASE_DAT}, 
        {L"тысячами",          1000, CASE_INS},
        {L"тысячах",           1000, CASE_PRE}, 
        {L"миллион",        1000000, CASE_NOM | CASE_ACC},
        {L"миллиона",       1000000, CASE_GEN}, 
        {L"миллиону",       1000000, CASE_DAT},
        {L"миллионом",      1000000, CASE_INS}, 
        {L"миллионе",       1000000, CASE_PRE},
        {L"миллионы",       1000000, CASE_NOM | CASE_ACC}, 
        {L"миллионов",      1000000, CASE_NOM | CASE_GEN},
        {L"миллионам",      1000000, CASE_DAT}, 
        {L"миллионами",     1000000, CASE_INS},
        {L"миллионах",      1000000, CASE_PRE}, 
        {L"миллиард",    1000000000, CASE_NOM | CASE_ACC},
        {L"миллиарда",   1000000000, CASE_GEN}, 
        {L"миллиарду",   1000000000, CASE_DAT},
        {L"миллиардом",  1000000000, CASE_INS}, 
        {L"миллиарде",   1000000000, CASE_PRE},
        {L"миллиарды",   1000000000, CASE_NOM | CASE_ACC}, 
        {L"миллиардов",  1000000000, CASE_NOM | CASE_GEN},
        {L"миллиардам",  1000000000, CASE_DAT}, 
        {L"миллиардами", 1000000000, CASE_INS},
        {L"миллиардах",  1000000000, CASE_PRE},
    };

    setlocale(LC_ALL, "");

    suffix_arena.emplace_back();
    for (const auto& suffix : SUFFIX_LIST) {
        InsertSuffix(suffix_arena, 0, suffix.text, static_cast<ssize_t>(suffix.text.length() - 1), suffix.case_flags);
    }

    stem_arena.emplace_back();
    for (const auto& dict_entry : ORDINAL_STEMS) {
        InsertStem(stem_arena, 0, dict_entry.text, 0, dict_entry.numeric_value, true, CASE_NOM);
    }
    for (const auto& dict_entry : CARDINAL_NUMBERS) {
        InsertStem(stem_arena, 0, dict_entry.text, 0, dict_entry.numeric_value, false, dict_entry.case_flags);
    }

    std::wstring prev_word;
    std::wstring extracted_suffix;
    std::wstring ws_buffer;
    
    bool is_building = false;
    uint64_t total_acc = 0;
    uint64_t chunk_val = 0;
    NumberScale prev_scale = SCALE_BILLION;
    int32_t prev_grammar = 0;
    int prev_mult_scale = SCALE_BILLION + 1;

    std::wstring chunk;
    wchar_t buf[2048];

    auto process_segment = [&](const std::wstring& line) {
        size_t char_idx = 0;
        size_t len = line.length();

        while (char_idx < len) {
            size_t ws_start = char_idx;

            while (char_idx < len && !iswalpha(static_cast<wint_t>(line[char_idx]))) {
                if (is_building && !iswspace(static_cast<wint_t>(line[char_idx]))) {

                    if (total_acc + chunk_val >= 10) {
                        PrintNum(total_acc + chunk_val);

                        if (!extracted_suffix.empty()) {
                            PrintChar(L'-'); PrintStr(extracted_suffix);
                        }
                    } 
                    else {
                        PrintStr(prev_word);
                    }

                    PrintStr(ws_buffer);

                    for(size_t i = ws_start; i < char_idx; ++i) {
                        PrintChar(line[i]); 
                    }
                    
                    ws_buffer.clear();
                    is_building = false;
                    ws_start = char_idx; 
                }

                ++char_idx;
            }
            
            if (is_building) {
                ws_buffer.append(line, ws_start, char_idx - ws_start);
            } 
            else {
                for (size_t i = ws_start; i < char_idx; ++i) {
                    PrintChar(line[i]);
                }
            }

            if (char_idx == len) {
                break;
            }

            size_t word_start = char_idx;
            while (char_idx < len && iswalpha(static_cast<wint_t>(line[char_idx]))) {
                char_idx++;
            }
            size_t word_end = char_idx;

            StemTrieNode* found_stem = SearchStemIter(stem_arena, line, word_start, word_end);

            size_t missing_suffix = (found_stem == nullptr) 
                               ? 0 
                               : (word_end - word_start) - static_cast<size_t>(found_stem->stem_length);

            int32_t cur_grammar = (found_stem == nullptr) 
                                ? 0 
                                : found_stem->allowed_cases;

            if (missing_suffix != 0) {
                SuffixTrieNode* found_suffix = SearchSuffixIter(suffix_arena, line, word_end - missing_suffix, word_end);
                cur_grammar = found_suffix->case_flags;

                if (static_cast<size_t>(found_suffix->length) != missing_suffix) {
                    found_stem = nullptr;
                }
            }

            bool valid_num = found_stem != nullptr 
                          && found_stem->mapped_value != MAX_NUM_VAL 
                          && !(missing_suffix == 0 && found_stem->requires_suffix);

            if (valid_num) {
                uint64_t parsed_num = found_stem->mapped_value;
                NumberScale cur_scale = DetermineScale(parsed_num);

                if (is_building) {
                    bool breaks = (!extracted_suffix.empty()) 
                               || (prev_scale <= cur_scale && !IsMultiplier(parsed_num)) 
                               || (prev_mult_scale <= cur_scale) 
                               || (   
                                    !(cur_grammar & prev_grammar) 
                                 && !(chunk_val % 10 == 0 && (prev_grammar & CASE_NOM))
                                  );

                    if (breaks) {
                        if (total_acc + chunk_val >= 10) {
                            PrintNum(total_acc + chunk_val);
                            if (!extracted_suffix.empty()) {
                                PrintChar(L'-'); PrintStr(extracted_suffix);
                            }
                        } 
                        else {
                            PrintStr(prev_word);
                        }
                        PrintStr(ws_buffer);

                        extracted_suffix.assign(line, word_end - missing_suffix, missing_suffix);
                        prev_word.assign(line, word_start, word_end - word_start);
                        chunk_val = parsed_num;
                        prev_mult_scale = IsMultiplier(parsed_num) ? cur_scale : SCALE_BILLION + 1;
                        total_acc = 0;
                    } 
                    else {
                        extracted_suffix.assign(line, word_end - missing_suffix, missing_suffix);
                        if (IsMultiplier(parsed_num)) {
                            chunk_val *= parsed_num;
                            total_acc += chunk_val;
                            chunk_val = 0;
                            prev_mult_scale = cur_scale;
                        } 
                        else {
                            chunk_val += parsed_num;
                        }
                    }
                } 
                else {
                    extracted_suffix.assign(line, word_end - missing_suffix, missing_suffix);
                    prev_word.assign(line, word_start, word_end - word_start);
                    chunk_val = parsed_num;
                    total_acc = 0;
                    prev_mult_scale = IsMultiplier(parsed_num) ? cur_scale : SCALE_BILLION + 1;
                    is_building = true;
                }

                prev_scale = cur_scale;
                prev_grammar = cur_grammar;
                ws_buffer.clear();

            } 
            else {
                if (is_building) {
                    if (total_acc + chunk_val >= 10) {
                        PrintNum(total_acc + chunk_val);
                        if (!extracted_suffix.empty()) {
                            PrintChar(L'-'); PrintStr(extracted_suffix);
                        }
                    } 
                    else {
                        PrintStr(prev_word);
                    }
                    PrintStr(ws_buffer);
                    is_building = false;
                }

                for (size_t k = word_start; k < word_end; ++k) {
                    PrintChar(line[k]);
                }
                ws_buffer.clear();
            }
        }
    };

    while (fgetws(buf, 2048, stdin)) {
        chunk.append(buf);

        ssize_t last_non_alpha = -1;

        for (ssize_t i = static_cast<ssize_t>(chunk.length()) - 1; i >= 0; --i) {
            if (!iswalpha(static_cast<wint_t>(chunk[static_cast<size_t>(i)]))) {
                last_non_alpha = i;
                break;
            }
        }

        if (last_non_alpha != -1) {
            std::wstring safe_part = chunk.substr(0, static_cast<size_t>(last_non_alpha + 1));
            process_segment(safe_part);
            chunk.erase(0, static_cast<size_t>(last_non_alpha + 1));
        }
    }
    
    if (!chunk.empty()) {
        process_segment(chunk);
    }

    if (is_building) {
        if (total_acc + chunk_val >= 10) {
            PrintNum(total_acc + chunk_val);

            if (!extracted_suffix.empty()) {
                PrintChar(L'-'); 
                PrintStr(extracted_suffix);
            }
        } 
        else {
            PrintStr(prev_word);
        }
        PrintStr(ws_buffer);
    }

    return 0;
}