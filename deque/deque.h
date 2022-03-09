#include <vector>

template<typename T>
class Deque {
public:

    Deque() : size_(0), buffer_(std::vector<std::vector<T *> *>(1)), begin_basket_(0), end_basket_(0), begin_ind_(1),
              end_ind_(0) {
        buffer_[0] = new std::vector<T *>(basket_cap);
    };

    explicit Deque(size_t sz, const T &val = T()) {
        size_ = sz;
        size_t num_baskets = size_ / basket_cap + 1;
        buffer_ = std::vector<std::vector<T *> *>(2 * (num_baskets + 1));
        begin_basket_ = num_baskets / 2 + 1;
        end_basket_ = begin_basket_ + num_baskets;
        begin_ind_ = 0;
        end_ind_ = size_ % basket_cap;
        size_t end;
        for (size_t i = begin_basket_; i < end_basket_; ++i) {
            buffer_[i] = new std::vector<T *>(basket_cap);
            try {
                for (end = 0; end < (i == end_basket_ - 1 ? end_ind_ : basket_cap); ++end) {
                    T *next = new T(val);
                    (*buffer_[i])[end] = next;
                }
            } catch (...) {
                for (size_t k = begin_basket_; k <= i; ++k) {
                    for (size_t j = 0; j < (k == i ? end : basket_cap); ++j) {
                        delete (*buffer_[k])[j];
                    }
                    delete buffer_[k];
                }
                setDefault();
                std::rethrow_exception(std::current_exception());
            }
        }
        --end_basket_;
        if (end_ind_ == 0) {
            delete buffer_[end_basket_];
            --end_basket_;
            end_ind_ = basket_cap - 1;
        } else {
            end_ind_--;
        }
    }

    size_t size() const {
        return size_;
    }

    Deque(const Deque<T> &other) : size_(other.size_), buffer_(std::vector<std::vector<T *> *>(other.buffer_.size())),
                                   begin_basket_(other.begin_basket_),
                                   end_basket_(other.end_basket_), begin_ind_(other.begin_ind_),
                                   end_ind_(other.end_ind_) {
        size_t end;
        for (size_t i = begin_basket_; i <= end_basket_; ++i) {
            buffer_[i] = new std::vector<T *>(basket_cap);
            try {
                for (end = (i == begin_basket_ ? begin_ind_ : 0);
                     end <= (i == end_basket_ ? end_ind_ : basket_cap - 1); ++end) {
                    (*buffer_[i])[end] = new T(*((*other.buffer_[i])[end]));
                }
            } catch (...) {
                for (size_t k = begin_basket_; k <= i; ++k) {
                    for (size_t j = 0; j < (k == i ? end : basket_cap); ++j) {
                        delete (*buffer_[k])[j];
                    }
                    delete buffer_[k];
                }
                setDefault();
                std::rethrow_exception(std::current_exception());
            }
        }
    }

    Deque<T> &operator=(const Deque<T> &other) {
        auto copy = other;
        swap(copy);
        return *this;
    }

    ~Deque() {
        for (size_t i = begin_basket_; i < end_basket_; ++i) {
            for (size_t j = (i == begin_basket_ ? begin_ind_ : 0);
                 j < (i == end_basket_ ? end_ind_ : basket_cap); ++j) {
                delete (*buffer_[i])[j];
            }
            delete buffer_[i];
        }
    }

    T &operator[](size_t index) {
        return *((*buffer_[begin_basket_ + (index + begin_ind_) / basket_cap])[(index + begin_ind_) %
                                                                               basket_cap]);
    }

    T *&getPtrRef(size_t index) {
        return ((*buffer_[begin_basket_ + (index + begin_ind_) / basket_cap])[(index + begin_ind_) %
                                                                              basket_cap]);
    }

    const T &operator[](size_t index) const {
        return *(*buffer_[begin_basket_ + (index + begin_ind_) / basket_cap])[(index + begin_ind_) %
                                                                              basket_cap];
    }

    T &at(size_t index) {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return *(*buffer_[begin_basket_ + (index + begin_ind_) / basket_cap])[(index + begin_ind_) %
                                                                              basket_cap];
    }

