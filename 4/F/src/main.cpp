#include <iostream>
#include <vector>
#include <cstdint>

struct Token {
    uint8_t type; // 0 - WORD, 1 - NON_WORD
    std::vector<char32_t> text;
    std::vector<char32_t> normalized;
    uint64_t val;
    bool is_num;
    bool is_ord;
};

struct DictEntry {
    std::vector<char32_t> word;
    uint64_t val;
    bool is_ord;
};

std::vector<DictEntry> dictionary;

std::vector<char32_t> normalize(const std::vector<char32_t>& word) {
    std::vector<char32_t> res;
    res.reserve(word.size());
    for (size_t i = 0; i < word.size(); ++i) {
        char32_t ch = word[i];
        if (ch >= U'А' && ch <= U'Я') ch = ch - U'А' + U'а';
        if (ch == U'Ё' || ch == U'ё') ch = U'е';
        res.push_back(ch);
    }
    return res;
}

void add_dict(const char32_t* word, uint64_t val, bool is_ord) {
    std::vector<char32_t> w;
    for (size_t i = 0; word[i]; ++i) {
        w.push_back(word[i]);
    }
    dictionary.push_back({normalize(w), val, is_ord});
}

void add_quant(const char32_t* word, uint64_t val) {
    add_dict(word, val, false);
}

void add_ord(const char32_t* root, uint64_t val) {
    const char32_t* endings[] = {
        U"ый", U"ой", U"ого", U"ому", U"ым", U"ом", U"ая", U"ую", U"ое", U"ые", U"ых", U"ыми",
        U"ий", U"яя", U"ее", U"ие", U"его", U"ему", U"ем", U"им", U"ей", U"юю", U"их", U"ими"
    };
    for (size_t i = 0; i < 24; ++i) {
        std::vector<char32_t> w;
        for (size_t j = 0; root[j]; ++j) w.push_back(root[j]);
        for (size_t j = 0; endings[i][j]; ++j) w.push_back(endings[i][j]);
        dictionary.push_back({normalize(w), val, true});
    }
}

