#pragma once

class Any {
private:
    struct Base {
        virtual ~Base() = default;
        virtual std::unique_ptr<Base> Clone() = 0;
    };

    template <typename T>
    struct Derived : public Base {
        T value;
        std::unique_ptr<Base> Clone() override {
            return std::make_unique<Derived<T>>(value);
        }
        Derived(T value) : value(std::move(value)) {
        }
        ~Derived() = default;
    };
    std::unique_ptr<Base> ptr_;

public:
    Any() {
        ptr_ = nullptr;
    }

    template <class T>
    Any(const T& value) {
        ptr_ = std::make_unique<Derived<T>>(value);
    }

    template <class T>
    Any& operator=(const T& value) {
        auto copy = Any(value);
        Swap(copy);
        return *this;
    }

    Any(const Any& rhs) {
        ptr_ = rhs.ptr_->Clone();
    }
    Any& operator=(const Any& rhs) {
        if (this == &rhs) {
            return *this;
        }
        auto copy = rhs;
        Swap(copy);
        return *this;
    }

    Any(Any&& rhs) {
        ptr_ = std::move(rhs.ptr_);
    }
    ~Any() {
    }

    bool Empty() const {
        return ptr_.get() == nullptr;
    }

    void Clear() {
        ptr_ = nullptr;
    }
    void Swap(Any& rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

    template <class T>
    const T& GetValue() const {
        return dynamic_cast<const Derived<T>&>(*ptr_).value;
    }
};
