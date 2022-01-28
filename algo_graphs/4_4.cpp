#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>
 // Задано дерево с корнем, содержащее (1 ≤ n ≤ 100 000) вершин, пронумерованных от 0 до n-1.
//Требуется ответить на m (1 ≤ m ≤ 10 000 000) запросов о наименьшем общем предке для пары вершин.
//Запросы генерируются следующим образом. Заданы числа a1, a2 и числа x, y и z.
//Числа a3, ..., a2m генерируются следующим образом: ai = (x ⋅ ai-2 + y ⋅ ai-1 + z) mod n.
//Первый запрос имеет вид (a1, a2). Если ответ на i-1-й запрос равен v, то i-й запрос имеет вид ((a_2i-1 + v) mod n, a_2i).

//Первая строка содержит два числа: n и m.
//Корень дерева имеет номер 0.
//Вторая строка содержит n-1 целых чисел, i-е из этих чисел равно номеру родителя вершины i.
//Третья строка содержит два целых числа в диапазоне от 0 до n-1: a1 и a2.
//Четвертая строка содержит три целых числа: x, y и z, эти числа неотрицательны и не превосходят 10^9.
using std::vector;
using std::pair;

const int max_log = 18;
const int INF = INT32_MAX;

class sparseTable { // таблица для минимума
private:
    vector<vector<pair<int, int>>> table; // сама таблица. хранит {высота, номер вершины}
    vector<int> prec_log;
    int size;
public:
    sparseTable() = default;

    void init(vector<int> &data, vector<int> &ind_of_vert) {
        size = data.size();
        for (int i = 0; i < size; ++i) {
            table.push_back(vector<pair<int, int>>(max_log));
        }
        prec_log = vector<int>(size + 1, 0);
        for (int i = 2; i <= size; ++i) {
            prec_log[i] = prec_log[i >> 1] + 1;
        }
        for (int i = 0; i < size; ++i) {
            table[i][0] = {ind_of_vert[data[i]], data[i]}; // заполнение первого слоя
        }
        for (int j = 1; (1 << j) <= size; ++j) { // дальнейшее заполнение
            for (int i = 0; i < size; ++i) {
                if (i + (1 << j) > size) {
                    break;
                }
                table[i][j] = std::min(table[i][j - 1], table[i + (1 << (j - 1))][j - 1]);
            }
        }
    }

    pair<int, int> findMin(int l, int r) {
        if (r < l) {
            return findMin(r, l); // если l > r, надо перевернуть вершины
        }
        int s = prec_log[r - l + 1];
        return std::min(table[l][s], table[r - (1 << s) + 1][s]);
    }

    /* int findSecond(int l, int r) {
         pair<int, int> min = findMin(l, r);
         pair<int, int> min_left = findMin(l, min.second - 1);
         pair<int, int> min_right = findMin(min.second + 1, r);
         return std::min(min_left.first, min_right.first);
     }*/
};
//А говорил всё через классы...
void lcaDfs(vector<vector<int>> &graph, vector<int> &dfs_list, vector<char> &used, vector<int> &high,
            vector<int> &first, int v, int h = 1) { // дфс для lca и заполнение высот и первого вхождения
    used[v] = true;
    high[v] = h;
    dfs_list.push_back(v);
    first[v] = dfs_list.size() - 1;
    for (int i = 0; i < graph[v].size(); ++i) {
        if (!used[graph[v][i]]) {
            lcaDfs(graph, dfs_list, used, high, first, graph[v][i], h + 1);
            dfs_list.push_back(v);
        }
    }
}


int main() {
    int n, m;
    std::cin >> n >> m;
    vector<vector<int>> graph(n);
    for (int i = 1; i < n; ++i) {
        int ch;
        std::cin >> ch; // ввод и заполнение графа
        graph[i].push_back(ch);
        graph[ch].push_back(i);
    }

    vector<char> used(n, 0);// ПОчему char, bool ведь компактнее?
    vector<int> first(n, -1);
    vector<int> dfs_list;
    vector<int> high(n); // заполнение dfs листа, высот и первых вхождений
    lcaDfs(graph, dfs_list, used, high, first, 0, 1);

    // clear не гарантирует освобождение памяти и уменьшение capacity вектора, только помечает объекты как готовые к перезаписи (и делает size = 0)
    // Можно после этого делать shrink_to_fit, тогда он точно ужмётся. Либо swap с пустым вектором, тогда этот деструкнется.
    used.clear();
    graph.clear();

    sparseTable our_table;
    our_table.init(dfs_list, high); // заполнение таблицы
    long long a1, a2, x, y, z;
    long long last_ans = 0;
    std::cin >> a1 >> a2 >> x >> y >> z; // ввод
    long long res = 0;

    for (int j = 0; j < m; ++j) {
        pair<int, int> resa = our_table.findMin(first[(a1 + last_ans) % n], first[a2]);
        res += resa.second; // подсчет наименьшего общ предка для текущих а-шек
        last_ans = resa.second;
        for (int i = 0; i < 2; ++i) {
            int tmp_a = (x * a1 + y * a2 + z) % n; // пересчет а-шек
            a1 = a2;
            a2 = tmp_a;
        }

    }
    std::cout << res; // вывод суммарного результата
}
