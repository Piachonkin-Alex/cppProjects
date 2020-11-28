#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <utility>


template<typename T>
class List {
private:
    class Node { // часть листа, где лежат указатели на прошлый и след элемент и указатель на значение
    public:
        T *value;
        Node *pre;
        Node *next;

        Node() = default;

        Node(Node *pre, Node *next) : value(NULL), pre(pre), next(next) {};

        Node(Node *pre, Node *next, const T &val) : pre(pre), next(next) { // копирования
            value = new T(val);
        }

        Node(Node *pre, Node *next, T &&val) : pre(pre), next(next) { // перемещения
            value = new T(std::move(val));
        }
    };

    Node *insert_(Node *node, const T &value) { // инсер с копированием
        ++size_;
        Node *construct_node = new Node(nullptr, node, value); // создали с указателем в качестве след. эл-та на node
        if (node->pre == nullptr) { // проверка того, что вставляем в начало, тогда first меняем
            first_node->pre = construct_node;
            first_node = construct_node;
            return construct_node;
        } // иначе просто вставляем между данной и предыдущей
        construct_node->pre = node->pre;
        construct_node->next = node;
        node->pre->next = construct_node;
        node->pre = construct_node;
        return construct_node;
    }

    Node *insert_(Node *node, T &&value) { // тоже самое с &&
        ++size_;
        Node *construct_node = new Node(nullptr, node, std::forward<T>(value));
        if (node->pre == nullptr) {
            first_node->pre = construct_node;
            first_node = construct_node;
            return construct_node;
        }
        construct_node->pre = node->pre;
        construct_node->next = node;
        node->pre->next = construct_node;
        node->pre = construct_node;
        return construct_node;

    }

    template<typename... Args>
    void emplace_(Node *node, Args &&... args) { // в принципе как и insert, только надо создавать руками значение в
        // новой ноде. Также, возвр войд
        ++size_;
        Node *construct_node = new Node(nullptr, node);
        construct_node->value = new T(std::forward<Args>(args)...);
        //Последующий код я вижу уже в 3-й раз, было бы неплохо спрятать его в отдельной private функции
        if (node->pre == nullptr) {
            first_node->pre = construct_node;
            first_node = construct_node;
            return;
        }
        construct_node->pre = node->pre;
        construct_node->next = node;
        node->pre->next = construct_node;
        node->pre = construct_node;
    }

    Node *erase(Node *node) {
        if (size_ == 0) { // хотите -- ловите эксепшн на удаление из пустого
            throw 1;
        }
        --size_;
        if (node == first_node) { // если удаляем по first_node -- надо ее обновить
            Node *new_tail = first_node->next;
            new_tail->pre = nullptr;
            delete first_node->value; // память
            delete first_node;
            first_node = new_tail;
            return first_node;
        }
        Node *ret_node = node->next;
        ret_node->pre = node->pre;
        node->pre->next = ret_node;
        delete node->value;
        delete node;
        return ret_node; // возвр следующую ноду
    }


    size_t size_;
    Node *last_node; // поля наши. размер, последняя нода, которая всегда всегда фиктивная, но для понимания ее пишу
    Node *first_node;
    Node *fict_node;

public:
    typedef T value_type;
    typedef size_t difference_type;
    typedef T *pointer;
    typedef T &reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    List() : size_(0) {
        fict_node = new Node(nullptr, nullptr); // пустой лист
        last_node = first_node = fict_node;
    };

    List(size_t count, const T &value = T()) { // лист с нач значениями
        size_ = 0;
        fict_node = new Node(nullptr, nullptr);
        last_node = first_node = fict_node;
        if (count != 0) {
            for (size_t i = 0; i < count; ++i) {
                push_back(value);
            }
        }
    }

    List(const List &other) { // конструктор копирования
        size_ = 0;
        Node *current = other.first_node;
        fict_node = new Node(nullptr, nullptr);
        last_node = first_node = fict_node;
        while (current != other.fict_node) {
            push_back(*current->value);
            current = current->next;
        }
    }

    List(List &&other) { // перемещения
        size_ = std::move(other.size_);
        first_node = other.first_node;
        last_node = other.last_node;
        fict_node = other.fict_node;
        Node *other_fict_node = new Node(nullptr, nullptr); // тут важно, нужно оставить other рабочим
        other.last_node = other.first_node = other.fict_node = other_fict_node;
        other.size_ = 0;
    }

    List &operator=(const List &other) { // оператор присваивания
        size_ = 0;
        Node *current = other.first_node;
        fict_node = new Node(nullptr, nullptr);
        last_node = first_node = fict_node;
        while (current != other.fict_node) {
            push_back(*current->value);
            current = current->next;
        }
        return *this;
    }

