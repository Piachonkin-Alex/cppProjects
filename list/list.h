#include <algorithm>
#include <cstddef>
#include <new>

template<size_t N>
class alignas(std::max_align_t) StackStorage {
private:
    char memory_[N]{};
    size_t current_ = 0;

public:
    StackStorage() = default;

    void *allocate(size_t size, size_t align) {
        size_t next = (current_ + align - 1) / align * align;
        if (N < next + size) {
            throw std::bad_alloc();
        }
        current_ = next + size;
        return memory_ + next;
    }
};


template<typename T, size_t N>
class StackAllocator {
private:
    StackStorage<N> &storage_;
public:
    template<typename T1, typename T2, size_t N1, size_t N2>
    friend bool operator==(const StackAllocator<T1, N1> &lhs, const StackAllocator<T2, N2> &rhs) noexcept;

    template<typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    using value_type = T;

    StackAllocator(StackStorage<N> &stack) noexcept: storage_(stack) {}

    StackStorage<N> &stack() const {
        return storage_;
    }

    template<typename U>
    StackAllocator(const StackAllocator<U, N> &other) noexcept: storage_(other.stack()) {}

    StackAllocator &operator=(const StackAllocator &other) noexcept {
        storage_ = other.storage_;
        return *this;
    }

    T *allocate(size_t n) {
        return reinterpret_cast<T *>(storage_.allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T *, size_t) {}
};


template<typename T1, typename T2, size_t N1, size_t N2>
bool operator==(const StackAllocator<T1, N1> &lhs, const StackAllocator<T2, N2> &rhs) noexcept {
    return &lhs.storage_ == &rhs.storage_;
}

template<typename T, typename Alloc = std::allocator<T>>
class List {
private:
    struct BaseNode {
        BaseNode *prev;
        BaseNode *next;

        BaseNode(BaseNode *prev, BaseNode *next) : prev(prev), next(next) {};

        BaseNode() : prev(this), next(this) {}


        void makeSwap(BaseNode &other) {
            if (other.next == &other) {
                prev = this, next = this;
                return;
            }
            prev = other.prev, next = other.next;
            prev->next = this, next->prev = this;
        }
    };

    struct Node : BaseNode {
        T value;


        Node(T &&value, BaseNode *prev = nullptr, BaseNode *next = nullptr) : BaseNode(prev, next),
                                                                              value(std::move(value)) {}

        Node(const T &value, BaseNode *prev = nullptr, BaseNode *next = nullptr) : BaseNode(prev, next),
                                                                                   value(value) {}

        template<typename... Args>
        Node(Args &&... args): BaseNode(nullptr, nullptr),
                               value(std::forward<Args>(args)...) {};

        Node() : value() {}
    };

    using NodeAllocTraits = std::allocator_traits<typename std::allocator_traits<Alloc>::template rebind_alloc<Node>>;
    typename std::allocator_traits<Alloc>::template rebind_alloc<Node> alloc_;
    size_t sz_ = 0;
    BaseNode fake_node_;

public:

    template<bool IsConst>
    class CommonIter {
    private:
        BaseNode *node_;


    public:
        using difference_type = long long;
        using value_type = typename std::conditional<IsConst, const T, T>::type;
        using pointer = typename std::conditional<IsConst, const T *, T *>::type;
        using reference = typename std::conditional<IsConst, const T &, T &>::type;
        using iterator_category = std::bidirectional_iterator_tag;

        CommonIter() = default;

        explicit CommonIter(const BaseNode *node) : node_(const_cast<BaseNode *>(node)) {};


        CommonIter(const CommonIter &other) : node_(other.node_) {};

        operator CommonIter<true>() const {
            return CommonIter<true>(node_);
        }

        CommonIter &operator=(const CommonIter &other) = default;

        reference operator*() {
            return static_cast<Node *>(node_)->value;
        }

        friend bool operator==(const CommonIter &first, const CommonIter &second) {
            return first.node_ == second.node_;
        }

        friend bool operator!=(const CommonIter &first, const CommonIter &second) {
            return !(first == second);
        }

        pointer operator->() {
            return &(static_cast<Node *>(node_)->value);
        }

        CommonIter &operator++() {
            node_ = node_->next;
            return *this;
        }

        CommonIter &operator--() {
            node_ = node_->prev;
            return *this;
        }

        CommonIter operator++(int) {
            CommonIter copy = *this;
            node_ = node_->next;
            return copy;
        }

        CommonIter operator--(int) {
            CommonIter copy = *this;
            node_ = node_->prev;
            return copy;
        }

        friend class List;
    };

    using value_type = T;
    using iterator = CommonIter<false>;
    using const_iterator = CommonIter<true>;
    using reverse_iterator = std::reverse_iterator<CommonIter<false>>;
    using const_reverse_iterator = std::reverse_iterator<CommonIter<true>>;

    iterator begin() {
        return iterator(fake_node_.next);
    }

    const_iterator begin() const {
        return const_iterator(fake_node_.next);
    }

