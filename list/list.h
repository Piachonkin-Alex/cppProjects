#pragma once

template<typename T>
class List {
private:
    struct Node {  // часть листа, где лежат указатели на прошлый и след элемент и указатель на
        // значение
    public:
        T *value;
        Node *pre;
        Node *next;

        Node() = default;

        Node(Node *pre, Node *next) : value(nullptr), pre(pre), next(next) {};

        Node(Node *pre, Node *next, const T &val) : pre(pre), next(next) {  // копирования
            value = new T(val);
        }

        Node(Node *pre, Node *next, T &&val) : pre(pre), next(next) {  // перемещения
            value = new T(std::move(val));
        }
    };

    template<typename... Args>
    void Emplace(Node *node, Args &&... args) {
        ++size_;
        Node *construct_node = new Node(nullptr, node);
        construct_node->value = new T(std::forward<Args>(args)...);
        if (node->pre == nullptr) {
            first_->pre = construct_node;
            first_ = construct_node;
            return;
        }
        construct_node->pre = node->pre;
        construct_node->next = node;
        node->pre->next = construct_node;
        node->pre = construct_node;
    }

    template<typename U = T>
    Node *Insert(Node *node, U &&value) {
        Emplace(node, std::forward<U>(value));
        return node->pre;
    }

    Node *Erase(Node *node) {
        if (size_ == 0) {
            throw 1;
        }
        --size_;
        if (node == first_) {
            Node *new_tail = first_->next;
            new_tail->pre = nullptr;
            delete first_->value;
            delete first_;
            first_ = new_tail;
            return first_;
        }
        Node *ret_node = node->next;
        ret_node->pre = node->pre;
        node->pre->next = ret_node;
        delete node->value;
        delete node;
        return ret_node;
    }

    size_t size_;
    Node *first_;
    Node *last_;
    Node *fake_;

public:
    template<bool IsConst>
    class MyIterator {
    private:
        Node *ptr_;

    public:
        using iterator_category = std::bidirectional_iterator_tag;   // NOLINT
        using value_type = std::conditional_t<IsConst, const T, T>;  // NOLINT
        using reference = value_type &;                               // NOLINT
        using pointer = value_type *;                                 // NOLINT
        using difference_type = int;                                 // NOLINT

        explicit MyIterator(Node *ptr) : ptr_(ptr) {
        }

        MyIterator(const MyIterator<false> &other) : ptr_(other.ptr_) {
        }

        MyIterator<IsConst> &operator=(const MyIterator<IsConst> &other) {
            ptr_ = other.ptr_;
            return *this;
        }

        MyIterator<IsConst> &operator++() {
            ptr_ = ptr_->next;
            return *this;
        }

        MyIterator<IsConst> &operator--() {
            ptr_ = ptr_->pre;
            return *this;
        }

        MyIterator<IsConst> operator++(int) {
            MyIterator<IsConst> copy = *this;
            ptr_ = ptr_->next;
            return copy;
        }

        MyIterator<IsConst> operator--(int) {
            MyIterator<IsConst> copy = *this;
            ptr_ = ptr_->pre;
            return copy;
        }

        std::conditional_t<IsConst, const T &, T &> operator*() {
            return *ptr_->value;
        }

        std::conditional_t<IsConst, const T *, T *> operator->() {
            return ptr_->value;
        }

        template<bool IsConst2>
        bool operator==(const MyIterator<IsConst2> &other) const {
            return ptr_ == other.ptr_;
        }

        template<bool IsConst2>
        bool operator!=(const MyIterator<IsConst2> &other) const {
            return !operator==(other);
        }
    };

    using Iterator = MyIterator<false>;
    // using ConstIterator = MyIterator<true>;

    // using ReverseIterator = std::reverse_iterator<MyIterator<false>>;
    // using ConstReverseIterator = std::reverse_iterator<MyIterator<true>>;

    List() : size_(0) {
        fake_ = new Node(nullptr, nullptr);
        last_ = first_ = fake_;
    };

    List(const List &other) {
        size_ = 0;
        Node *current = other.first_;
        fake_ = new Node(nullptr, nullptr);
        last_ = first_ = fake_;
        while (current != other.fake_) {
            PushBack(*current->value);
            current = current->next;
        }
    }

    List(List &&other) {
        size_ = std::move(other.size_);
        first_ = other.first_;
        last_ = other.last_;
        fake_ = other.fake_;
        Node *other_fict_node = new Node(nullptr, nullptr);
        other.last_ = other.first_ = other.fake_ = other_fict_node;
        other.size_ = 0;
    }

    ~List() {
        if (size_ == 0) {
            delete fake_;
            return;
        }
        Node *current = first_;
        Node *next;
        while (current != fake_) {
            next = current->next;
            delete current->value;
            delete current;
            current = next;
        }
        delete fake_;
    }

    void Swap(List &other) {
        std::swap(first_, other.first_);
        std::swap(last_, other.last_);
        std::swap(fake_, other.fake_);
        std::swap(size_, other.size_);
    }

    List &operator=(const List &other) {
        if (this == &other) {
            return *this;
        }
        List copy(other);
        Swap(copy);
        return *this;
    }

    List &operator=(List &&other) {
        Swap(other);
        List empty;
        other.Swap(empty);
        return *this;
    }

    bool IsEmpty() const {
        return size_ == 0;
    }

    size_t Size() const {
        return size_;
    }

    template<typename U = T>
    void PushBack(U &&element) {
        Insert(last_, std::forward<U>(element));
    }

    template<typename U = T>
    void PushFront(U &&element) {
        Insert(first_, std::forward<U>(element));
    }

    T &Front() {
        return *first_->value;
    }

    T &Back() {
        return *last_->pre->value;
    }

    const T &Front() const {
        return *first_->value;
    }

    const T &Back() const {
        return *last_->pre->value;
    }

    void PopBack() {
        Erase(last_->pre);
    }

    void PopFront() {
        Erase(first_);
    }

    Iterator Begin() {
        return Iterator(first_);
    }

    Iterator End() {
        return Iterator(last_);
    }
};

template<typename T>
typename List<T>::Iterator begin(List<T> &list) {  // NOLINT
    return list.Begin();
}

template<typename T>
typename List<T>::Iterator end(List<T> &list) {  // NOLINT
    return list.End();
}

// to be updated