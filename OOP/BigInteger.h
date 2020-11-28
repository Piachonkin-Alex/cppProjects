#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <assert.h>
#include <iomanip>
#include <math.h>

class DivideZeroException : public std::exception { // эксепшен на деление на ноль
public:
    const char *what() const noexcept override {
        return "Error, division by zero";
    }
};

class BigInteger {
public:
    explicit BigInteger(std::string num) {   // конструктор от строки
        if (num.length() == 0) {
            is_negative = false;
            digits.push_back(0);
        } else {
            if (num[0] == '-') {
                num = num.substr(1, num.length() - 1); 
                is_negative = true;
            }
            for (int i = num.length() - card_of_base; i >= -card_of_base + 1; i -= card_of_base) {
                int current_digit;
                if (i >= 0) {
                    current_digit = atoi(num.substr(i, card_of_base).c_str());
                } else {
                    current_digit = atoi(num.substr(0, i + card_of_base).c_str());
                }

                digits.push_back(current_digit);
            }
            RemoveZeros();
        }
    }
    BigInteger(long long num) : BigInteger(std::to_string(num)) {}; // конструктор от числа

    BigInteger() {
        digits.push_back(0);
    };  // по умолчанию
    explicit operator int() const {
        assert(digits.size() <= 1);
        return digits[0] * (is_negative ? -1 : 1);
    }

    std::string toString() const {
        if (*this == 0) {
            return "0";
        }
        std::string res;
        BigInteger copy = this->abs();
        while (copy != 0) {
            res += char('0' + int(copy % 10));
            copy /= 10;
        }
        res += (this->is_negative ? "-" : "");
        std::reverse(res.begin(), res.end());
        return res;
    }

    friend bool operator==(const BigInteger &first, const BigInteger &other) {
        return first.is_negative == other.is_negative && first.digits == other.digits;
    }

    friend bool operator!=(const BigInteger &first, const BigInteger &other) {
        return !(operator==(first, other));
    }

    explicit operator bool() {
        return (*this != BigInteger(0));
    }

    friend BigInteger operator-(const BigInteger &first) { //  унарный минус
        BigInteger copy(first);
        copy.is_negative = !copy.is_negative;
        copy.RemoveZeros();
        return copy;
    }

    bool IsLess(const BigInteger &other) const {
        if (this->digits.size() != other.digits.size()) {
            return (this->digits.size() < other.digits.size());
        }
        for (int i = this->digits.size() - 1; i >= 0; --i) {
            if (this->digits[i] != other.digits[i]) {
                return (this->digits[i] < other.digits[i]);
            }
        }
        return false;
    }

    friend bool operator<(const BigInteger &first, const BigInteger &other) {
        if (first == other) {
            return false;
        }
        if (first.is_negative) {
            return (other.is_negative ? -other < -first : true);
        } else {
            if (other.is_negative) {
                return false;
            }
            return first.IsLess(other);
        }
    }

    friend bool operator<=(const BigInteger &first, const BigInteger &other) {
        return (first < other || first == other);
    }

    friend bool operator>(const BigInteger &first, const BigInteger &other) {
        return (other < first);
    }

    friend bool operator>=(const BigInteger &first, const BigInteger &other) {
        return (other <= first);
    }

    // можно было бы добавить проверку, что если other закончился, а flag = 0, то дальше идти по массиву нет смысла. Это придало бы существенной скорости в работе с огромными числами плюс инт или лонг
    void AddNoSign(const BigInteger &other, bool type) {
        int flag = 0;
        for (uint i = 0; i < std::max(this->digits.size(), other.digits.size()) || flag != 0; ++i) {
            if (i == this->digits.size()) {
                this->digits.push_back(0);
            }
            this->digits[i] += flag + (type ? -1 : 1) * (i < other.digits.size() ? other.digits[i] : 0);
            if (this->digits[i] >= base) {
                flag = 1;
                this->digits[i] -= base;
            } else if (this->digits[i] < 0) {
                flag = -1;
                this->digits[i] += base;
            } else {
                flag = 0;
            }
        }
        this->RemoveZeros();
    }