    iterator end() {
        return iterator(&fake_node_);
    }

    const_iterator end() const {
        return const_iterator(&fake_node_);
    }

    const_iterator cbegin() const {
        return const_iterator(fake_node_.next);
    }

    const_iterator cend() const {
        return const_iterator(&fake_node_);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }


    List() = default;

    List(const Alloc &alloc) : alloc_(alloc) {};

    List(size_t n, const Alloc &alloc = Alloc()) : alloc_(alloc) {
        try {
            while (n--) {
                emplace_(&fake_node_);
            }
        } catch (...) {
            clear();
            std::rethrow_exception(std::current_exception());
        }
    }

    List(size_t n, const T &val, const Alloc &alloc = Alloc()) : alloc_(alloc) {
        try {
            while (n--) {
                emplace_(&fake_node_, val);
            }
        } catch (...) {
            clear();
            std::rethrow_exception(std::current_exception());
        }
    }

    void swap(List &other) {
        std::swap(sz_, other.sz_);
        std::swap(fake_node_, other.fake_node_);
        std::swap(fake_node_.prev->next, other.fake_node_.prev->next);
        std::swap(fake_node_.next->prev, other.fake_node_.next->prev);
        std::swap(alloc_, other.alloc_);
    }

    List(const List &other) : alloc_(NodeAllocTraits::select_on_container_copy_construction(other.alloc_)) {
        try {
            for (auto &el: other) {
                push_back(el);
            }
        } catch (...) {
            clear();
            std::rethrow_exception(std::current_exception());
        }
    }

    List(List &&other) {
        if (this == &other) {
            return;
        }
        fake_node_.makeSwap(other.fake_node_);
        other.fake_node_.prev = &other.fake_node_, other.fake_node_.next = &other.fake_node_;
        sz_ = other.sz_;
        other.sz_ = 0;
    }

    List &operator=(const List &other) {
        List copy(NodeAllocTraits::propagate_on_container_copy_assignment::value ? other.alloc_ : alloc_);
        for (auto &el: other) {
            copy.push_back(el);
        }
        swap(copy);
        return *this;
    }

    List &operator=(List &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        clear();
        if (NodeAllocTraits::propagate_on_container_move_assignment::value) {
            alloc_ = std::move(other.alloc_);
        }
        fake_node_.makeSwap(other.fake_node_);
        other.fake_node_ = BaseNode();
        sz_ = other.sz_;
        other.sz_ = 0;
        return *this;
    }

    ~List() {
        clear();
    }

    void connect(BaseNode *first, BaseNode *second) {
        first->next = second;
        second->prev = first;
    }


    template<typename U = T>
    void push_back(U &&val) {
        emplace_(&fake_node_, std::forward<U>(val));
    }


    template<typename U = T>
    void push_front(U &&val) {
        emplace_(fake_node_.next, std::forward<U>(val));
    }

    template<typename... Args>
    void emplace_front(Args &&...args) {
        emplace_(fake_node_.next, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void emplace_back(Args &&...args) {
        emplace_(&fake_node_, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void emplace(const_iterator it, Args &&...args) {
        emplace_(it.node_, std::forward<Args>(args)...);
    }

    void pop_back() {
        erase_(fake_node_.prev);
    }

    void pop_front() {
        erase_(fake_node_.next);
    }


    iterator insert(const_iterator it, const T &val) {
        return emplace_(it.node_, val);
    }

    void erase(const_iterator it) {
        erase_(it.node_);
    }

    void erase(const_iterator left, const_iterator right) {
        const_iterator it;
        while ((it = left++) != right) {
            erase(it);
        }
    }

    size_t size() const {
        return sz_;
    }

    Alloc get_allocator() const {
        return alloc_;
    }

    void clear() {
        while (sz_ > 0) {
            pop_back();
        }
    }

    T &front() {
        return static_cast<Node *>(fake_node_.next)->value;
    }

    const T &front() const {
        return static_cast<Node *>(fake_node_.next)->value;
    }

    T &back() {
        return static_cast<Node *>(fake_node_.prev)->value;
    }

    const T &back() const {
        return static_cast<Node *>(fake_node_.prev)->value;
    }

private:
    template<typename... Args>
    iterator emplace_(BaseNode *node, Args &&...args) {
        Node *new_node = NodeAllocTraits::allocate(alloc_, 1);
        BaseNode *prev = node->prev;
        try {
            NodeAllocTraits::construct(alloc_, new_node, std::forward<Args>(args)...);
            connect(prev, new_node);
            connect(new_node, node);
            ++sz_;
        } catch (...) {
            NodeAllocTraits::deallocate(alloc_, new_node, 1);
            std::rethrow_exception(std::current_exception());
        }
        return iterator(new_node);
    }

    void erase_(BaseNode *node) {
        connect(node->prev, node->next);
        NodeAllocTraits::destroy(alloc_, static_cast<Node *>(node));
        NodeAllocTraits::deallocate(alloc_, static_cast<Node *>(node), 1);
        --sz_;
    }
};
