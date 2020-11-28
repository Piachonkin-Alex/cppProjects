#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>
// У художника-авангардиста есть полоска разноцветного холста. За один раз он перекрашивает
// некоторый отрезок полоски в некоторый цвет. После каждого перекрашивания специально обученный
// фотограф делает снимок части получившегося творения для музея современного искусства.
// Для правильного экспонирования требуется знать яркость самого темного цвета на выбранном фотографом отрезке.
// Напишите программу для определения яркости самого темного цвета на отрезке.
//Требуемая скорость определения яркости — O(log N).
//Цвет задается тремя числами R, G и B (0 ≤ R, G, B ≤ 255), его яркость = R + G + B.
//Цвет (R1, G1, B1) темнее цвета (R2, G2, B2), если R1 + G1 + B1 < R2 + G2 + B2.

//Первая строка содержит число N –– длина отрезка (координаты [0...N-1]).
//Затем следует N строк по 3 числа –– цвета отрезка.
//Следующая строка содержит число K –– количество перекрашиваний и фотографирований.
//Затем задано K строк, описывающих перекрашивание и фотографирование.
//В каждой такой строке 7 натуральных чисел: C, D, R, G, B, E, F, где [C, D] –– координаты перекрашиваемого отрезка, (R, G, B) –– цвет покраски, [E, F] –– фотографируемый отрезок.

//Требуется вывести K целых чисел. Каждое число –– яркость самого темного цвета на отрезке [E, F] после соответствующего окрашивания.
using std::vector;
using std::pair;

class segmentTree { // cнова дерево отрезков
    vector<int> arr;
    vector<int> change;
public:
    segmentTree() = default;

    void getSize(int n) {
        arr = vector<int>(4 * n, -1); // сами значение
        change = vector<int>(4 * n, -1); // для отложенных операций
    }

    void build(vector<int> &data, int v, int tl, int tr) {
        if (tl == tr) {
            arr[v] = data[tl];
        } else {
            int tm = (tl + tr) / 2;
            build(data, v * 2, tl, tm);
            build(data, v * 2 + 1, tm + 1, tr);
            arr[v] = std::min(arr[v * 2], arr[v * 2 + 1]);
        }
    }

    void push(int v) { // тут немножко поменялось проталкивание. делаем присваивания
        if (change[v] != -1) {

            if (2 * v + 1 < change.size()) {
                change[2 * v + 1] = change[v];
            }
            if (2 * v < change.size()) {
                change[2 * v] = change[v];
            }

            arr[v] = change[v];
            change[v] = -1;
        }
    }

    int minimum(int v, int tl, int tr, int left, int right) {
        if (tl > tr || right < tl || left > tr) {
            return 1e9;
        }
        push(v);
        if (tl >= left && tr <= right) {
            return arr[v];
        }

        int tm = (tl + tr) / 2;
        return std::min(minimum(v * 2, tl, tm, left, right), minimum(v * 2 + 1, tm + 1, tr, left, right));
    }


    void update(int v, int tl, int tr, int left, int right, int val) {
        if (tl > tr || right < tl || left > tr) {
            return;
        }

        if (tl >= left && tr <= right) {
            change[v] = val;
            return;
        }
        push(v);
        
        int tm = (tl + tr) / 2;
        update(v * 2, tl, tm, left, right, val);
        update(v * 2 + 1, tm + 1, tr, left, right, val);
        
        int left_side, right_side;
        left_side = (change[v * 2] == -1) ? (arr[v * 2]) : (change[v * 2]); // тут проверяем есть ли change в детях
        right_side = (change[v * 2 + 1] == -1) ? (arr[v * 2 + 1]) : (change[v * 2 + 1]);
        arr[v] = std::min(left_side, right_side);
        //   arr[v] = std::min(arr[v * 2] + add[v * 2], arr[v * 2 + 1] + add[v * 2 + 1]);
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int n;
    std::cin >> n;
    vector<int> init;
    for (int i = 0; i < n; ++i) { // ввод
        int r, g, b;
        std::cin >> r >> g >> b;
        init.push_back(r + g + b);
    }

    int k;
    std::cin >> k;
    segmentTree our_tree;
    our_tree.getSize(n);
    our_tree.build(init, 1, 0, n - 1);

    for (int i = 0; i < k; ++i) {
        int c, d, r, g, b, e, f;
        std::cin >> c >> d >> r >> g >> b >> e >> f;
        int darkness = r + g + b;
        our_tree.update(1, 0, n - 1, c, d, darkness);
        int dark = our_tree.minimum(1, 0, n - 1, e, f);
        std::cout << dark << ' ';
    }
}
