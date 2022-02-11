#pragma once

#include <string>

struct Block {
    Block() = default;

    size_t strong = 1;
    size_t weak = 0;
};

template<typename T>
class Shared;

template<typename T>
class Weak {
    friend class Shared<T>;

    using SharedType = Shared<T>;

public:
    Weak() = default;

    Weak(T *ptr) = delete;

    Weak(const SharedType &ptr) : ptr_(ptr.ptr_), counter_(ptr.counter_) {
        ++counter_->weak;
    }

    Weak(const Weak &other) : ptr_(other.ptr_), counter_(other.counter_) {
        if (counter_ != nullptr) {
            ++counter_->weak;
        }
    }

    Weak(Weak &&other) : ptr_(other.ptr_), counter_(other.counter_) {
        other.counter_ = nullptr;
        other.ptr_ = nullptr;
    }

    void Swap(Weak &other) {
        std::swap(ptr_, other.ptr_);
        std::swap(counter_, other.counter_);
    }

    Weak &operator=(const Weak &other) {
        if (this == &other) {
            return *this;
        }
        Weak copy(other);
        Swap(copy);
        return *this;
    }

    Weak &operator=(Weak &&other) {
        Swap(other);
        Weak deleter;
        deleter.Swap(other);
        return *this;
    }

    bool IsExpired() const {
        if (counter_ == nullptr) {
            return true;
        }
        return counter_->strong == 0;
    }

    SharedType Lock() const {
        if (IsExpired()) {
            return SharedType();
        } else {
            return SharedType(*this);
        }
    }

    ~Weak() {
        if (counter_ == nullptr) {
            return;
        }
        --counter_->weak;
        if (counter_->strong + counter_->weak == 0) {
            delete counter_;
        }
    }

private:
    T *ptr_ = nullptr;
    Block *counter_ = nullptr;
};

template<typename T>
class Shared {
    friend class Weak<T>;

public:
    Shared() = default;

    Shared(T *ptr) : ptr_(ptr), counter_(new Block()) {
    }

    Shared(const T &object) : ptr_(new T(object)), counter_(new Block) {
    }

    Shared(const Shared &other) : ptr_(other.ptr_), counter_(other.counter_) {
        if (counter_ != nullptr) {
            counter_->strong++;
        }
    }

    Shared(const Weak<T> &weak) : ptr_(weak.ptr_), counter_(weak.counter_) {
        if (weak.IsExpired()) {
            ptr_ = nullptr;
            counter_ = nullptr;
        } else {
            ++counter_->strong;
        }
    }

    Shared(Shared &&other) : ptr_(other.ptr_), counter_(other.counter_) {
        other.ptr_ = nullptr;
        other.counter_ = nullptr;
    }

    T &operator*() {
        return *ptr_;
    }

    T *operator->() {
        return ptr_;
    }

    T *Get() {
        return ptr_;
    }

    void Swap(Shared &other) {
        std::swap(ptr_, other.ptr_);
        std::swap(counter_, other.counter_);
    }

    Shared &operator=(Shared &&other) {
        Swap(other);
        Shared deleter;
        deleter.Swap(other);
        return *this;
    }

    Shared &operator=(const Shared &other) {
        if (this == &other) {
            return *this;
        }
        Shared copy(other);
        Swap(copy);
        return *this;
    }

    void Reset(T *ptr) {
        Shared reset(ptr);
        Swap(reset);
    }

    ~Shared() {
        if (counter_ == nullptr) {
            return;
        }
        --counter_->strong;
        if (counter_->strong == 0) {
            delete ptr_;
        }
        if (counter_->strong + counter_->weak == 0) {
            delete counter_;
        }
    }

private:
    T *ptr_ = nullptr;
    Block *counter_ = nullptr;
};

using SharedPtr = Shared<std::string>;
using WeakPtr = Weak<std::string>;