    const T &at(size_t index) const {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return *(*buffer_[begin_basket_ + (index + 1 + begin_ind_) / basket_cap])[(index + 1 + begin_ind_) %
                                                                                  basket_cap];
    }

    void push_back(const T &value) {
        if (end_ind_ == basket_cap - 1) {
            if (end_basket_ == buffer_.size()) {
                updateCapacity(buffer_.size() * 2, 0);
            }
            end_ind_ = 0;
            buffer_[++end_basket_] = new std::vector<T *>(basket_cap);

        } else {
            ++end_ind_;
        }
        ++size_;
        try {
            (*buffer_[end_basket_])[end_ind_] = new T(value);
        } catch (...) {
            std::rethrow_exception(std::current_exception());
        }
    }

    void push_front(const T &value) {
        if (begin_ind_ == 0) {
            if (begin_basket_ == 0) {
                updateCapacity(buffer_.size() * 2, buffer_.size());
            }
            begin_ind_ = basket_cap - 1;
            buffer_[--begin_basket_] = new std::vector<T *>(basket_cap);
        } else {
            --begin_ind_;
        }
        ++size_;
        try {
            (*buffer_[begin_basket_])[begin_ind_] = new T(value);
        } catch (...) {
            std::rethrow_exception(std::current_exception());
        }
    }

    void pop_front() {
        if (size_ == 0) {
            throw std::runtime_error("pop at zero size!");
        }
        --size_;
        delete (*buffer_[begin_basket_])[begin_ind_];
        if (begin_ind_ == basket_cap - 1) {
            delete buffer_[begin_basket_];
            begin_ind_ = 0, ++begin_basket_;
        } else {
            ++begin_ind_;
        }
    }

    void pop_back() {
        if (size_ == 0) {
            throw std::runtime_error("pop at zero size!");
        }
        --size_;
        delete (*buffer_[end_basket_])[end_ind_];
        if (end_ind_ == 0) {
            delete buffer_[end_basket_];
            end_ind_ = basket_cap - 1, --end_basket_;
        } else {
            --end_ind_;
        }
    }

    template<bool IsConst>
    class CommonIterator {
    private:
        using BasketIter = std::conditional_t<IsConst, typename std::vector<std::vector<T *> *>::const_iterator,
                typename std::vector<std::vector<T *> *>::iterator>;
        BasketIter iter;
        int pos = 0;
    public:

        CommonIterator() = default;

        CommonIterator(BasketIter it, int pos) : iter(it), pos(pos) {}

        CommonIterator(const CommonIterator<IsConst> &other) : iter(other.iter), pos(other.pos) {}

        CommonIterator &operator=(const CommonIterator<IsConst> &other) {
            iter = other.iter;
            pos = other.pos;
            return *this;
        }

        CommonIterator &operator+=(int n) {
            pos += n;
            if (pos >= 0) {
                iter += pos / basket_cap;
                pos = pos % basket_cap;
            } else {
                iter += (pos % basket_cap == 0) ? pos / basket_cap : pos / basket_cap - 1;
                pos = ((pos % basket_cap) + basket_cap) % basket_cap;
            }
            return *this;
        }

        CommonIterator &operator-=(int n) {
            return operator+=(-1 * n);
        }

        CommonIterator operator+(int n) {
            auto copy = *this;
            return copy += n;
        }

        CommonIterator operator-(int n) {
            auto copy = *this;
            return copy -= n;
        }


        CommonIterator &operator++() {
            return operator+=(1);
        }

        CommonIterator &operator--() {
            return operator-=(1);
        }

        CommonIterator operator++(int) {
            auto copy = *this;
            operator+=(1);
            return copy;
        }

        CommonIterator operator--(int) {
            auto copy = *this;
            operator-=(1);
            return copy;
        }

        int operator-(const CommonIterator<IsConst> &other) const {
            return basket_cap * (iter - other.iter) + (pos - other.pos);
        }

        bool operator==(const CommonIterator<IsConst> &other) const {
            return iter == other.iter && pos == other.pos;
        }

        bool operator!=(const CommonIterator<IsConst> &other) const {
            return !(operator==(other));
        }

        bool operator<(const CommonIterator<IsConst> &other) const {
            if (iter != other.iter) {
                return iter < other.iter;
            }
            return pos < other.pos;
        }