    BigInteger &operator+=(const BigInteger &other) {
        if (this->is_negative == other.is_negative) {
            this->AddNoSign(other, false);
        } else {
            if (other.IsLess(*this)) {
                this->AddNoSign(other, true);
            } else {// копирование - дорогое удовольствие. Уж лучше было бы изхитрится в AddNoSign на проверки
                BigInteger res = other;
                res.AddNoSign(*this, true);
                *this = res;
            }
        }
        return *this;
    }

    friend BigInteger operator+(const BigInteger &first, const BigInteger &other) {
        BigInteger res = first;
        res += other;
        return res;
    }

    void CheckMinusZero() {//??
    }
    // аналогичное += замечание
    BigInteger &operator-=(const BigInteger &other) {
        if (this->is_negative != other.is_negative) {
            this->AddNoSign(other, false);
        } else {
            if (other.IsLess(*this)) {
                this->AddNoSign(other, true);
            } else {
                BigInteger res = other;
                res.AddNoSign(*this, true);
                *this = res;
                this->is_negative = !other.is_negative;
            }
        }
        this->RemoveZeros();
        return *this;
    }

    friend BigInteger operator-(const BigInteger &first, const BigInteger &other) {
        BigInteger res = first;
        res -= other;
        return res;
    }

    BigInteger &operator++() {
        *this += 1;
        return *this;
    }

    BigInteger &operator--() {
        *this -= 1;
        return *this;
    }

    BigInteger operator++(int) {
        BigInteger copy = *this;
        *this += BigInteger("1");
        return copy;
    }

    BigInteger operator--(int) {
        BigInteger copy = *this;
        *this -= BigInteger("1");
        return copy;
    }

    BigInteger MultipleOnInteger(const BigInteger &other) const {
        BigInteger res;
        res.digits.clear();
        long long flag = 0;
        for (uint i = 0; i < digits.size() || flag != 0; ++i) {
            long long current = flag + (i < digits.size() ? digits[i] : 0) * 1LL * other.digits[0];
            flag = current / base;
            res.digits.push_back(current % base);
        }
        return res;
        /*  long long val = this->digits[0] * 1LL * other.digits[0];
        long long second = val / long(base);
        val = (val % base);
        BigInteger res = BigInteger("");
        res.digits[0] = val;
        if (second > 0) {
            res.digits.push_back(second);
        }
        return res;*/
    }

    void InPowBase(int n) {
        std::vector<int> val;
        for (int i = 0; i < n; ++i) {
            val.push_back(0);
        }
        for (uint i = 0; i < this->digits.size(); ++i) {
            val.push_back(this->digits[i]);
        }
        this->digits = val;
    }

    friend BigInteger operator*(const BigInteger &first, const BigInteger &other) {
        BigInteger res;
        if (first.digits.size() == 1) {
            res = other.MultipleOnInteger(first);
        } else if (other.digits.size() == 1) {
            res = first.MultipleOnInteger(other);
        } else {
            BigInteger x_l, x_r, y_l, y_r;
            uint n = std::max(first.digits.size(), other.digits.size());
            if (n % 2 == 1) {
                n += 1;
            }
            BigInteger fake_this = first;
            BigInteger fake_other = other;
            while (fake_this.digits.size() < n) {
                fake_this.digits.push_back(0);
            }
            while (fake_other.digits.size() < n) {
                fake_other.digits.push_back(0);
            }
            x_r.digits[0] = fake_this.digits[0];
            y_r.digits[0] = fake_other.digits[0];
            x_l.digits[0] = fake_this.digits[n / 2];
            y_l.digits[0] = fake_other.digits[n / 2];
            for (uint i = 1; i < n / 2; ++i) {
                x_r.digits.push_back(fake_this.digits[i]);
                y_r.digits.push_back(fake_other.digits[i]);
                x_l.digits.push_back(fake_this.digits[n / 2 + i]);
                y_l.digits.push_back(fake_other.digits[n / 2 + i]);
            }
            x_l.RemoveZeros();
            x_r.RemoveZeros();
            y_l.RemoveZeros();
            y_r.RemoveZeros();
            BigInteger prod1 = x_l * y_l;
            BigInteger prod2 = x_r * y_r;
            BigInteger prod3 = (x_l + x_r) * (y_l + y_r) - prod1 - prod2;
            prod1.InPowBase(n);
            prod3.InPowBase(n / 2);
            res += (prod1 + prod2 + prod3);
        }
        if (first.is_negative == other.is_negative) {
            res.is_negative = false;
        } else {
            res.is_negative = true;
        }
        res.RemoveZeros();
        return res;
    }

