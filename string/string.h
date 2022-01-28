#include <iostream>
#include <cstring>

class String {
public:
    String() : size_(0), capacity_(8), data_(new char[1]) {}

    String(size_t n, char sym) : size_(n), capacity_(2 * n), data_(new char[2 * n]) {
        memset(data_, sym, n);
    }

    String(const char *str) : size_(strlen(str)) {
        capacity_ = 2 * size_, data_ = new char[capacity_];
        memcpy(data_, str, size_);
    }

    ~String() {
        delete[]data_;
    }

    void Swap(String &other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    String(const String &other) : size_(other.size_), capacity_(other.capacity_), data_(new char[other.capacity_]) {
        memcpy(data_, other.data_, size_);
    }

    String &operator=(const String &other) {
        String copy = other;
        Swap(copy);
        return *this;
    }

    size_t length() const {
        return size_;
    }

    void push_back(char sym) {
        if (size_ >= capacity_) {
            ChangeCapcity(2);
        }
        data_[size_++] = sym;
    }

    void pop_back() {
        --size_;
    }

    String &operator+=(const String &other) {
        if (size_ + other.size_ > capacity_) {
            ChangeCapcity(other.size_ / (size_ + 1) + 1);
        }
        memcpy(data_ + size_, other.data_, other.size_);
        size_ += other.size_;
        return *this;
    }

    String &operator+=(char sym) {
        push_back(sym);
        return *this;
    }

    char &front() {
        return data_[0];
    }

    char front() const {
        return data_[0];
    }

    char &back() {
        return data_[size_ - 1];
    }

    char back() const {
        return data_[size_ - 1];
    }

    char &operator[](size_t ind) {
        return data_[ind];
    }

    char operator[](size_t ind) const {
        return data_[ind];
    }

    void clear() {
        delete[] data_;
        size_ = 0, capacity_ = 8;
        data_ = new char[capacity_];
    }

    friend bool operator==(const String &fir, const String &sec);

    bool empty() const {
        return size_ == 0;
    }

    String substr(size_t left, size_t num) const {
        String result(num, '\0');
        memcpy(result.data_, data_ + left, num);
        return result;
    }

    size_t find(const String &pattern) const {
        size_t current = 0;
        while (current + pattern.size_ < size_) {
            if (memcmp(data_ + current, pattern.data_, pattern.size_) == 0) {
                return current;
            }
            ++current;
        }
        return size_;
    }

    size_t rfind(const String &pattern) const {
        int current = static_cast<int>(size_) - pattern.size_;
        while (current >= 0) {
            if (memcmp(data_ + current, pattern.data_, pattern.size_) == 0) {
                return current;
            }
            --current;
        }
        return size_;
    }

private:
    void ChangeCapcity(size_t multipier) {
        char *new_data = new char[capacity_ *= multipier];
        memcpy(new_data, data_, size_);
        delete[] data_;
        data_ = new_data;
    }

    size_t size_;
    size_t capacity_;
    char *data_;
};

bool operator==(const String &fir, const String &sec) {
    if (fir.size_ != sec.size_) {
        return false;
    }
    return memcmp(fir.data_, sec.data_, fir.size_) == 0;
}

String operator+(const String &first, const String &second) {
    String result = first;
    result += second;
    return result;
}

String operator+(const String &first, char second) {
    String result = first;
    result += second;
    return result;
}

String operator+(char first, const String &second) {
    String result(1, first);
    result += second;
    return result;
}

std::ostream &operator<<(std::ostream &out, const String &str) {
    for (size_t i = 0; i < str.length(); i++)
        out << str[i];
    return out;
}

std::istream &operator>>(std::istream &in, String &string) {
    string.clear();
    char c;
    do {
        in.get(c);
    } while (isspace(c));
    do {
        string.push_back(c);
    } while (in.get(c) && !isspace(c));
    return in;
}
