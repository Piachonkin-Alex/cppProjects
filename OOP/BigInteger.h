#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

class DivideZeroException : public std::exception {
public:
    const char *what() const noexcept override {
        return "Error, division by zero";
    }
};

class BigInteger {
private:
    static const int card_of_base = 9;
    static const long long base = 1000000000;
    std::vector<long long> digits;
    long long sign = 1;

    void RemoveZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
        fixZeroSign();
    }

    bool isFalse() const {
        return digits.size() == 1 && digits[0] == 0;
    }

    bool compareNoSignLess(const BigInteger &other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() < other.digits.size();
        }
        for (int i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] != other.digits[i]) {
                return (digits[i] < other.digits[i]);
            }
        }
        return false;
    }

    void Shift(size_t n) {
        if (!isFalse()) {
            digits.insert(digits.begin(), n, 0);
        }
    }

    void Swap(BigInteger &other) {
        std::swap(digits, other.digits);
        std::swap(sign, other.sign);
    }

    void fixZeroSign() {
        isFalse() ? sign = 1 : sign *= 1;
    }

    void addNoSign(const BigInteger &biggest, const BigInteger &less, int signum) {
        digits.resize(std::max(biggest.digits.size(), less.digits.size()) + 1);
        long long flag = 0;
        for (size_t i = 0; i < less.digits.size() || flag; ++i) {
            long long cur_digit = flag;
            if (i < biggest.digits.size()) {
                cur_digit += biggest.digits[i];
            }
            if (i < less.digits.size()) {
                cur_digit += signum * less.digits[i];
            }
            if (cur_digit >= base) {
                flag = 1;
                cur_digit -= base;
            } else if (cur_digit < 0) {
                flag = -1;
                cur_digit += base;
            } else {
                flag = 0;
            }
            digits[i] = cur_digit;
        }
        RemoveZeros();
    }

