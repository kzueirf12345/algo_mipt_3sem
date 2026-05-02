#include <bit>
#include <iostream>
#include <vector>
#include <string>
#include <compare>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <cstdio>

struct FastReader {
    static constexpr size_t BUF_SIZE = 1 << 16;
    char buf[BUF_SIZE];
    size_t pos = 0, len = 0;

    inline int next_char() {
        if (pos == len) {
            pos = 0;
            len = fread(buf, 1, BUF_SIZE, stdin);
            if (len == 0) {
                return EOF;
            }
        }
        return static_cast<unsigned char>(buf[pos++]);
    }

    inline size_t read_size_t() {
        size_t res = 0;
        int c = next_char();
        while (c != EOF && c <= ' ') {
            c = next_char();
        }
        if (c == EOF) return 0;
        while (c != EOF && c > ' ') {
            res = res * 10 + static_cast<size_t>(c - '0');
            c = next_char();
        }
        return res;
    }

    inline void read_hex_string(std::string& s) {
        s.clear();
        int c = next_char();
        while (c != EOF && c <= ' ') {
            c = next_char();
        }
        while (c != EOF && c > ' ') {
            s.push_back(static_cast<char>(c));
            c = next_char();
        }
    }
};

struct Complex {
    double r, i;
    Complex(double real = 0.0, double imag = 0.0) : r(real), i(imag) {}
    Complex operator+(const Complex& o) const { return Complex(r + o.r, i + o.i); }
    Complex operator-(const Complex& o) const { return Complex(r - o.r, i - o.i); }
    Complex operator*(const Complex& o) const { return Complex(r * o.r - i * o.i, r * o.i + i * o.r); }
};

const double PI = std::acos(-1.0);

static void fft(std::vector<Complex>& a, size_t n, bool invert) {
    for (size_t i = 1, j = 0; i < n; i++) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }

    for (size_t len = 2; len <= n; len <<= 1) {
        double ang = 2.0 * PI / static_cast<double>(len) * (invert ? -1.0 : 1.0);
        Complex wlen(std::cos(ang), std::sin(ang));

        for (size_t i = 0; i < n; i += len) {
            Complex w(1.0, 0.0);
            for (size_t j = 0; j < len / 2; j++) {
                Complex u = a[i + j];
                Complex v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w = w * wlen;
            }
        }
    }

    if (invert) {
        for (size_t i = 0; i < n; ++i) { 
            a[i].r /= static_cast<double>(n); 
            a[i].i /= static_cast<double>(n); 
        }
    }
}

class Integer;

static Integer inverse(const Integer& B);

class Integer {
public:
    std::vector<uint32_t> digits;
    bool negative = false;

    Integer() = default;
    Integer(uint64_t val);
    Integer(const Integer& other) = default;
    Integer(Integer&& other) noexcept = default;
    Integer& operator=(const Integer& other) = default;
    Integer& operator=(Integer&& other) noexcept = default;

    void trim() {
        while (!digits.empty() && digits.back() == 0) {
            digits.pop_back();
        }
        if (digits.empty()) {
            negative = false;
        }
    }

