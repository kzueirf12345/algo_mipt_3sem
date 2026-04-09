//  В этой задаче вам предлагается разработать свою мини библиотеку для работы с длинной арифметикой. Реализуйте структуру данных Integer и методы работы с ней.

// Ваши типы (или type-alias) должны удовлетворять соответсвующему концепту. Сами концепты добавлять в код не надо, он буду присоеденены автоматически.


// namespace traits {
//     template<typename T, typename U>
//     concept CanArithmeticWith =
//         std::constructible_from<U>   &&
//         std::assignable_from<T&, U>  &&
//         requires(T x, const T& t, const U& u) {
//             { x += u } -> std::same_as<T&>;
//             { x -= u } -> std::same_as<T&>;
//             { x *= u } -> std::same_as<T&>;
//             { x /= u } -> std::same_as<T&>;
//             { x %= u } -> std::same_as<T&>;
//             { t + u }  -> std::same_as<T>;
//             { t - u }  -> std::same_as<T>;
//             { t * u }  -> std::same_as<T>;
//             { t / u }  -> std::same_as<T>;
//             { t % u }  -> std::same_as<T>;
//             { u + t }  -> std::same_as<T>;
//             { u - t }  -> std::same_as<T>;
//             { u * t }  -> std::same_as<T>;
//             { u / t }  -> std::same_as<T>;
//             { u % t }  -> std::same_as<T>;
//         };

//     template<class T>
//     concept Integer = std::regular<T>       &&
//         std::three_way_comparable<T>        &&
//         std::convertible_to<const T&, bool> &&
//         CanArithmeticWith<T,  int64_t>      &&
//         CanArithmeticWith<T, uint64_t>      &&
//         CanArithmeticWith<T, T>             &&
//         requires(T x, const T& cx, uint64_t u64, std::ostream& out, std::istream& in) {
//             { cx.template fits< int64_t>() } -> std::same_as<bool>;
//             { cx.template fits<uint64_t>() } -> std::same_as<bool>;
//             { static_cast< int64_t>(cx) } -> std::same_as< int64_t>; // Semantic note: Converison valid only if number value fits destination. Overwise implementation-defined.
//             { static_cast<uint64_t>(cx) } -> std::same_as<uint64_t>; // Semantic note: Converison valid only if number value fits destination. Overwise implementation-defined.

//             { +cx } -> std::same_as<T>;
//             { -cx } -> std::same_as<T>;

//             { ++x } -> std::same_as<T&>;
//             { --x } -> std::same_as<T&>;

//             { x++ } -> std::same_as<T>;
//             { x-- } -> std::same_as<T>;

//             { x <<= u64 } -> std::same_as<T&>;
//             { x >>= u64 } -> std::same_as<T&>;

//             { cx << u64 } -> std::same_as<T>;
//             { cx >> u64 } -> std::same_as<T>;

//             { out << cx } -> std::same_as<std::ostream&>;
//             { in  >>  x } -> std::same_as<std::istream&>;
//         };
// };


#include <bit>
#include <iostream>
#include <vector>
#include <string>
#include <compare>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <limits>

class Integer;

namespace std {
    template<std::integral T>
    struct common_type<Integer, T> { using type = Integer; };
    template<std::integral T>
    struct common_type<T, Integer> { using type = Integer; };
}

class Integer {
    std::vector<uint32_t> digits;
    bool negative = false;

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