public:

    BigInteger() : digits(std::vector<long long>(1)) {}

    BigInteger(long long number) : sign(number >= 0 ? 1 : -1) {
        number = number >= 0 ? number : -number;
        while (number > 0) {
            digits.push_back(number % base), number /= base;
        }
        if (digits.empty()) {
            digits.resize(1);
        }
    }

    explicit BigInteger(const std::string &num) {
        if (num.length() == 0) {
            sign = 1;
            digits.push_back(0);
        } else {
            int minus_operator = 0;
            if (num[0] == '-') {
                sign = -1, minus_operator = 1;
            }
            digits.resize((num.length() - minus_operator + card_of_base) / card_of_base);
            for (size_t i = 0; i * card_of_base < num.length() - minus_operator; i++) {
                if (num.length() - minus_operator >= card_of_base * (i + 1)) {
                    digits[i] = std::stoi(num.substr(num.length() - (i + 1) * card_of_base, card_of_base));
                } else {
                    digits[i] = std::stoi(num.substr(minus_operator, num.length() - minus_operator - i * card_of_base));
                }
            }
            RemoveZeros();
        }
    }

    bool isNegative() const {
        return sign == -1;
    }

    BigInteger &operator+=(const BigInteger &other) {
        if (sign == other.sign) {
            addNoSign(*this, other, 1);
            return *this;
        }
        if (compareNoSignLess(other)) {
            addNoSign(other, *this, -1);
            sign *= -1;
        } else {
            addNoSign(*this, other, -1);
        }
        return *this;
    }

    BigInteger &operator-=(const BigInteger &other) {
        sign *= -1;
        *this += other;
        sign *= -1;
        fixZeroSign();
        return *this;
    }

    BigInteger &operator*=(const BigInteger &other);

    BigInteger &operator/=(const BigInteger &other);

    BigInteger &operator%=(const BigInteger &other);

    std::string toString() const {
        std::string result;
        result.reserve(digits.size() * card_of_base);
        for (size_t i = 0; i < digits.size(); i++) {
            size_t prev_size = result.size();
            long long copy_digit = digits[i];
            while (copy_digit > 0) {
                result += static_cast<char>(copy_digit % 10 + '0'), copy_digit /= 10;
            }
            if (i != digits.size() - 1) {
                result += std::string(card_of_base + prev_size - result.size(), '0');
            }
        }
        if (result.empty()) {
            result.push_back('0');
        }
        if (isNegative()) {
            result.push_back('-');
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    BigInteger operator-() const {
        BigInteger result = *this;
        result.sign *= -1;
        result.fixZeroSign();
        return result;
    }

    BigInteger &operator++() {
        return *this += 1;
    }

    BigInteger operator++(int) {
        BigInteger copy = *this;
        *this += BigInteger(1);
        return copy;
    }

    BigInteger &operator--() {
        return *this -= 1;
    }

    BigInteger operator--(int) {
        BigInteger copy = *this;
        *this -= BigInteger(1);
        return copy;
    }

    explicit operator bool() const {
        return !isFalse();
    }

    void clear() {
        sign = 1, digits.resize(1, 0);
    }

    friend std::istream &operator>>(std::istream &in, BigInteger &integer);

    friend bool operator<(const BigInteger &first, const BigInteger &second);

    friend bool operator==(const BigInteger &first, const BigInteger &second);
};

bool operator<(const BigInteger &first, const BigInteger &second) {
    if (first.isNegative() && !second.isNegative()) {
        return true;
    } else if (!first.isNegative() && second.isNegative()) {
        return false;
    }
    return first.isNegative() ? !first.compareNoSignLess(second) : first.compareNoSignLess(second);
}

bool operator<=(const BigInteger &first, const BigInteger &second) {
    return first == second || first < second;
}

bool operator>(const BigInteger &first, const BigInteger &second) {
    return second < first;
}

bool operator!=(const BigInteger &first, const BigInteger &second) {
    return !(first == second);
}

bool operator>=(const BigInteger &first, const BigInteger &second) {
    return !(first < second);
}

bool operator==(const BigInteger &first, const BigInteger &second) {
    return first.sign == second.sign && first.digits == second.digits;
}

std::ostream &operator<<(std::ostream &out, const BigInteger &number) {
    out << number.toString();
    return out;
}

std::istream &operator>>(std::istream &in, BigInteger &integer) {
    integer.clear();
    std::string input;
    in >> input;
    integer = BigInteger(input);
    return in;
}

BigInteger operator ""_bi(const char *p, size_t) {
    return BigInteger(std::string(p));
}

BigInteger operator*(const BigInteger &first, const BigInteger &second) {
    BigInteger result = first;
    return result *= second;
}

BigInteger operator-(const BigInteger &first, const BigInteger &second) {
    BigInteger result = first;
    return result -= second;
}

BigInteger operator+(const BigInteger &first, const BigInteger &second) {
    BigInteger result = first;
    return result += second;
}

BigInteger operator/(const BigInteger &first, const BigInteger &second) {
    BigInteger result = first;
    return result /= second;
}

BigInteger operator%(const BigInteger &first, const BigInteger &second) {
    BigInteger result = first;
    return result %= second;
}

BigInteger &BigInteger::operator*=(const BigInteger &other) {
    if (*this == 0 || other == 0) {
        return *this = 0;
    }
    if (other == -1 || other == 1) {
        sign *= other.sign;
        return *this;
    }
    std::vector<long long> result(digits.size() + other.digits.size() + 2, 0);
    for (size_t i = 0; i < other.digits.size(); ++i) {
        for (size_t j = 0; j < digits.size(); ++j) {
            result[i + j] += (other.digits[i] * digits[j]);
            if (result[i + j] >= base) {
                result[i + j + 1] += (result[i + j] / base);
                result[i + j] %= base;
            }
        }
    }
    std::swap(digits, result);
    sign *= other.sign;
    RemoveZeros();
    return *this;
}

BigInteger &BigInteger::operator/=(const BigInteger &other) {
    if (other == 0) {
        throw DivideZeroException();
    }
    BigInteger copy_other = other;
    copy_other.sign = 1;
    BigInteger res = 0, current = 0;
    res.digits.resize(digits.size());
    for (int i = digits.size() - 1; i >= 0; --i) {
        current.Shift(1);
        current.digits[0] = digits[i];
        current.RemoveZeros();
        long long val = 0, left = 0, right = base - 1;
        while (left <= right) {
            long long median = (left + right) / 2;
            BigInteger now = copy_other * median;
            if (now <= current) {
                val = median;
                left = median + 1;
            } else {
                right = median - 1;
            }
        }
        res.digits[i] = val;
        current = current - copy_other * static_cast<BigInteger>(val);
    }
    res.sign = sign * other.sign;
    res.RemoveZeros();
    Swap(res);
    return *this;
}

BigInteger &BigInteger::operator%=(const BigInteger &other) {
    BigInteger result = *this - (*this / other) * other;
    Swap(result);
    return *this;
}

BigInteger gcd(const BigInteger &first, const BigInteger &other) {
    return (first == 0 ? other : gcd(other % first, first));
}

BigInteger abs(const BigInteger &value) {
    return value < 0 ? -1 * value : value;
}

class Rational {
private:
    BigInteger numerator;
    BigInteger denominator;

    void normalize() {
        if (denominator < 0) {
            numerator *= -1, denominator *= -1;
        }
        BigInteger divider = gcd(abs(numerator), denominator);
        numerator /= divider;
        denominator /= divider;
    }

    static const size_t double_precision = 50;

public:
    Rational() : numerator(), denominator(1) {};

    Rational(long long number) : Rational(BigInteger(number)) {}

    Rational(const BigInteger &number) : numerator(number), denominator(1) {};

    Rational(const BigInteger &top, const BigInteger &bottom) : numerator(top), denominator(bottom) {
        normalize();
    };

    Rational &operator+=(const Rational &another_num) {
        numerator = numerator * another_num.denominator + denominator * another_num.numerator;
        denominator = denominator * another_num.denominator;
        normalize();
        return *this;
    }

    Rational &operator-=(const Rational &another_num) {
        numerator = numerator * another_num.denominator - denominator * another_num.numerator;
        denominator = denominator * another_num.denominator;
        normalize();
        return *this;
    }

    Rational &operator*=(const Rational &another_num) {
        numerator = numerator * another_num.numerator;
        denominator = denominator * another_num.denominator;
        normalize();
        return *this;

    };

    Rational &operator/=(const Rational &another_num) {
        numerator = numerator * another_num.denominator;
        denominator = denominator * another_num.numerator;
        normalize();
        return *this;
    }

    Rational operator-() const {
        Rational result(-numerator, denominator);
        return result;
    }

    std::string toString() const {
        std::string result = numerator.toString();
        if (numerator != 0 && denominator != 1) {
            result += "/" + denominator.toString();
        }
        return result;
    }

    std::string asDecimal(size_t precision) const {
        BigInteger numer = abs(numerator);
        BigInteger denom = abs(denominator);
        BigInteger integ_part = numer / denom;
        std::string result;
        result += (numerator < 0 ? "-" : "");
        result += integ_part.toString();
        result.reserve(result.size() + precision + 1);
        if (precision) {
            result += '.';
            for (size_t i = 0; i < precision; ++i) {
                numer = 10 * (numer % denom);
                result += (numer / denom).toString();
            }
        }
        return result;
    }

    explicit operator double() {
        return std::stod(asDecimal(double_precision));
    }

    friend bool operator<(const Rational &first, const Rational &second);

    friend bool operator==(const Rational &first, const Rational &second);
};

Rational operator+(const Rational &first, const Rational &second) {
    Rational result = first;
    result += second;
    return result;
}

Rational operator-(const Rational &first, const Rational &second) {
    Rational result = first;
    result -= second;
    return result;
}

Rational operator*(const Rational &first, const Rational &second) {
    Rational result = first;
    result *= second;
    return result;
}

Rational operator/(const Rational &first, const Rational &second) {
    Rational result = first;
    result /= second;
    return result;
}

bool operator<(const Rational &first, const Rational &second) {
    BigInteger top_left = first.numerator * second.denominator;
    BigInteger top_right = second.numerator * first.denominator;
    return top_left < top_right;
}

bool operator>(const Rational &first, const Rational &second) {
    return second < first;
}

bool operator==(const Rational &first, const Rational &second) {
    return (first.numerator == second.numerator && first.denominator == second.denominator);
}

bool operator!=(const Rational &first, const Rational &second) {
    return !(first == second);
}

bool operator<=(const Rational &first, const Rational &second) {
    return !(first > second);
}

bool operator>=(const Rational &first, const Rational &second) {
    return !(first < second);
}

