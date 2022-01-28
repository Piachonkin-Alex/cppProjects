#pragma once

template <class T>
class Vector {

public:
    Vector() : root_(std::make_shared<Node>()), sz_(0) {
    }

    explicit Vector(size_t count, const T& value = T()) : Vector() {
        sz_ = count;
        for (size_t i = 0; i < count; ++i) {
            InitValue(i, value, root_);
        }
    }

    template <typename Iterator>
    Vector(Iterator first, Iterator last) : Vector() {
        while (first != last) {
            InitValue(sz_++, *(first++), root_);
        }
    }

    Vector(std::initializer_list<T> l) : Vector() {
        for (auto& elem : l) {
            InitValue(sz_++, elem, root_);
        }
    }

    Vector Set(size_t index, const T& value) {
        auto new_root = UpdateValue(index, value, root_);
        return Vector(new_root, sz_);
    }

    const T& Get(size_t index) const {
        return GetValue(index, root_);
    }

    Vector PushBack(const T& value) {
        auto new_root = UpdateValue(sz_, value, root_);
        return Vector(new_root, sz_ + 1);
    }

    Vector PopBack() {
        return Vector(root_, sz_ - 1);
    }

    size_t Size() const {
        return sz_;
    }
    static const size_t kLeafSize = 1 << 5;

private:
    struct Node {
        std::vector<std::shared_ptr<Node>> childrens;
        T value;

        Node() : childrens(kLeafSize) {
        }

        Node(T&& value) : value(std::move(value)) {
        }
        Node(const T& value) : value(value) {
        }
        Node(const Node& node) : value(value) {
            for (auto elem : node.childrens) {
                childrens.push_back(elem);
            }
        }

        Node(const std::shared_ptr<Node>& other) : value(other->value) {
            for (auto elem : other->childrens) {
                childrens.push_back(elem);
            }
        }

        void SetValue(const T& val) {
            value = val;
        }

        void SetValue(T&& val) {
            value = std::move(val);
        }
        std::shared_ptr<Node> GetNodeByIndex(size_t index) {
            return childrens[index] ? childrens[index]
                                    : childrens[index] = std::make_shared<Node>();
        }
    };

    template <typename U = T>
    std::shared_ptr<Node> SetValue(const std::shared_ptr<Node>& node, U&& value) {
        auto result = std::make_shared<Node>(node);
        result->SetValue(std::forward<U>(value));
        return result;
    }

    template <typename U = T>
    void InitValue(size_t index, U&& val, const std::shared_ptr<Node>& current_node) {
        index == 0 ? (current_node->SetValue(std::forward<U>(val)))
                   : InitValue(index / 32, std::forward<U>(val),
                               current_node->GetNodeByIndex(index % kLeafSize));
    }

    template <typename U = T>
    std::shared_ptr<Node> UpdateValue(size_t index, U&& val,
                                      const std::shared_ptr<Node>& current_node) {
        if (index == 0) {
            return SetValue(current_node, std::forward<U>(val));
        }
        auto new_node = UpdateValue(index / kLeafSize, std::forward<U>(val),
                                    current_node->GetNodeByIndex(index % kLeafSize));
        auto new_current_node = std::make_shared<Node>(current_node);
        new_current_node->childrens[index % kLeafSize] = new_node;
        return new_current_node;
    }

    const T& GetValue(size_t index, const std::shared_ptr<Node>& current_node) const {
        return index == 0
               ? current_node->value
               : GetValue(index / kLeafSize, current_node->GetNodeByIndex(index % kLeafSize));
    }

    Vector(const std::shared_ptr<Node> root, size_t sz) : root_(root), sz_(sz) {
    }
    std::shared_ptr<Node> root_;
    size_t sz_;
};