void init_dictionary() {
    // Количественные
    add_quant(U"ноль", 0); add_quant(U"ноля", 0); add_quant(U"нулю", 0); add_quant(U"нулём", 0); add_quant(U"нуле", 0);
    add_quant(U"один", 1); add_quant(U"одного", 1); add_quant(U"одному", 1); add_quant(U"одним", 1); add_quant(U"одном", 1); add_quant(U"одна", 1); add_quant(U"одной", 1); add_quant(U"одну", 1);
    add_quant(U"два", 2); add_quant(U"двух", 2); add_quant(U"двум", 2); add_quant(U"двумя", 2); add_quant(U"две", 2);
    add_quant(U"три", 3); add_quant(U"трёх", 3); add_quant(U"трём", 3); add_quant(U"тремя", 3);
    add_quant(U"четыре", 4); add_quant(U"четырёх", 4); add_quant(U"четырём", 4); add_quant(U"четырьмя", 4);
    add_quant(U"пять", 5); add_quant(U"пяти", 5); add_quant(U"пятью", 5);
    add_quant(U"шесть", 6); add_quant(U"шести", 6); add_quant(U"шестью", 6);
    add_quant(U"семь", 7); add_quant(U"семи", 7); add_quant(U"семью", 7);
    add_quant(U"восемь", 8); add_quant(U"восьми", 8); add_quant(U"восемью", 8);
    add_quant(U"девять", 9); add_quant(U"девяти", 9); add_quant(U"девятью", 9);
    add_quant(U"десять", 10); add_quant(U"десяти", 10); add_quant(U"десятью", 10);
    add_quant(U"одиннадцать", 11); add_quant(U"одиннадцати", 11); add_quant(U"одиннадцатью", 11);
    add_quant(U"двенадцать", 12); add_quant(U"двенадцати", 12); add_quant(U"двенадцатью", 12);
    add_quant(U"тринадцать", 13); add_quant(U"тринадцати", 13); add_quant(U"тринадцатью", 13);
    add_quant(U"четырнадцать", 14); add_quant(U"четырнадцати", 14); add_quant(U"четырнадцатью", 14);
    add_quant(U"пятнадцать", 15); add_quant(U"пятнадцати", 15); add_quant(U"пятнадцатью", 15);
    add_quant(U"шестнадцать", 16); add_quant(U"шестнадцати", 16); add_quant(U"шестнадцатью", 16);
    add_quant(U"семнадцать", 17); add_quant(U"семнадцати", 17); add_quant(U"семнадцатью", 17);
    add_quant(U"восемнадцать", 18); add_quant(U"восемнадцати", 18); add_quant(U"восемнадцатью", 18);
    add_quant(U"девятнадцать", 19); add_quant(U"девятнадцати", 19); add_quant(U"девятнадцатью", 19);
    add_quant(U"двадцать", 20); add_quant(U"двадцати", 20); add_quant(U"двадцатью", 20);
    add_quant(U"тридцать", 30); add_quant(U"тридцати", 30); add_quant(U"тридцатью", 30);
    add_quant(U"сорок", 40); add_quant(U"сорока", 40);
    add_quant(U"пятьдесят", 50); add_quant(U"пятидесяти", 50); add_quant(U"пятьюдесятью", 50);
    add_quant(U"шестьдесят", 60); add_quant(U"шестидесяти", 60); add_quant(U"шестьюдесятью", 60);
    add_quant(U"семьдесят", 70); add_quant(U"семидесяти", 70); add_quant(U"семьюдесятью", 70);
    add_quant(U"восемьдесят", 80); add_quant(U"восьмидесяти", 80); add_quant(U"восемьюдесятью", 80);
    add_quant(U"девяносто", 90); add_quant(U"девяноста", 90);
    add_quant(U"сто", 100); add_quant(U"ста", 100);
    add_quant(U"двести", 200); add_quant(U"двухсот", 200); add_quant(U"двумстам", 200); add_quant(U"двумястами", 200); add_quant(U"двухстах", 200);
    add_quant(U"триста", 300); add_quant(U"трёхсот", 300); add_quant(U"трёмстам", 300); add_quant(U"тремястами", 300); add_quant(U"трёхстах", 300);
    add_quant(U"четыреста", 400); add_quant(U"четырёхсот", 400); add_quant(U"четырёмстам", 400); add_quant(U"четырьмястами", 400); add_quant(U"четырёхстах", 400);
    add_quant(U"пятьсот", 500); add_quant(U"пятисот", 500); add_quant(U"пятистам", 500); add_quant(U"пятьюстами", 500); add_quant(U"пятистах", 500);
    add_quant(U"шестьсот", 600); add_quant(U"шестисот", 600); add_quant(U"шестистам", 600); add_quant(U"шестьюстами", 600); add_quant(U"шестистах", 600);
    add_quant(U"семьсот", 700); add_quant(U"семисот", 700); add_quant(U"семистам", 700); add_quant(U"семьюстами", 700); add_quant(U"семистах", 700);
    add_quant(U"восемьсот", 800); add_quant(U"восьмисот", 800); add_quant(U"восьмистам", 800); add_quant(U"восемьюстами", 800); add_quant(U"восьмистах", 800);
    add_quant(U"девятьсот", 900); add_quant(U"девятисот", 900); add_quant(U"девятистам", 900); add_quant(U"девятьюстами", 900); add_quant(U"девятистах", 900);
    add_quant(U"тысяча", 1000); add_quant(U"тысячи", 1000); add_quant(U"тысяче", 1000); add_quant(U"тысячу", 1000); add_quant(U"тысячей", 1000); add_quant(U"тысяч", 1000); add_quant(U"тысячам", 1000); add_quant(U"тысячами", 1000); add_quant(U"тысячах", 1000);
    add_quant(U"миллион", 1000000); add_quant(U"миллиона", 1000000); add_quant(U"миллиону", 1000000); add_quant(U"миллионом", 1000000); add_quant(U"миллионе", 1000000); add_quant(U"миллионы", 1000000); add_quant(U"миллионов", 1000000); add_quant(U"миллионам", 1000000); add_quant(U"миллионами", 1000000); add_quant(U"миллионах", 1000000);
    add_quant(U"миллиард", 1000000000); add_quant(U"миллиарда", 1000000000); add_quant(U"миллиарду", 1000000000); add_quant(U"миллиардом", 1000000000); add_quant(U"миллиарде", 1000000000); add_quant(U"миллиарды", 1000000000); add_quant(U"миллиардов", 1000000000); add_quant(U"миллиардам", 1000000000); add_quant(U"миллиардами", 1000000000); add_quant(U"миллиардах", 1000000000);

    // Порядковые
    add_ord(U"нулев", 0); add_ord(U"перв", 1); add_ord(U"втор", 2); add_ord(U"четверт", 4); add_ord(U"пят", 5);
    add_ord(U"шест", 6); add_ord(U"седьм", 7); add_ord(U"восьм", 8); add_ord(U"девят", 9); add_ord(U"десят", 10);
    add_ord(U"одиннадцат", 11); add_ord(U"двенадцат", 12); add_ord(U"тринадцат", 13); add_ord(U"четырнадцат", 14);
    add_ord(U"пятнадцат", 15); add_ord(U"шестнадцат", 16); add_ord(U"семнадцат", 17); add_ord(U"восемнадцат", 18);
    add_ord(U"девятнадцат", 19); add_ord(U"двадцат", 20); add_ord(U"тридцат", 30); add_ord(U"сороков", 40);
    add_ord(U"пятидесят", 50); add_ord(U"шестидесят", 60); add_ord(U"семидесят", 70); add_ord(U"восьмидесят", 80);
    add_ord(U"девяност", 90); add_ord(U"сот", 100); add_ord(U"двухсот", 200); add_ord(U"трехсот", 300);
    add_ord(U"четырехсот", 400); add_ord(U"пятисот", 500); add_ord(U"шестисот", 600); add_ord(U"семисот", 700);
    add_ord(U"восьмисот", 800); add_ord(U"девятисот", 900); add_ord(U"тысячн", 1000); add_ord(U"миллионн", 1000000);
    add_ord(U"миллиардн", 1000000000);

    // Исключения для 3
    const char32_t* third_forms[] = {
        U"третий", U"третьего", U"третьему", U"третьим", U"третьем",
        U"третья", U"третьей", U"третью", U"третье", U"третьи", U"третьих", U"третьими"
    };
    for (size_t i = 0; i < 12; ++i) {
        add_dict(third_forms[i], 3, true);
    }
}