    int compare_abs(const Integer& other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() < other.digits.size() ? -1 : 1;
        }
        for (size_t i = digits.size(); i-- > 0; ) {
            if (digits[i] != other.digits[i]) {
                return digits[i] < other.digits[i] ? -1 : 1;
            }
        }
        return 0;
    }

    void add_abs(const Integer& other) {
        size_t sz1 = digits.size();
        size_t sz2 = other.digits.size();
        size_t n = std::max(sz1, sz2);
        uint64_t carry = 0;

        digits.resize(n, 0);
        for (size_t i = 0; i < n; ++i) {
            uint64_t other_val = (i < sz2 ? other.digits[i] : 0ULL);
            uint64_t cur = carry + digits[i] + other_val;
            digits[i] = static_cast<uint32_t>(cur & 0xFFFFFFFFULL);
            carry = cur >> 32;
        }
        if (carry) {
            digits.push_back(static_cast<uint32_t>(carry));
        }
    }

    void sub_abs(const Integer& other) {
        uint64_t borrow = 0;
        size_t sz1 = digits.size();
        size_t sz2 = other.digits.size();

        for (size_t i = 0; i < sz1; ++i) {
            uint64_t other_val = (i < sz2 ? other.digits[i] : 0ULL);
            uint64_t sub = other_val + borrow;
            if (digits[i] < sub) {
                digits[i] = static_cast<uint32_t>(0x100000000ULL + digits[i] - sub);
                borrow = 1;
            } 
            else {
                digits[i] -= static_cast<uint32_t>(sub);
                borrow = 0;
            }
        }
        trim();
    }

    bool get_bit(size_t n) const {
        size_t word_ind = n / 32;
        uint32_t bit_ind = static_cast<uint32_t>(n % 32);
        if (word_ind >= digits.size()) {
            return false;
        }
        return (digits[word_ind] >> bit_ind) & 1U;
    }

    void set_bit(size_t n) {
        size_t word_ind = n / 32;
        uint32_t bit_ind = static_cast<uint32_t>(n % 32);
        while (word_ind >= digits.size()) {
            digits.push_back(0);
        }
        digits[word_ind] |= (1U << bit_ind);
        trim();
    }

    uint32_t mod_small(uint32_t m) const {
        if (m == 0) {
            return 0;
        }
        uint64_t rem = 0;
        for (size_t i = digits.size(); i-- > 0; ) {
            rem = ((rem << 32) | digits[i]) % m;
        }
        return static_cast<uint32_t>(rem);
    }

    auto operator<=>(const Integer& other) const {
        if (digits.empty() && other.digits.empty()) return std::strong_ordering::equal;
        if (digits.empty()) return other.negative ? std::strong_ordering::greater : std::strong_ordering::less;
        if (other.digits.empty()) return negative ? std::strong_ordering::less : std::strong_ordering::greater;
        if (negative != other.negative) return negative ? std::strong_ordering::less : std::strong_ordering::greater;

        int cmp = compare_abs(other);
        if (negative) cmp = -cmp;
        return cmp <=> 0;
    }

    bool operator==(const Integer& other) const { return (*this <=> other) == 0; }
    bool operator>=(const Integer& other) const { return (*this <=> other) >= 0; }
    bool operator<=(const Integer& other) const { return (*this <=> other) <= 0; }
    bool operator<(const Integer& other) const { return (*this <=> other) < 0; }
    bool operator>(const Integer& other) const { return (*this <=> other) > 0; }

    Integer& operator+=(const Integer& other) {
        if (negative == other.negative) {
            add_abs(other);
        } 
        else {
            if (compare_abs(other) >= 0) {
                sub_abs(other);
            } 
            else {
                Integer tmp = other;
                tmp.sub_abs(*this);
                *this = std::move(tmp);
            }
        }
        trim();
        return *this;
    }

    Integer& operator-=(const Integer& other) {
        negative = !negative;
        *this += other;
        if (!digits.empty()) negative = !negative;
        else negative = false;
        return *this;
    }

    Integer naive_mul(const Integer& other) const {
        Integer res;
        size_t sz1 = digits.size();
        size_t sz2 = other.digits.size();
        res.digits.assign(sz1 + sz2, 0);

        uint32_t* res_ptr = res.digits.data();
        const uint32_t* other_ptr = other.digits.data();

        for (size_t i = 0; i < sz1; ++i) {
            uint32_t val_i = digits[i];
            if (val_i == 0) {
                continue;
            }
            uint64_t carry = 0;
            uint32_t* res_out = res_ptr + i;

            for (size_t j = 0; j < sz2; ++j) {
                uint64_t cur = res_out[j] + static_cast<uint64_t>(val_i) * other_ptr[j] + carry;
                res_out[j] = static_cast<uint32_t>(cur & 0xFFFFFFFFULL);
                carry = cur >> 32;
            }
            if (carry) {
                res_out[sz2] += static_cast<uint32_t>(carry);
            }
        }

        res.negative = (negative != other.negative);
        res.trim();
        return res;
    }

    Integer operator*(const Integer& other) const {
        if (digits.empty() || other.digits.empty()) {
            return Integer(0ULL);
        }

        if (digits.size() <= 64 || other.digits.size() <= 64) {
            return naive_mul(other);
        }

        static std::vector<Complex> fa, fb;
        size_t req_n = digits.size() * 2 + other.digits.size() * 2;
        size_t n = 1;
        while (n < req_n) {
            n <<= 1;
        }

        if (fa.size() < n) {
            fa.resize(n);
            fb.resize(n);
        }

        for (size_t i = 0; i < n; ++i) {
            fa[i] = Complex(0.0, 0.0);
            fb[i] = Complex(0.0, 0.0);
        }

        for (size_t i = 0; i < digits.size(); ++i) {
            fa[2 * i] = Complex(static_cast<double>(digits[i] & 0xFFFFU), 0.0);
            fa[2 * i + 1] = Complex(static_cast<double>(digits[i] >> 16), 0.0);
        }

        for (size_t i = 0; i < other.digits.size(); ++i) {
            fb[2 * i] = Complex(static_cast<double>(other.digits[i] & 0xFFFFU), 0.0);
            fb[2 * i + 1] = Complex(static_cast<double>(other.digits[i] >> 16), 0.0);
        }

        fft(fa, n, false); 
        fft(fb, n, false);

        for (size_t i = 0; i < n; ++i) { 
            fa[i] = fa[i] * fb[i];
        }

        fft(fa, n, true);

        Integer res;
        res.digits.reserve(n / 2 + 2);

        uint64_t carry = 0;
        for (size_t i = 0; i < n || carry; i += 2) {
            uint64_t v0 = (i < n ? static_cast<uint64_t>(std::round(fa[i].r)) : 0ULL) + carry;
            uint64_t d0 = v0 & 0xFFFFULL;
            carry = v0 >> 16;

            uint64_t v1 = (i + 1 < n ? static_cast<uint64_t>(std::round(fa[i + 1].r)) : 0ULL) + carry;
            uint64_t d1 = v1 & 0xFFFFULL;
            carry = v1 >> 16;

            res.digits.push_back(static_cast<uint32_t>(d0 | (d1 << 16)));
        }

        res.negative = (negative != other.negative);
        res.trim();
        return res;
    }

    Integer& operator*=(const Integer& other) { return *this = *this * other; }
    Integer operator+(const Integer& o) const { Integer r = *this; r += o; return r; }
    Integer operator-(const Integer& o) const { Integer r = *this; r -= o; return r; }

    Integer& operator<<=(uint64_t shift) {
        if (shift == 0 || digits.empty()) {
            return *this;
        }

        size_t words = static_cast<size_t>(shift / 32);
        uint32_t bits = static_cast<uint32_t>(shift % 32);

        if (words > 0) {
            digits.insert(digits.begin(), words, 0);
        }

        if (bits > 0) {
            uint32_t carry = 0;
            for (auto& digit : digits) {
                uint64_t val = (static_cast<uint64_t>(digit) << bits) | carry;
                digit = static_cast<uint32_t>(val & 0xFFFFFFFFULL);
                carry = static_cast<uint32_t>(val >> 32);
            }
            if (carry) {
                digits.push_back(static_cast<uint32_t>(carry));
            }
        }
        return *this;
    }

    Integer& operator>>=(uint64_t shift) {
        if (shift == 0 || digits.empty()) {
            return *this;
        }

        size_t words = static_cast<size_t>(shift / 32);
        uint32_t bits = static_cast<uint32_t>(shift % 32);

        if (words >= digits.size()) {
            digits.clear(); 
            negative = false; 
            return *this;
        }

        if (words > 0) {
            digits.erase(digits.begin(), digits.begin() + static_cast<std::ptrdiff_t>(words));
        }

        if (bits > 0) {
            uint32_t carry = 0;
            for (size_t i = digits.size(); i-- > 0; ) {
                uint32_t next_carry = digits[i] << (32 - bits);
                digits[i] = (digits[i] >> bits) | carry;
                carry = next_carry;
            }
        }
        trim();
        return *this;
    }

    Integer operator<<(uint64_t s) const { Integer r = *this; r <<= s; return r; }
    Integer operator>>(uint64_t s) const { Integer r = *this; r >>= s; return r; }
    Integer& operator++() { *this += Integer(1ULL); return *this; }
    Integer& operator--() { *this -= Integer(1ULL); return *this; }

    std::pair<Integer, Integer> divmod(const Integer& other) const {
        if (other.digits.empty() || digits.empty()) return {Integer(0ULL), Integer(0ULL)};
        
        int cmp = compare_abs(other);
        if (cmp < 0) {
            return {Integer(0ULL), *this};
        }

        if (cmp == 0) {
            Integer q(1ULL); 
            q.negative = (negative != other.negative);
            return {q, Integer(0ULL)};
        }

        uint64_t lz = static_cast<uint64_t>(std::countl_zero(other.digits.back()));
        Integer A = *this; A.negative = false;
        Integer B = other; B.negative = false;

        Integer A_shift = A << lz;
        Integer B_shift = B << lz;

        size_t m = A_shift.digits.size();
        size_t n = B_shift.digits.size();

        if (m < n) {
            return {Integer(0ULL), *this};
        }

        Integer B_pad = B_shift << (static_cast<uint64_t>(32) * static_cast<uint64_t>(m - n));
        Integer Inv = inverse(B_pad);

        Integer Q = (A_shift * Inv) >> (static_cast<uint64_t>(32) * static_cast<uint64_t>(m + n));
        Integer rem = A - Q * B;

        while (rem.negative) { 
            --Q; 
            rem += B; 
        }
        while (rem >= B) { 
            ++Q; 
            rem -= B; 
        }

        Q.negative = (negative != other.negative);
        rem.negative = negative;

        if (Q.digits.empty()) {
            Q.negative = false;
        }

        if (rem.digits.empty()) {
            rem.negative = false;
        }

        return {Q, rem};
    }

    Integer& operator/=(const Integer& other) { return *this = divmod(other).first; }
    Integer& operator%=(const Integer& other) { return *this = divmod(other).second; }
    Integer operator/(const Integer& o) const { Integer r = *this; r /= o; return r; }
    Integer operator%(const Integer& o) const { Integer r = *this; r %= o; return r; }

    static Integer from_hex(const std::string& s) {
        Integer res;
        if (s.empty()) {
            return res;
        }

        size_t len = s.length();
        size_t num_digits = (len + 7) / 8;
        res.digits.reserve(num_digits);

        for (size_t i = len; i > 0; ) {
            size_t chunk_len = std::min<size_t>(8, i);
            size_t start = i - chunk_len;

            uint32_t val = 0;
            for (size_t j = 0; j < chunk_len; ++j) {
                char c = s[start + j];
                uint32_t digit = 0;

                if (c >= '0' && c <= '9')      digit = static_cast<uint32_t>(c - '0');
                else if (c >= 'a' && c <= 'f') digit = static_cast<uint32_t>(c - 'a' + 10);
                else if (c >= 'A' && c <= 'F') digit = static_cast<uint32_t>(c - 'A' + 10);

                val = (val << 4) | digit;
            }

            res.digits.push_back(val);
            i -= chunk_len;
        }

        res.trim();
        return res;
    }

    std::string to_hex() const {
        if (digits.empty()) {
            return "0";
        }

        std::string s;
        s.reserve(digits.size() * 8);
        const char* hex_chars = "0123456789abcdef";
        bool leading = true;

        for (size_t i = digits.size(); i-- > 0; ) {
            uint32_t val = digits[i];
            for (uint32_t j = 8; j-- > 0; ) {
                uint32_t nibble = (val >> (j * 4)) & 0xFU;
                if (nibble != 0) {
                    leading = false;
                }

                if (!leading) {
                    s += hex_chars[nibble];
                }
            }
        }

        return s.empty() ? "0" : s;
    }
};