        for (int i = (int)digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i]) {
                return digits[i] < other.digits[i] ? -1 : 1;
            }
        }
        return 0;
    }

    void add_abs(const Integer& other) {
        const size_t n = std::max(digits.size(), other.digits.size());

        uint64_t carry = 0;

        for (size_t i = 0; i < n || carry; ++i) {
            if (i >= digits.size()) {
                digits.push_back(0);
            }

            uint64_t cur = carry + digits[i] + (i < other.digits.size() ? other.digits[i] : 0);
            digits[i] = static_cast<uint32_t>(cur & 0xFFFFFFFF);

            carry = cur >> 32;
        }
    }

    void sub_abs(const Integer& other) { // |this| >= |other|
        const size_t n = other.digits.size();

        uint64_t borrow = 0;

        for (size_t i = 0; i < n || borrow; ++i) {
            uint64_t sub = (i < n ? (uint64_t)other.digits[i] : 0ULL) + borrow;

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

    int bit_length() const {
        if (digits.empty()) {
            return 0;
        }

        const int word_ind = (int)digits.size() - 1;

        return word_ind * 32 + std::bit_width(digits[word_ind]);
    }

    bool get_bit(int n) const {
        const int word_ind = n / 32;
        const int bit_ind = n % 32;

        if (word_ind >= (int)digits.size()) {
            return false;
        }

        return (digits[word_ind] >> bit_ind) & 1;
    }

    void set_bit(int n) {
        const int word_ind = n / 32;
        const int bit_ind = n % 32;

        while (word_ind >= (int)digits.size()) {
            digits.push_back(0);
        }

        digits[word_ind] |= (1U << bit_ind);
    }

public:

    Integer() = default;

    Integer(int64_t val) {
        if (val < 0) {
            negative = true;
            const uint64_t abs_v = (val == std::numeric_limits<int64_t>::min()) 
                                 ? static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1
                                 : static_cast<uint64_t>(-val);

            digits = {static_cast<uint32_t>(abs_v & 0xFFFFFFFF), static_cast<uint32_t>(abs_v >> 32)};
        } 
        else {
            const uint64_t abs_v = val;
            digits = {static_cast<uint32_t>(abs_v & 0xFFFFFFFF), static_cast<uint32_t>(abs_v >> 32)};
        }

        trim();
    }

    Integer(uint64_t val) {
        digits = {static_cast<uint32_t>(val & 0xFFFFFFFF), static_cast<uint32_t>(val >> 32)};
        trim();
    }

    Integer(const std::string& s) {
        negative = false;
        digits.clear();

        size_t ind = 0;
        for (; ind < s.size() && std::isspace(s[ind]); ++ind);

        if (ind < s.size() && (s[ind] == '-' || s[ind] == '+')) {
            if (s[ind] == '-') {
                negative = true;
            }
            ++ind;
        }

        for (; ind < s.size() && std::isdigit(s[ind]); ++ind) {
            *this *= 10ul;
            add_abs(static_cast<uint64_t>(s[ind] - '0'));
        }

        trim();
    }

    operator bool() const { return !digits.empty(); }

    template<typename T>
    bool fits() const {
        if constexpr (std::is_same_v<T, uint64_t>) {
            if (negative) {
                return digits.empty();
            }

            if (digits.size() > 2) {
                return false;
            }

            return true;
        } 
        else {
            if (digits.size() > 2) {
                return false;
            }

            const uint64_t val = static_cast<uint64_t>(*this);

            if (negative) {
                return val <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1;
            }

            return val <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
        }
    }

    explicit operator uint64_t() const {
        uint64_t res = 0;

        if (digits.size() >= 1) res |= (uint64_t)digits[0];
        if (digits.size() >= 2) res |= ((uint64_t)digits[1] << 32);

        return res;
    }

    explicit operator int64_t() const {
        uint64_t res = static_cast<uint64_t>(*this);

        if (negative) {
            return -static_cast<int64_t>(res);
        }

        return static_cast<int64_t>(res);
    }

    auto operator<=>(const Integer& other) const {
        if (negative != other.negative) {
            return negative ? std::strong_ordering::less : std::strong_ordering::greater;
        }

        int cmp = compare_abs(other);

        if (negative) {
            cmp = -cmp;
        }

        return cmp <=> 0;
    }

    bool operator==(const Integer& other) const { 
        return (*this <=> other) == 0; 
    }

    Integer operator+() const { 
        return *this; 
    }

    Integer operator-() const {
        if (digits.empty()) {
            return *this;
        }

        Integer res = *this;
        res.negative = !negative;

        return res;
    }

    Integer& operator+=(const Integer& other) {
        if (negative == other.negative) {
            add_abs(other);
        } 
        else {
            const int cmp = compare_abs(other);

            if (cmp >= 0) {
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

        if (!digits.empty()) {
            negative = !negative;
        }
        else {
            negative = false;
        }

        return *this;
    }

    Integer& operator*=(const Integer& other) {
        if (digits.empty() || other.digits.empty()) {
            digits.clear(); 
            negative = false; 
            return *this;
        }

        std::vector<uint32_t> res_digits(digits.size() + other.digits.size(), 0);

        for (size_t i = 0; i < digits.size(); ++i) {
            uint64_t carry = 0;

            for (size_t j = 0; j < other.digits.size() || carry; ++j) {
                const uint64_t cur = res_digits[i + j] 
                                   + static_cast<uint64_t>(digits[i]) 
                                      * (j < other.digits.size() ? other.digits[j] : 0ull) 
                                   + carry;

                res_digits[i + j] = static_cast<uint32_t>(cur & 0xFFFFFFFF);
                carry = cur >> 32;
            }
        }

        negative = (negative != other.negative);
        digits = std::move(res_digits);

        trim();

        return *this;
    }

    std::pair<Integer, Integer> divmod(const Integer& other) const {
        if (other.digits.empty() || digits.empty()) {
            return {0ul, 0ul};
        }

        Integer quot = 0ul;
        Integer rem = 0ul;
        Integer b_abs = other; 
        b_abs.negative = false;
        
        for (int i = bit_length() - 1; i >= 0; --i) {
            rem <<= 1;

            if (get_bit(i)) {
                rem.set_bit(0);
            }

            if (rem.compare_abs(b_abs) >= 0) {
                rem.sub_abs(b_abs);
                quot.set_bit(i);
            }
        }

        quot.negative = (negative != other.negative);
        rem.negative = negative;

        quot.trim(); 
        rem.trim();

        return {quot, rem};
    }

    Integer& operator/=(const Integer& other) { 
        return *this = divmod(other).first; 
    }

    Integer& operator%=(const Integer& other) { 
        return *this = divmod(other).second; 
    }

    Integer& operator++() { 
        return *this += 1ul; 
    }

    Integer operator++(int) { 
        const Integer tmp = *this; 
        ++(*this); 
        return tmp; 
    }

    Integer& operator--() { 
        return *this -= 1ul; 
    }

    Integer operator--(int) { 
        Integer tmp = *this; 
        --(*this); 
        return tmp; 
    }

    Integer& operator<<=(uint64_t shift) {
        if (shift == 0 || digits.empty()) {
            return *this;
        }

        const uint64_t word_shift_ind = shift / 32;
        const uint32_t bit_shift_ind = shift % 32;

        if (word_shift_ind > 0) {
            digits.insert(digits.begin(), word_shift_ind, 0);
        }

        if (bit_shift_ind > 0) {
            uint32_t carry = 0;

            for (auto& digit : digits) {
                const uint64_t val = (static_cast<uint64_t>(digit) << bit_shift_ind) | carry;
                digit = static_cast<uint32_t>(val & 0xFFFFFFFF);
                carry = static_cast<uint32_t>(val >> 32);
            }

            if (carry) {
                digits.push_back(carry);
            }
        }

        return *this;
    }

    Integer& operator>>=(uint64_t shift) {
        if (shift == 0 || digits.empty()) {
            return *this;
        }

        const uint64_t word_shift_ind = shift / 32;
        const uint32_t bit_shift_ind = shift % 32;

        if (word_shift_ind >= digits.size()) { 
            digits.clear(); 
            negative = false; 
            return *this; 
        }

        if (word_shift_ind > 0) {
            digits.erase(digits.begin(), digits.begin() + word_shift_ind);
        }

        if (bit_shift_ind > 0) {
            uint32_t carry = 0;

            for (int i = (int)digits.size() - 1; i >= 0; --i) {
                uint32_t next_carry = (digits[i] << (32 - bit_shift_ind));
                digits[i] = (digits[i] >> bit_shift_ind) | carry;
                carry = next_carry;
            }
        }

        trim();

        return *this;
    }

    Integer operator<<(uint64_t s) const { 
        Integer r = *this;
        r <<= s;
        return r;
    }

    Integer operator>>(uint64_t s) const { 
        Integer r = *this;
        r >>= s;
        return r;
    }

    friend std::ostream& operator<<(std::ostream& out, const Integer& n) {
        if (n.digits.empty()) {
            return out << "0";
        }

        if (n.negative) {
            out << "-";
        }

        std::vector<uint32_t> parts;

        Integer tmp = n; 
        tmp.negative = false;

        while (tmp) {
            auto [q, r] = tmp.divmod(1000000000ul);
            parts.push_back(static_cast<uint32_t>(static_cast<uint64_t>(r)));
            tmp = q;
        }

        const int parts_size = static_cast<int>(parts.size());

        for (int i = parts_size - 1; i >= 0; --i) {
            if (i == parts_size - 1) {
                out << parts[i];
            }
            else {
                out << std::setfill('0') << std::setw(9) << parts[i];
            }
        }

        return out;
    }

    friend std::istream& operator>>(std::istream& in, Integer& n) {
        std::string string; 
        if (in >> string) {
            n = Integer(string);
        }

        return in;
    }

#define ARITHMETIC_OVERLOADS(TYPE) \
    Integer& operator+=(TYPE rhs) { return *this += Integer(rhs); } \
    Integer& operator-=(TYPE rhs) { return *this -= Integer(rhs); } \
    Integer& operator*=(TYPE rhs) { return *this *= Integer(rhs); } \
    Integer& operator/=(TYPE rhs) { return *this /= Integer(rhs); } \
    Integer& operator%=(TYPE rhs) { return *this %= Integer(rhs); } \
    friend Integer operator+(const Integer& lhs, TYPE rhs) { return Integer(lhs) += Integer(rhs); } \
    friend Integer operator-(const Integer& lhs, TYPE rhs) { return Integer(lhs) -= Integer(rhs); } \
    friend Integer operator*(const Integer& lhs, TYPE rhs) { return Integer(lhs) *= Integer(rhs); } \
    friend Integer operator/(const Integer& lhs, TYPE rhs) { return Integer(lhs) /= Integer(rhs); } \
    friend Integer operator%(const Integer& lhs, TYPE rhs) { return Integer(lhs) %= Integer(rhs); } \
    friend Integer operator+(TYPE lhs, const Integer& rhs) { return Integer(lhs) += rhs; } \
    friend Integer operator-(TYPE lhs, const Integer& rhs) { return Integer(lhs) -= rhs; } \
    friend Integer operator*(TYPE lhs, const Integer& rhs) { return Integer(lhs) *= rhs; } \
    friend Integer operator/(TYPE lhs, const Integer& rhs) { return Integer(lhs) /= rhs; } \
    friend Integer operator%(TYPE lhs, const Integer& rhs) { return Integer(lhs) %= rhs; }

    ARITHMETIC_OVERLOADS(int64_t)
    ARITHMETIC_OVERLOADS(uint64_t)
    
    friend Integer operator+(Integer lhs, const Integer& rhs) { return lhs += rhs; }
    friend Integer operator-(Integer lhs, const Integer& rhs) { return lhs -= rhs; }
    friend Integer operator*(Integer lhs, const Integer& rhs) { return lhs *= rhs; }
    friend Integer operator/(Integer lhs, const Integer& rhs) { return lhs /= rhs; }
    friend Integer operator%(Integer lhs, const Integer& rhs) { return lhs %= rhs; }
};