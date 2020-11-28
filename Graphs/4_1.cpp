#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>
// Дано число N и последовательность из N целых чисел. Найти вторую порядковую статистику на заданных диапазонах.
// Для решения задачи используйте структуру данных Sparse Table. Требуемое время обработки каждого диапазона O(1).
// Время подготовки структуры данных O(nlogn).

// В первой строке заданы 2 числа: размер последовательности N и количество диапазонов M.

//Следующие N целых чисел задают последовательность. Далее вводятся M пар чисел - границ диапазонов.

//Для каждого из M диапазонов напечатать элемент последовательности - 2ю порядковую статистику. По одному числу в строке.

//Гарантируется, что каждый диапазон содержит как минимум 2 элемента.
using std::vector;
using std::pair;

const int max_log = 30;
const int INF = INT32_MAX;

class sparseTable { // в этом контесте у меня проснулось желание все делать через классы. Надеюсь получите удовольствие
//Поглядим)
private:
    vector<vector<pair<int, int>>> table; // сама таблица. хранить будем {значение, индекс}
    //Целый массив ради одной математической функции, ну ладно.
    vector<int> prec_log; // таблица логарифмов от чиселок, чтобы понимать какую клеточку в запросе чекать
    int size;
public:
    sparseTable() = default;

    void init(vector<int> &data) {
        size = data.size();
        for (int i = 0; i < size; ++i) {
            table.push_back(vector<pair<int, int>>(max_log));
        }
        prec_log = vector<int>(size + 1, 0);
        for (int i = 2; i <= size; ++i) {
            prec_log[i] = prec_log[i >> 1] + 1; // предподсчет логарифмов
        }
        for (int i = 0; i < size; ++i) {
            table[i][0] = {data[i], i}; // заполнение 1 слоя
        }
        for (int j = 1; (1 << j) <= size; ++j) {
            for (int i = 0; i < size; ++i) { // остальное заполнение
                if (i + (1 << j) > size) {
                    break;
                }
                table[i][j] = std::min(table[i][j - 1], table[i + (1 << (j - 1))][j - 1]);
            }
        }
    }

    pair<int, int> findMin(int l, int r) { // поиск минимума.
        if (r < l) {
            return {INF, -1};
        }
        int s = prec_log[r - l + 1];
        return std::min(table[l][s], table[r - (1 << s) + 1][s]);
    }

    int findSecond(int l, int r) { // поиск 2 статы. находим минимум, ищем минимум справа и слево от него, берем
        // минимум из них
        pair<int, int> min = findMin(l, r);
        pair<int, int> min_left = findMin(l, min.second - 1);
        pair<int, int> min_right = findMin(min.second + 1, r);
        return std::min(min_left.first, min_right.first);
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int n, m;
    std::cin >> n >> m;
    vector<int> data;
    for (int i = 0; i < n; ++i) {
        int numb;
        std::cin >> numb;  // ввод
        data.push_back(numb);
    }
    sparseTable tab;
    tab.init(data);
    vector<pair<int, int>> ranges;
    for (int i = 0; i < m; ++i) {
        int l, r;
        std::cin >> l >> r;
        int mini = tab.findSecond(l - 1, r - 1);
        std::cout << mini << '\n';
    }

}