Integer::Integer(uint64_t val) {
    if (val == 0) {
        return;
    }

    digits.push_back(static_cast<uint32_t>(val & 0xFFFFFFFFULL));

    if (val >> 32) {
        digits.push_back(static_cast<uint32_t>(val >> 32));
    }
}

static Integer inverse(const Integer& B) {
    if (B.digits.empty()) {
        return Integer(0ULL);
    }

    size_t n = B.digits.size();

    if (n == 1) {
        uint64_t b = B.digits[0];
        uint64_t q = (~0ULL) / b;
        uint64_t rem = (~0ULL) % b;

        if (rem + 1 == b) {
            q++;
        }

        Integer Q_res;
        Q_res.digits.push_back(static_cast<uint32_t>(q & 0xFFFFFFFFULL));

        if (q >> 32) {
            Q_res.digits.push_back(static_cast<uint32_t>(q >> 32));
        }

        return Q_res;
    }

    size_t k = (n + 1) / 2;
    Integer B1 = B >> (static_cast<uint64_t>(32) * static_cast<uint64_t>(n - k));
    Integer X1 = inverse(B1);

    Integer Two(2ULL);
    Two <<= static_cast<uint64_t>(32) * static_cast<uint64_t>(n + k);
    
    Integer prod1 = X1 * B;
    while (prod1 > Two) {
        --X1;
        prod1 -= B;
    }
    
    Integer T = Two - prod1;
    Integer X = (X1 * T) >> (static_cast<uint64_t>(32) * static_cast<uint64_t>(2 * k));

    Integer limit(1ULL);
    limit <<= static_cast<uint64_t>(64) * static_cast<uint64_t>(n);
    
    Integer prod = X * B;
    while (prod > limit) {
        --X;
        prod -= B;
    }
    
    Integer rem = limit - prod;
    while (!rem.negative && rem >= B) {
        ++X;
        rem -= B;
    }

    return X;
}