std::vector<char32_t> decode_utf8(const std::vector<uint8_t>& in) {
    std::vector<char32_t> out;
    size_t i = 0;
    while (i < in.size()) {
        uint8_t c = in[i];
        if (c < 0x80) { out.push_back(c); ++i; }
        else if ((c & 0xE0) == 0xC0) {
            if (i + 1 < in.size()) { out.push_back(((c & 0x1F) << 6) | (in[i+1] & 0x3F)); i += 2; } else break;
        } else if ((c & 0xF0) == 0xE0) {
            if (i + 2 < in.size()) { out.push_back(((c & 0x0F) << 12) | ((in[i+1] & 0x3F) << 6) | (in[i+2] & 0x3F)); i += 3; } else break;
        } else if ((c & 0xF8) == 0xF0) {
            if (i + 3 < in.size()) { out.push_back(((c & 0x07) << 18) | ((in[i+1] & 0x3F) << 12) | ((in[i+2] & 0x3F) << 6) | (in[i+3] & 0x3F)); i += 4; } else break;
        } else { ++i; }
    }
    return out;
}

void encode_utf8(char32_t cp, std::vector<uint8_t>& out) {
    if (cp < 0x80) { out.push_back(static_cast<uint8_t>(cp)); }
    else if (cp < 0x800) { out.push_back(static_cast<uint8_t>(0xC0 | (cp >> 6))); out.push_back(static_cast<uint8_t>(0x80 | (cp & 0x3F))); }
    else if (cp < 0x10000) { out.push_back(static_cast<uint8_t>(0xE0 | (cp >> 12))); out.push_back(static_cast<uint8_t>(0x80 | ((cp >> 6) & 0x3F))); out.push_back(static_cast<uint8_t>(0x80 | (cp & 0x3F))); }
    else { out.push_back(static_cast<uint8_t>(0xF0 | (cp >> 18))); out.push_back(static_cast<uint8_t>(0x80 | ((cp >> 12) & 0x3F))); out.push_back(static_cast<uint8_t>(0x80 | ((cp >> 6) & 0x3F))); out.push_back(static_cast<uint8_t>(0x80 | (cp & 0x3F))); }
}

bool is_alpha(char32_t ch) {
    return (ch >= U'A' && ch <= U'Z') || (ch >= U'a' && ch <= U'z') || (ch >= 0x0400 && ch <= 0x04FF);
}

