#pragma once

#include <algorithm>


// class to inherit for working with container
class Hook {
public:
    template <class T>
    friend class List;
    Hook() = default;
    bool IsLinked() const {
        return next_ != nullptr;
    }

    void Unlink() {
        if (next_) {
            next_->prev_ = prev_;
        }
        if (prev_) {
            prev_->next_ = next_;
        }
        next_ = prev_ = nullptr;
    }

    template <typename T>
    T* AsItem() {
        return static_cast<T*>(this);
    }

    ~Hook() {
        Unlink();
    }

    Hook(const Hook&) = delete;

private:
    Hook* prev_;
    Hook* next_;

    void LinkBefore(Hook* other) {
        prev_ = other->prev_;
        prev_->next_ = this;
        next_ = other;
        other->prev_ = this;
    }
};

template <typename T>
class IntrusiveList {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator() = default;
        Iterator(Hook* cur, Hook* end) : current_(cur), end_(end) {
        }
        Iterator(const Iterator& other) : current_(other.current_), end_(other.end_) {
        }
        Iterator& operator++() {
            current_ = current_->next_;
            return *this;
        }

        Iterator operator++(int) {
            auto copy = *this;
            current_ = current_->next_;
            return copy;
        }

        T& operator*() const {
            return *(static_cast<T*>(current_));
        }
        T* operator->() const {
            return static_cast<T*>(current_);
        }

        bool operator==(const Iterator& rhs) const {
            return current_ == rhs.current_ && end_ == rhs.end_;
        }
        bool operator!=(const Iterator& rhs) const {
            return !operator==(rhs);
        }

    private:
        Hook* current_;
        Hook* end_;
    };

    IntrusiveList() {
        dummy_.next_ = dummy_.prev_ = &dummy_;
    }
    IntrusiveList(const IntrusiveList&) = delete;
    IntrusiveList(IntrusiveList&& other) {
        dummy_.next_ = dummy_.prev_ = &dummy_;
        InsertEnd(other);
    }

    // must unlink all elements from list
    ~IntrusiveList() {
        UnlinkAll();
    }

    IntrusiveList& operator=(const IntrusiveList&) = delete;
    IntrusiveList& operator=(IntrusiveList&& other) {
        UnlinkAll();
        InsertEnd(other);
        return *this;
    }

    void UnlinkAll() {
        Hook* current = dummy_.next_;
        while (current != &dummy_) {
            Hook* next = current->next_;
            current->Unlink();
            current = next;
        }
    }

    bool IsEmpty() const {
        return dummy_.next_ == &dummy_;
    }
    //  O(n)
    size_t Size() const {
        size_t counter = 0;
        auto cur = dummy_.next_;
        while (cur != &dummy_) {
            ++counter;
            cur = cur->next_;
        }
        return counter;
    }

    void PushBack(T* elem) {
        elem->LinkBefore(&dummy_);
    }
    void PushFront(T* elem) {
        elem->LinkBefore(dummy_.next_);
    }

    T& Front() {
        return *(dummy_.next_->template AsItem<T>());
    }
    const T& Front() const {
        return *(dummy_.next_->template AsItem<T>());
    }

    T& Back() {
        return *(dummy_.prev_->template AsItem<T>());
    }
    const T& Back() const {
        return *(dummy_.prev_->template AsItem<T>());
    }

    void PopBack() {
        if (IsEmpty()) {
            throw 1;
        }
        Hook* back = dummy_.prev_;
        back->Unlink();
    }
    void PopFront() {
        if (IsEmpty()) {
            throw 1;
        }
        Hook* front = dummy_.next_;
        front->Unlink();
    }

    Iterator Begin() {
        return Iterator(dummy_.next_, &dummy_);
    }
    Iterator End() {
        return Iterator(&dummy_, &dummy_);
    }

    Iterator IteratorTo(T* element) {
        return Iterator(element, &dummy_);
    }

private:
    void InsertEnd(IntrusiveList& other) noexcept {
        if (other.IsEmpty()) {
            return;
        }
        Hook* back = dummy_.prev_;
        Hook* other_front = other.dummy_.next_;
        Hook* other_back = other.dummy_.prev_;

        other_back->next_ = &dummy_;
        other_front->prev_ = dummy_.prev_;
        dummy_.prev_ = other_back;
        back->next_ = other_front;
        other.dummy_.next_ = other.dummy_.prev_ = &other.dummy_;
    }

    Hook dummy_;
};

template <typename T>
typename IntrusiveList<T>::Iterator begin(IntrusiveList<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename IntrusiveList<T>::Iterator end(IntrusiveList<T>& list) {  // NOLINT
    return list.End();
}