    BigInteger &operator*=(const BigInteger &other) {
        return *this = (*this * other);
    }

    friend BigInteger
    operator/(const BigInteger &first, const BigInteger &other) {
        if (other == 0) {
            throw DivideZeroException();
        }
        BigInteger copy_other = other;
        copy_other.is_negative = false;
        BigInteger res = 0, current = 0;
        res.digits.resize(first.digits.size());
        for (int i = first.digits.size() - 1; i >= 0; --i) {
            current.Shift();
            current.digits[0] = first.digits[i];
            current.RemoveZeros();
            int val = 0, left = 0, right = base - 1;
            while (left <= right) {
                int median = (left + right) / 2;
                BigInteger now = copy_other * median;
                if (now <= current) {
                    val = median;
                    left = median + 1;
                } else {
                    right = median - 1;
                }
            }
            res.digits[i] = val;
            current = current - copy_other * val;
        }
        res.is_negative = !(first.is_negative == other.is_negative);
        res.RemoveZeros();
        return res;
    }

    BigInteger &operator/=(const BigInteger &other) {
        return *this = (*this / other);
    }

    friend BigInteger operator%(const BigInteger &first, const BigInteger &other) {
        BigInteger res = first - (first / other) * other;
        return res;
    }

    BigInteger operator%=(const BigInteger &other) {
        return *this = (*this % other);
    }

    BigInteger abs() const {
        BigInteger res = *this;
        res.is_negative = false;
        return res;
    }

    bool IsOdd() const {
        return digits[0] & 1;
    }

    BigInteger pow(BigInteger n) const {
        if (n == BigInteger(0)) {
            return 1;
        }
        if (!n.IsOdd()) {
            BigInteger half = this->pow(n / BigInteger(2));
            return half * half;
        } else {
            return this->pow(n - BigInteger(1)) * *this;
        }
    }

private:
    static const int card_of_base = 9; 
    static const int base = 1000000000;
    std::vector<int> digits;
    bool is_negative = false;

    void RemoveZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
        if (digits.size() == 1 && digits[0] == 0) {
            is_negative = false;
        }
    }

    void Shift() {
        if (digits.size() == 1 && digits[0] == 0) {
            return;
        }
        digits.push_back(digits[digits.size() - 1]);
        for (int i = int(digits.size() - 2); i > 0; --i) {
            digits[i] = digits[i - 1];
        }
        digits[0] = 0;
    }
    friend std::ostream &operator<<(std::ostream &ostr, const BigInteger &numb) {
        if (numb.is_negative) {
            ostr << '-';
        }
        ostr << numb.digits[numb.digits.size() - 1];
        for (int i = int(numb.digits.size() - 2); i >= 0; --i) {
            std::string digit = std::to_string(numb.digits[i]);
            for (uint i = 0; i < card_of_base - digit.size(); ++i) {
                ostr << "0";
            }
            ostr << digit;
        }
        return ostr;
    }

    friend std::istream &operator>>(std::istream &istr, BigInteger &numb) {
        std::string num;
        istr >> num;
        numb = BigInteger(num);
        return istr;
    }

};

BigInteger operator ""_bi(const char *p, size_t) {
    return BigInteger(std::string(p));
}


BigInteger gcd(const BigInteger &first, const BigInteger &other) {
    return (first == 0 ? other : gcd(other % first, first));
}