bool is_space_only(const std::vector<char32_t>& text) {
    for (size_t i = 0; i < text.size(); ++i) {
        char32_t c = text[i];
        if (c != U' ' && c != U'\n' && c != U'\r' && c != U'\t') return false;
    }
    return true;
}

uint64_t calc_seq(const std::vector<Token>& tokens, size_t start, size_t end) {
    uint64_t total = 0;
    uint64_t current = 0;
    for (size_t k = start; k <= end; k += 2) {
        uint64_t v = tokens[k].val;
        if (v == 1000 || v == 1000000 || v == 1000000000) {
            if (current == 0) current = 1;
            total += current * v;
            current = 0;
        } else {
            current += v;
        }
    }
    return total + current;
}

int main() {
    init_dictionary();

    std::vector<uint8_t> input;
    int32_t c;
    while ((c = std::cin.get()) != EOF) {
        input.push_back(static_cast<uint8_t>(c));
    }

    std::vector<char32_t> text = decode_utf8(input);
    std::vector<Token> tokens;
    
    size_t idx = 0;
    while (idx < text.size()) {
        Token t;
        if (is_alpha(text[idx])) {
            t.type = 0; // WORD
            while (idx < text.size() && is_alpha(text[idx])) {
                t.text.push_back(text[idx]);
                ++idx;
            }
            t.normalized = normalize(t.text);
            t.is_num = false; t.is_ord = false; t.val = 0;
            for (size_t j = 0; j < dictionary.size(); ++j) {
                if (dictionary[j].word == t.normalized) {
                    t.is_num = true;
                    t.val = dictionary[j].val;
                    t.is_ord = dictionary[j].is_ord;
                    break;
                }
            }
        } else {
            t.type = 1; // NON_WORD
            while (idx < text.size() && !is_alpha(text[idx])) {
                t.text.push_back(text[idx]);
                ++idx;
            }
        }
        tokens.push_back(t);
    }

    std::vector<uint8_t> output;
    
    auto print_token = [&](const Token& t) {
        for (size_t k = 0; k < t.text.size(); ++k) encode_utf8(t.text[k], output);
    };
    
    auto print_num = [&](uint64_t val) {
        if (val == 0) { encode_utf8(U'0', output); return; }
        std::vector<char32_t> digits;
        while (val > 0) { digits.push_back(U'0' + (val % 10)); val /= 10; }
        for (size_t k = digits.size(); k > 0; --k) encode_utf8(digits[k - 1], output);
    };

    size_t i = 0;
    while (i < tokens.size()) {
        if (tokens[i].type == 0 && tokens[i].is_num) {
            size_t j = i;
            while (j + 2 < tokens.size()) {
                if (tokens[j+1].type == 1 && is_space_only(tokens[j+1].text) &&
                    tokens[j+2].type == 0 && tokens[j+2].is_num) {
                    j += 2;
                } else {
                    break;
                }
            }
            uint64_t total = calc_seq(tokens, i, j);
            if (total >= 10) {
                print_num(total);
                if (tokens[j].is_ord) {
                    encode_utf8(U'-', output);
                    size_t len = tokens[j].text.size();
                    size_t end_len = 2;
                    if (len >= 3) {
                        char32_t c1 = tokens[j].normalized[len - 3];
                        char32_t c2 = tokens[j].normalized[len - 2];
                        char32_t c3 = tokens[j].normalized[len - 1];
                        
                        if ((c1 == U'о' && c2 == U'г' && c3 == U'о') ||
                            (c1 == U'е' && c2 == U'г' && c3 == U'о') ||
                            (c1 == U'о' && c2 == U'м' && c3 == U'у') ||
                            (c1 == U'е' && c2 == U'м' && c3 == U'у') ||
                            (c1 == U'ы' && c2 == U'м' && c3 == U'и') ||
                            (c1 == U'и' && c2 == U'м' && c3 == U'и')) {
                            end_len = 3;
                        }
                    }
                    if (end_len > len) end_len = len;
                    
                    for (size_t k = len - end_len; k < len; ++k) {
                        encode_utf8(tokens[j].text[k], output);
                    }
                }
                i = j + 1;
                continue;
            }
        }
        print_token(tokens[i]);
        ++i;
    }

    for (size_t k = 0; k < output.size(); ++k) {
        std::cout.put(static_cast<char>(output[k]));
    }

    return 0;
}