        bool operator<=(const CommonIterator<IsConst> &other) const {
            return operator<(other) || operator==(other);
        }

        bool operator>(const CommonIterator<IsConst> &other) const {
            return other < *this;
        }

        bool operator>=(const CommonIterator<IsConst> &other) const {
            return other <= *this;
        }

        std::conditional_t<IsConst, const T &, T &> operator*() {
            auto s = (**iter)[pos];
            return *s;
        }

        std::conditional_t<IsConst, const T *, T *> operator->() {
            return ((**iter)[pos]);
        }
    };

    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;

    using reverse_iterator = std::reverse_iterator<CommonIterator<false>>;
    using const_reverse_iterator = std::reverse_iterator<CommonIterator<true>>;

    iterator begin() {
        return iterator(buffer_.begin() + begin_basket_, begin_ind_);
    }

    const_iterator begin() const {
        return const_iterator(buffer_.cbegin() + begin_basket_, begin_ind_);
    }

    const_iterator cbegin() const {
        return const_iterator(buffer_.cbegin() + begin_basket_, begin_ind_);
    }


    iterator end() {
        if (end_ind_ != basket_cap - 1) {
            return iterator(buffer_.begin() + end_basket_, end_ind_ + 1);
        }
        return iterator(buffer_.begin() + (end_basket_ + 1), 0);
    }

    const_iterator end() const {
        if (end_ind_ != basket_cap - 1) {
            return const_iterator(buffer_.cbegin() + end_basket_, end_ind_ + 1);
        }
        return const_iterator(buffer_.cbegin() + (end_basket_ + 1), 0);
    }

    const_iterator cend() const {
        if (end_ind_ != basket_cap - 1) {
            return const_iterator(buffer_.cbegin() + end_basket_, end_ind_ + 1);
        }
        return const_iterator(buffer_.cbegin() + (end_basket_ + 1), 0);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(iterator(buffer_.begin() + end_basket_, end_ind_));
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(const_iterator(buffer_.begin() + end_basket_, end_ind_));
    }

//    reverse_iterator rend() {
//        return reverse_iterator(iterator(this, -1));
//    }
//
//    const_reverse_iterator crend() const {
//        return const_reverse_iterator(iterator(this, -1));
//    }

    void insert(iterator it, const T &val) {
        size_t pos = it - begin();
        push_back(val);
        T *ptr = (*buffer_[end_basket_])[end_ind_];
        for (size_t i = size_ - 1; i > pos; --i) {
            getPtrRef(i) = getPtrRef(i - 1);
        }
        getPtrRef(pos) = ptr;
    }

    void erase(iterator it) {
        size_t pos = it - begin();
        T *ptr = getPtrRef(pos);
        for (size_t i = pos; i < size_ - 1; ++i) {
            getPtrRef(i) = getPtrRef(i + 1);
        }
        getPtrRef(size_ - 1) = ptr;
        pop_back();
    }

private:

    void updateCapacity(size_t buffer_capacity, int shift) {
        std::vector<std::vector<T *> *> new_buffer_(buffer_capacity);
        for (size_t i = begin_basket_; i <= end_basket_; ++i) {
            new_buffer_[i + shift] = buffer_[i];
        }
        buffer_ = std::move(new_buffer_);
        begin_basket_ += shift, end_basket_ += shift;
    }

    void swap(Deque<T> &other) {
        std::swap(size_, other.size_);
        std::swap(buffer_, other.buffer_);
        std::swap(begin_basket_, other.begin_basket_);
        std::swap(end_basket_, other.end_basket_);
        std::swap(begin_ind_, other.begin_ind_);
        std::swap(end_ind_, other.end_ind_);
    }


    void setDefault() {
        size_ = 0;
        buffer_ = std::vector<std::vector<T *> *>(1);
        begin_basket_ = 0;
        end_basket_ = 0;
        begin_ind_ = 0;
        end_ind_ = 0;
    }

    static const int basket_cap = 10;
    size_t size_ = 0;
    std::vector<std::vector<T *> *> buffer_;
    size_t begin_basket_;
    size_t end_basket_;
    size_t begin_ind_;
    size_t end_ind_;
};