class Rational {
public:
    Rational(const BigInteger &numer, const BigInteger &denom);

    Rational(const BigInteger &integer);

    Rational(long long integer);

    Rational();

    explicit operator double() const;

    Rational &operator+=(const Rational &other);

    Rational &operator-=(const Rational &other);

    Rational &operator*=(const Rational &other);

    Rational &operator/=(const Rational &other);

    std::string toString() const;

    std::string asDecimal(size_t precision = 0) const;

    friend bool operator<(const Rational &first, const Rational &other) {
        return (first.numerator * other.denominator < other.numerator * first.denominator);
    }

    void normalize() {
        if (denominator < 0) {
            denominator *= -1, numerator *= -1;
        }
        BigInteger com_divisor = gcd(numerator.abs(), denominator.abs());
        numerator /= com_divisor, denominator /= com_divisor;
    }

private:
    BigInteger numerator, denominator;

};

Rational operator+(const Rational &first, const Rational &other) {
    Rational res = first;
    return res += other;
}

Rational operator-(const Rational &first, const Rational &other) {
    Rational res = first;
    return res -= other;
}

Rational operator*(const Rational &first, const Rational &other) {
    Rational res = first;
    return res *= other;
}

Rational operator/(const Rational &first, const Rational &other) {
    Rational res = first;
    return res /= other;
}

Rational operator-(const Rational &first) {
    Rational res = first;
    return res *= (-1);
}

Rational operator++(Rational &first, int) {
    Rational copy = first;
    first += 1;
    return copy;
}

Rational &operator++(Rational &first) {
    return first += 1;
}

Rational operator--(Rational &first, int) {
    Rational copy = first;
    first -= 1;
    return copy;
}

Rational &operator--(Rational &first) {
    return first -= 1;
}

Rational &Rational::operator+=(const Rational &other) {
    *this = Rational(numerator * other.denominator + denominator * other.numerator,
                     denominator * other.denominator);
    normalize();
    return *this;
}

Rational &Rational::operator-=(const Rational &other) {
    return operator+=(-other);
}

Rational &Rational::operator*=(const Rational &other) {
    *this = Rational(numerator * other.numerator, denominator * other.denominator);
    normalize();
    return *this;
}

Rational &Rational::operator/=(const Rational &other) {
    *this = Rational(numerator * other.denominator, denominator * other.numerator);
    normalize();
    return *this;
}


bool operator>(const Rational &first, const Rational &other) {
    return other < first;
}

bool operator!=(const Rational &first, const Rational &other) {
    return (first < other || first > other);
}

bool operator==(const Rational &first, const Rational &other) {
    return !(first != other);
}

bool operator<=(const Rational &first, const Rational &other) {
    return !(first > other);
}

bool operator>=(const Rational &first, const Rational &other) {
    return !(first < other);
}

Rational::Rational(long long integer) : numerator(integer), denominator(1) {}

Rational::Rational(const BigInteger &integer) : numerator(integer), denominator(1) {}

Rational::Rational() : Rational(0) {}

Rational::Rational(const BigInteger &numer, const BigInteger &denom) : numerator(numer), denominator(denom) {
    normalize();
}

std::string Rational::toString() const {
    std::string res = numerator.toString();
    if (denominator != 1) {
        res += "/";
        res += denominator.toString();
    }
    return res;
}

std::string Rational::asDecimal(size_t precision) const {
    std::string res = "";
    res += (numerator < 0 ? "-" : "");
    BigInteger numer = numerator.abs();
    BigInteger denom = denominator.abs();
    BigInteger integ_part = numer / denom;
    res += integ_part.toString();
    if (precision) {
        res += '.';
        for (uint i = 0; i < precision; ++i) {
            numer = 10 * (numer % denom);
            res += (numer / denom).toString();
        }
    }
    return res;
}

std::ostream &operator<<(std::ostream &ostr, const Rational &first) {
    ostr << first.toString();
    return ostr;
}

Rational::operator double() const {
    return atof(this->asDecimal(100).c_str());
}


