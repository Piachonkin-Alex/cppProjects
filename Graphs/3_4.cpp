#include<bits/stdc++.h>
//Шреку необходимо пробраться в башню, чтобы спасти прекрасную принцессу Фиону.
// И сейчас его путь лежит через старый полуразвалившийся мост.
// Осёл очень боится упасть с этого моста в лавовую реку под ним и отказывается идти дальше,
// пока мост не будет полностью починен. Шрек не может идти без друга и решил отремонтировать его.
//
//Мост представляет собой поле из n × m клеток, причем некоторые клетки ещё остались целыми.
// У Шрека есть только дощечки размера 1 × 2, установка которых занимает у него a секунд,
// и 1 × 1, установка которых занимает b секунд. Ему необходимо закрыть все пустые клетки,
// причем только их, не накладывая дощечки друг на друга.
//
//Определите, через какое минимальное количество секунд Шрек и Осёл смогут продолжить свой путь дальше.

//Первая строка входного файла содержит 4 целых числа n, m, a, b (1 ≤ n, m ≤ 100, |a| ≤ 1 000, |b| ≤ 1 000). 
//Каждая из последующих n строк содержит по m символов: символ  «.» (точка) обозначает целую клетку моста, а символ «*» (звёздочка) — пустую.

using std::vector;
using std::pair;
// задача на найти макс паросочетание (макс число плиток 2*1 при 2б>a). двудольность обеспечивается шахматной расскраской.

bool kuhn(pair<int, int> vert, vector<vector<vector<pair<int, int>>>> &edges, vector<vector<pair<int, int>>> &mt,
          vector<vector<int>> &used) { // Кунн
    if (used[vert.first][vert.second]) return false; // множество вершин графа -- двумерное.
    used[vert.first][vert.second] = 1;
    for (int i = 0; i < edges[vert.first][vert.second].size(); ++i) {
        pair<int, int> to = edges[vert.first][vert.second][i];
        if (mt[to.first][to.second] == pair<int, int>{-1, -1} || kuhn(mt[to.first][to.second], edges, mt, used)) {
            mt[to.first][to.second] = vert; // важный момент --  исходное значение mt[to] = {-1, -1}
            return true;
        }
    }
    return false;
}

void clean_used(vector<vector<int>> &used) { // функция очистки массива used (так как он двумерный)
    for (int i = 0; i < used.size(); ++i) {
        for (int j = 0; j < used[i].size(); ++j) {
            used[i][j] = 0;
        }
    }
}

void createGraph(int n, int m, vector<vector<vector<pair<int, int>>>> &edges, vector<vector<char>> &field) {
    vector<vector<pair<int, int>>> line(m); // заполнение графа по-по-полю
    for (int i = 0; i < n; ++i) {
        edges.push_back(line);
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (field[i][j] == '.') continue;
            if (j != m - 1) { // постоянные проверки на краи
                if (field[i][j + 1] == '*') edges[i][j].push_back({i, j + 1});
            }
            if (j != 0) {
                if (field[i][j - 1] == '*') edges[i][j].push_back({i, j - 1});
            }
            if (i != 0) {
                if (field[i - 1][j] == '*') edges[i][j].push_back({i - 1, j});
            }
            if (i != n - 1) {
                if (field[i + 1][j] == '*') edges[i][j].push_back({i + 1, j});
            }
        }
    }
}

int main() {
    int n, m, a, b;
    std::cin >> n >> m >> a >> b;
    vector<vector<char>> field;
    int bad_cells = 0;
    for (int i = 0; i < n; ++i) {
        vector<char> line;
        for (int j = 0; j < m; ++j) {
            char cell;
            std::cin >> cell;
            if (cell == '*') bad_cells++; // ввод поля
            line.push_back(cell);
        }
        field.push_back(line);
    }
    if (2 * b <= a) { // этот случай неинтересный
        std::cout << bad_cells * b;
        return 0;
    }
    vector<vector<vector<pair<int, int>>>> edges; //Вух, 3д матрица, интересно. Но, не проще было бы просто хранить 4 числа, обозначавшие наличие ребра
    //вверх, вниз, налево, направо?
    createGraph(n, m, edges, field); // создание графа
    vector<vector<int>> used;
    for (int g = 0; g < n; ++g) {
        used.push_back(vector<int>(m, 0)); // заполнение used
    }
    vector<vector<pair<int, int>>> mt;
    for (int j = 0; j < n; ++j) {
        mt.push_back(vector<pair<int, int>>(m, {-1, -1})); // заполнение массива mt
    }
    //По хорошему, этот цикл следовало бы выделить в отдельную функцию.
    int double_plit = 0;
    for (int t = 0; t < n; ++t) {
        for (int j = 0; j < m; ++j) {
            if ((t + j) % 2 == 1) {
                if (kuhn({t, j}, edges, mt, used)) { // запуск только по нечетным! (черным клеткам)
                    double_plit++; // если кунн зашел -- паросочетание увеличилось
                }
            }
            clean_used(used);
        }
    }
    std::cout << double_plit * a + (bad_cells - 2 * double_plit) * b; // вывод ответа
}
