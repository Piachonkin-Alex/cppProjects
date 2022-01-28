#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>

using std::vector;
using std::pair;
using std::string;

//Реализуйте структуру данных “массив строк” на основе декартового дерева по неявному ключу со следующими методами:
//    // Добавление строки в позицию position.\\
//    // Все последующие строки сдвигаются на одну позицию вперед.\\
//    void InsertAt( int position, const std::string& value );\\
//    // Удаление строки из позиции position.\\
//    // Все последующие строки сдвигаются на одну позицию назад.\\
//    void DeleteAt( int position );\\
//    // Получение строки из позиции position.\\
//    std::string GetAt( int position );
//Все методы должны работать за O(log n) в среднем, где n – текущее количество строк в массиве.

// Выведите все строки, запрошенные командами “?”.
class DescartTreap { // декартово дерево
public:
    //Можно было и template реализовать, тем более уже курс ООП пройден
    string val; // значение
    int prior; // приоритет
    DescartTreap *left_son;
    DescartTreap *right_son;
    int size_; // размер поддерева

    DescartTreap(string key, int prior) : val(key), prior(prior), left_son(nullptr), right_son(nullptr), size_(1) {};

    //Небольшая путаница с деструкторами. Ти при вызове delete снова вызовешь эту функцию у left_son и right_son, на самом деле эта функция лишняя и рекурсивное
    //уничтожение может выполнять и деструктор сам по себе, хотя ещё лучше было бы создать static (или для поддерева) функцию, которая сама бы проходила по дереву и по очереди удаляла вершины
    //А деструктор оставить пустым, просто стирать поля
    void destr(DescartTreap *node) {
        if (node != nullptr) {
            destr(node->left_son);
            destr(node->right_son);
        }
        delete node;
    }

    // деструктор
    ~DescartTreap() {
        this->destr(left_son);
        this->destr(right_son);
    }
};

int size(DescartTreap *tree) {
    return (tree == nullptr) ? 0 : tree->size_; // функция размера, поддерживающая nullptr
}

void update(DescartTreap *tree) { // обновление размера
    tree->size_ = size(tree->left_son) + size(tree->right_son) + 1;
}

pair<DescartTreap *, DescartTreap *> split(DescartTreap *tree, int pos) {// сплит
    if (tree == nullptr) {
        return {nullptr, nullptr}; // если пустое, то возвр два пустых
    }
    int left = size(tree->left_son); // количество элементов слево
    if (left >= pos) { // если больше или равно чем нужно, то идем влево
        pair<DescartTreap *, DescartTreap *> t1_t2 = split(tree->left_son, pos);
        tree->left_son = t1_t2.second;
        update(tree);
        return {t1_t2.first, tree};
    } else { // иначе вправо
        pair<DescartTreap *, DescartTreap *> t1_t2 = split(tree->right_son, pos - left - 1);
        tree->right_son = t1_t2.first;
        update(tree);
        return {tree, t1_t2.second};
    }
}

DescartTreap *Merge(DescartTreap *treap_1, DescartTreap *treap_2) {
    if (treap_2 == nullptr) {
        return treap_1; // второе пусто -- вернули 1
    }
    if (treap_1 == nullptr) { // первое пусто -- ввернули 2
        return treap_2;
    } else if (treap_1->prior > treap_2->prior) { //если у первого приоритет  больше -- пошли мержить 2 к правому сыну 1
        treap_1->right_son = Merge(treap_1->right_son, treap_2);
        update(treap_1); // не забываем обновлять размеры поддеревьев
        return treap_1;
    } else {
        treap_2->left_son = Merge(treap_1, treap_2->left_son); // иначе идем мержить 1 к к правому сыну 2
        update(treap_2);
        return treap_2;
    }
}

void insert(DescartTreap *&tree, int position, const string &value) {
    DescartTreap *new_node = new DescartTreap(value, (1 + rand()) % 100000); // новый лист со знач
    if (tree == nullptr) { // пустой случай отдельно
        assert(position == 0);
        tree = new_node;
    } else {
        pair<DescartTreap *, DescartTreap *> t1_t2 = split(tree, position); // сплит по позиции + 2 мержа
        DescartTreap *tmp = Merge(t1_t2.first, new_node);
        tree = Merge(tmp, t1_t2.second);
    }
}

//А отрезанный хвостик так и остался висеть мусором в космосе(пространстве) памяти
void erase(DescartTreap *&tree, int position) {
    pair<DescartTreap *, DescartTreap *> t1_t2 = split(tree, position + 1);
    pair<DescartTreap *, DescartTreap *> t1_t3 = split(t1_t2.first, position); // 2 сплита + мерж
    tree = Merge(t1_t3.first, t1_t2.second);
}

string GetAt(DescartTreap *&tree, int position) {
    if (position == size(tree->left_son)) { // если попали в нужную -- вернули
        return tree->val;
    } else if (position <= size(tree->left_son)) { // иначе смотрим, куда идти
        return GetAt(tree->left_son, position);
    } else {
        return GetAt(tree->right_son, position - size(tree->left_son) - 1);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int n;
    std::cin >> n;
    DescartTreap *tree = nullptr; // начальное дерево -- nullptr
    for (int i = 0; i < n; ++i) {
        char command;
        int pos;
        std::cin >> command;
        std::cin >> pos; // ввод команы и позиции, а дальше по случаям
        if (command == '+') {
            string val;
            std::cin >> val;
            insert(tree, pos, val);
        } else if (command == '-') {
            int del_pos;
            std::cin >> del_pos;
            for (int j = pos; j <= del_pos; ++j) {
                erase(tree, pos);
            }
        } else {
            std::cout << GetAt(tree, pos) << '\n';
        }
    }
}