class BarrettCtx {
public:
    Integer mod;
    Integer Inv;
    uint64_t lz;
    size_t n;

    BarrettCtx(const Integer& m_in) {
        mod = m_in;
        lz = static_cast<uint64_t>(std::countl_zero(mod.digits.back()));
        Integer mod_shift = mod << lz;
        n = mod_shift.digits.size();
        Inv = inverse(mod_shift);
    }

    Integer reduce(const Integer& A) const {
        if (A < mod) {
            return A;
        }

        Integer A_shift = A << lz;
        Integer Q = (A_shift * Inv) >> (static_cast<uint64_t>(64) * n);
        Integer rem = A - Q * mod;

        while (rem.negative) { 
            --Q; 
            rem += mod; 
        }

        while (rem >= mod) { 
            ++Q; 
            rem -= mod; 
        }
        return rem;
    }
};

static Integer power(const Integer& base_in, const Integer& exp_in, const BarrettCtx& ctx) {
    if (ctx.mod == Integer(1ULL)) {
        return Integer(0ULL);
    }
    
    Integer res(1ULL);
    Integer base = ctx.reduce(base_in);
    Integer exp = exp_in;

    while (exp > Integer(0ULL)) {
        if (exp.get_bit(0)) {
            res = ctx.reduce(res * base);
        }

        base = ctx.reduce(base * base);
        exp >>= 1ULL;
    }

    return res;
}

