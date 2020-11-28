#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>

// ООО "Новые Железные Дороги" поручило вам разработать систему бронирования билетов на новый маршрут
// поезда дальнего следования. Маршрут проходит через N станций, занумерованных от 0 до N-1.
// Вместимость поезда ограничена.В систему бронирования последовательно приходят запросы от пассажиров
// с указанием номера начальной и конечной станции, а также количество билетов, которые пассажир хочет приобрести.
//Требуемая скорость обработки каждого запроса - O(logn) .

// Первая строка содержит число N –– количество станций (1 ≤ N ≤ 50 000).

//Вторая строка содержит начальное количество проданных билетов.
//В примере продано по одному билету на перегонах 0->1 и 4->5.

//В третьей строке указана вместимость поезда. В четвёртой — количество запросов на бронирование.
//Далее идут непосредственно запросы в виде: номер начальной станции, номер конечной станции, количество запрашиваемых билетов.

//Необходимо вывести номера запросов, которые не удалось выполнить из-за отсутствия свободных мест.
using std::vector;
using std::pair;

class segmentTree { // дерево отрезков
    vector<int> arr; // массив значений
    vector<int> add; // массив для отлож операций
public:
    segmentTree() = default;

    void getSize(int n) { //задаем изначальный размер дерева
        arr = vector<int>(4 * n, -1);
        add = vector<int>(4 * n, 0);
    }

    void build(vector<int> &data, int v, int tl, int tr) {
        if (tl == tr) { // построение
            arr[v] = data[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(data, v * 2, tl, tm);
            build(data, v * 2 + 1, tm + 1, tr);
            arr[v] = std::max(arr[v * 2], arr[v * 2 + 1]);
        }
    }

    void push(int v) { // функция проталкивания
        if (add[v] != 0) {

            if (2 * v + 1 < add.size()) {
                add[2 * v + 1] += add[v];
            }  // проверка на границы
            if (2 * v < add.size()) {
                add[2 * v] += add[v];
            }

            arr[v] += add[v];
            add[v] = 0;
        }
    }

    int maximum(int v, int tl, int tr, int left, int right) { // поиск максимума
        if (tl > tr || right < tl || left > tr) {
            return -1e9; // если вырожденный случай
        }
        push(v);
        if (tl >= left && tr <= right) {
            return arr[v]; // если попали куда надо
        }

        int tm = (tl + tr) / 2;
        return std::max(maximum(v * 2, tl, tm, left, right), maximum(v * 2 + 1, tm + 1, tr, left, right));
    }


    void update(int v, int tl, int tr, int left, int right, int plus) {
        if (tl > tr || right < tl || left > tr) {
            return; // вырожденный
        }

        if (tl >= left && tr <= right) {
            add[v] += plus; // попали куда надо
            return;
        }
        push(v); // проталкивание
        int tm = (tl + tr) / 2;

        update(v * 2, tl, tm, left, right, plus);
        update(v * 2 + 1, tm + 1, tr, left, right, plus);

        arr[v] = std::max(arr[v * 2] + add[v * 2], arr[v * 2 + 1] + add[v * 2 + 1]);
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int n;
    std::cin >> n;
    vector<int> init_data(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        int numb;
        std::cin >> numb;
        init_data[i] = numb;
    }

    segmentTree our_tree;
    our_tree.getSize(n);
    our_tree.build(init_data, 1, 0, n - 1);
    int cap, m;
    std::cin >> cap >> m;


    for (int i = 0; i < m; ++i) {
        int tourists, l, r;
        std::cin >> l >> r >> tourists;
        int now = our_tree.maximum(1, 0, n - 1, l, r - 1);
        if (now + tourists > cap) { // проверка того, сможем ли мы посадить на данный маршрут еще
            std::cout << i << ' ';
        } else {
            our_tree.update(1, 0, n - 1, l, r - 1, tourists); // если можем -- обновили
        }
    }
}