    List &operator=(List &&other) { // перемещения
        size_ = other.size_;
        first_node = other.first_node;
        last_node = other.last_node;
        fict_node = other.fict_node;
        Node *other_fict_node = new Node(nullptr, nullptr);
        other.last_node = other.first_node = other.fict_node = other_fict_node;
        other.size_ = 0;
        return *this;
    }


    ~List() { // деструктор
        if (size_ == 0) { // если размер 0 -- ничего не удаляем кроме fict_node в конце
            delete fict_node;
        } else {
            Node *current = first_node; // fixed
            Node *next; // иначе итерируемся до fict_node и все удаляем
            while (current != fict_node) {
                next = current->next;
                delete current->value;
                delete current;
                current = next;
            }
            delete fict_node; // удаляем fict_node
        };
    }

    void push_back(const T &value) { //  == void insert last_node
        insert_(last_node, value);
    }

    void push_back(T &&value) {
        insert_(fict_node, std::forward<T>(value));
    }

    void push_front(const T &value) { // == void insert first_node
        insert_(first_node, value);
    }

    void push_front(T &&value) {
        insert_(first_node, std::forward<T>(value));
    }

    template<typename... Args>
    void emplace_back(Args &&... args) {
        emplace_(fict_node, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void emplace_front(Args &&... args) {
        emplace_(first_node, std::forward<Args>(args)...);
    }

    void pop_front() { // void erase first
        erase(first_node);
    }

    void pop_back() { // void erase last->pre
        erase(last_node->pre);
    }

    size_t size() {
        return size_;
    }

    bool empty() {
        return (size_ == 0);
    }

    void clear() { // как деструктор только без удаления fict_node
        // важно: деструктор тут нельзя юзать, иначе ub при 2 вызове clear (на лекции)
        size_ = 0;
        Node *current = first_node;
        Node *next;
        while (current != nullptr) {
            next = current->next;
            delete current->value;
            delete current;
            current = next;
        }
        fict_node = new Node(nullptr, nullptr);
        first_node = last_node = fict_node;
    }

    T &front() {
        return *first_node->value;
    }

    T &back() {
        return *last_node->pre->value;
    }

    class iterator {
    public:
        Node *pointer_; // одно поле -- указатель

    public:
        typedef T value_type;
        typedef size_t difference_type;
        typedef T *pointer;
        typedef T &reference; // чтобы ботал reverse
        typedef std::bidirectional_iterator_tag iterator_category;

        explicit iterator(Node *pointr) : pointer_(pointr) {};

        iterator() = default;

        iterator(const iterator &other) = default;

        iterator &operator=(const iterator &other) { // присваивание итератора
            pointer_ = other.pointer_;
            return *this;
        };

        iterator operator++(int) { // постфикс
            iterator copy = *this;
            pointer_ = pointer_->next;
            return copy;
        }

        iterator &operator++() { // префикс
            pointer_ = pointer_->next;
            return *this;
        }

        iterator operator--(int) {
            iterator copy = *this;
            pointer_ = pointer_->pre;
            return copy;
        }

        iterator &operator--() {
            pointer_ = pointer_->pre;
            return *this;
        }


        T & operator*() {
            return *pointer_->value;
        }

        bool operator==(const iterator &other){// И тут
            return (pointer_ == other.pointer_);
        }

        bool operator!=(const iterator &other){// И тут
            return !operator==(other);
        }
    };

    class const_iterator { // с констом тоже самое
    public:
        Node *pointer_;

    public:
        typedef T value_type;
        typedef size_t difference_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        explicit const_iterator(Node *pointr) : pointer_(pointr) {};

        const_iterator() = default;

        const_iterator(const const_iterator &other) = default;

        const_iterator(iterator it) {
            pointer_ = it.pointer_;
        }

        const_iterator &operator=(const const_iterator &other) = default;

        const_iterator operator++(int) {
            const_iterator copy = *this;
            pointer_ = pointer_->next;
            return copy;
        }

        const_iterator &operator++() {
            pointer_ = pointer_->next;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator copy = *this;
            pointer_ = pointer_->pre;
            return copy;
        }

        const_iterator &operator--() {
            pointer_ = pointer_->pre;
            return pointer_;
        }


        const T &operator*() const {// Почему он возвращает обычное значение? const_iterator должен всегда возвращать const
            return *pointer_->value;
        }

        bool operator==(const const_iterator &other) const {
            return (pointer_ == other.pointer_);
        }

        bool operator!=(const const_iterator &other) const {
            return !operator==(other);
        }
    };


    iterator begin() { // на первую ноду
        return iterator(first_node);
    }
    const_iterator begin() const {
        return const_iterator(first_node);
    }


    //требуется так же наличие отдельно const_iterator begin() const. У тебя const версия контейнера не должна ни при каких условиях возвращать обычный итератор
    iterator end() { // на последнюю (она фикт)
        return iterator(last_node);
    }
    const_iterator end() const {
        return const_iterator(last_node);
    }
    //аналогично
    const_iterator cbegin() const {// const модификатор где?
        return const_iterator(first_node);
    }

    const_iterator cend() const {// аналогично
        return const_iterator(last_node);
    }

    std::reverse_iterator<iterator> rbegin() { // от фиктивной а один назад
        return std::reverse_iterator<iterator>(iterator(last_node->pre));
    }

    std::reverse_iterator<iterator> rend() { // после первой -- то есть нулл птр
        return std::reverse_iterator<iterator>(iterator(nullptr));
    }

    const_iterator insert(const_iterator it, const T &value) { // все инсерты и эрейзы работают просто через
        // данные подчеркнутые ф-и, так как они возвр. Node*, что есть поле итератора
        Node *ret_node = insert_(it.pointer_, value);
        return iterator(ret_node);
    }

    const_iterator insert(const_iterator it, T &&value) {
        Node *ret_node = insert_(it.pointer_, std::forward<T>(value));
        return const_iterator(ret_node);
    }

    iterator insert(iterator it, const T &value) {
        Node *ret_node = insert_(it.pointer_, value);
        return iterator(ret_node);
    }

    iterator insert(iterator it, T &&value) {
        Node *ret_node = insert_(it.pointer_, std::forward<T>(value));
        return iterator(ret_node);
    }

    template<typename InputIter>
    const_iterator
    insert(const_iterator it, InputIter first, InputIter last) { // тут просто итерируемся по данному итеру до конца
        InputIter current = first;
        Node *ret_node;
        while (current != last) {
            ret_node = insert(it, *current);
            ++current;
        }
        return ret_node;
    }

    template<typename InputIter>
    iterator insert(iterator it, InputIter first, InputIter last) {
        InputIter current = first;
        Node *ret_node;
        while (current != last) {
            ret_node = insert(it, *current);
            ++current;
        }
        return ret_node;
    }


    template<typename... Args>
    void emplace(const_iterator it, Args &&... args) {
        emplace_(it.pointer_, std::forward<Args>(args)...);
    }

    iterator erase(iterator it) {
        Node *res = erase(it.pointer_);
        return iterator(res);
    }

    const_iterator erase(const_iterator it) {
        Node *res = erase(it.pointer_);
        return const_iterator(res);
    }

    iterator erase(iterator begin, iterator end) { // я долго думал, может стоило все-таки удаление написать
        // тут за O(1), но нам нужно все равно удалять память за длину удаляемого куска.
        // поэтому по асимптотике трабл нету, но по времени наверное раза в 3 медленее работать будет.
        iterator next;
        Node *res;
        while (begin != end) {
            next = begin;
            ++next;
            res = erase(begin.pointer_);
            begin = next;
        }
        return iterator(res);
    }

    const_iterator erase(const_iterator begin, const_iterator end) {
        const_iterator next;
        Node *res;
        while (begin != end) {
            next = begin;
            ++next;
            res = erase(begin.pointer_);
            begin = next;
        }
        return const_iterator(res);
    }

    void unique() {
        if (first_node == fict_node) {
            return; // если пустой
        }
        Node *current = first_node;
        Node *next = current->next;
        while (next != fict_node) { // ну далее просто идем до конца и проверяем что next == current
            if (*current->value == *next->value) {
                erase(next);
                next = current->next;
            } else {
                current = next;
                next = current->next;
            }
        }
    }

    void reverse() {
        if (size_ == 0) {
            return; // пустой
        }
        Node *current = last_node->pre;
        Node *after;
        Node *new_first = last_node->pre; // идем просто с конца до нуля. фактически создаем new_fist  и new_last
        // и делаем новый список указателей. в конце переустанавливаем first = new_first и меняем fict_node
        Node *temp = new_first->pre;
        new_first->pre = nullptr;
        new_first->next = temp;
        Node *new_last;
        while (true) {
            if (current->next == nullptr) {
                new_last = current;
                break;
            }

            after = current->next;
            temp = after->pre;
            after->pre = current;
            after->next = temp;
            current = after;
        }
        first_node = new_first;
        fict_node->pre = new_last;
        fict_node->next = nullptr;
        new_last->next = fict_node;
        last_node = fict_node;
    }
};

template<typename T>
bool operator==(const typename List<T>::const_iterator &it, const typename List<T>::iterator &iter) {
    return it.pointer_ = iter.pointer_; // сравнение конст-итератора и просто итератора
}

template<typename T>
bool operator==(const typename List<T>::iterator &it, const typename List<T>::const_iterator &iter) {
    return it.pointer_ = iter.pointer_; // сравнение итератора и конст итератора
}