static bool miller_rabin(const Integer& n) {
    if (n < Integer(2ULL))                        return false;
    if (n == Integer(2ULL) || n == Integer(3ULL)) return true;
    if (!n.get_bit(0))                            return false;

    static const uint32_t primes[] = {
        3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 
        53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
        127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
        211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293
    };

    for (uint32_t p : primes) {
        if (n.digits.size() == 1 && n.digits[0] == p) {
            return true;
        }

        if (n.mod_small(p) == 0) {
            return false;
        }
    }

    Integer d = n - Integer(1ULL);
    size_t s = 0;

    while (!d.get_bit(0)) {
        d >>= 1ULL;
        s++;
    }

    BarrettCtx ctx(n);
    Integer n_minus_1 = n - Integer(1ULL);

    static const Integer bases[] = {
        Integer(2ULL), Integer(325ULL), Integer(9375ULL), 
        Integer(28178ULL), Integer(450775ULL), Integer(9780504ULL), Integer(1795265022ULL)
    };

    for (const Integer& a : bases) {
        if (n <= a) break;

        Integer x = power(a, d, ctx);
        if (x == Integer(1ULL) || x == n_minus_1) {
            continue;
        }

        bool composite = true;
        for (size_t r = 1; r < s; r++) {
            x = ctx.reduce(x * x);
            if (x == n_minus_1) {
                composite = false;
                break;
            }
            if (x == Integer(1ULL)) {
                break;
            }
        }

        if (composite) {
            return false;
        }
    }
    
    return true;
}

int main() {
    FastReader reader;
    size_t N = reader.read_size_t();

    if (N == 0) {
        return EXIT_SUCCESS;
    }

    Integer mask;
    std::string hex_str;
    for (size_t i = 0; i < N; ++i) {
        reader.read_hex_string(hex_str);
        Integer num = Integer::from_hex(hex_str);
        if (miller_rabin(num)) {
            mask.set_bit(i);
        }
    }

    std::string out = mask.to_hex();
    fputs(out.c_str(), stdout);
    fputc('\n', stdout);

    return EXIT_SUCCESS;
}