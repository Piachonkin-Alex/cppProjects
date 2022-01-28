#include <iostream>
#include <vector>
#include <set>
#include <assert.h>
#include <algorithm>

// Минимальный остов Крускалом
//Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно. 
//Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается тремя натуральными числами 
//bi, ei и wi — номера концов ребра и его вес соответственно (1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100 000). n ≤ 5 000, m ≤ 100 000.

const int INF = 1000000000;
using std::vector;
using std::pair;


// первые 3 фукции -- реализация функций для системы непересекающихся мн-в
void createSet(int vert, vector<int> &parent, vector<vector<int>> &list) { // создание мн-в
    list[vert] = vector<int>(1, vert);
    parent[vert] = vert;
}

int findSet(int vert, vector<int> &parent) { // нахождение уникального номера мн-ва по элементу
    return parent[vert];
}

void uniteSets(int first, int second, vector<int> &parent, vector<vector<int>> &list) { // объединение
    int a = findSet(first, parent);
    int b = findSet(second, parent);
    if (list[a].size() < list[b].size()) { // меняем местами чтобы добавлять меньший размер
        std::swap(a, b);
    }
    //хранить список вершин класса неплозая идея, однако выйгрыша в производительности ты не получишь, так как без разницы, когда ты пройдёшься по вершинам
    // при вызове поиска родителя или сейчас. Однако памяти ты поглотил много, что плохо.
    while (!list[b].empty()) {
        int vert = list[b].back();
        list[b].pop_back();
        parent[vert] = a;
        list[a].push_back(vert);
    }
}

void Kruscal(int n, vector<pair<int, pair<int, int>>> &edges) { // сам Крускал
    vector<vector<int>> list(n);
    vector<int> parent(n);
    for (int i = 0; i < n; ++i) {
        createSet(i, parent, list); // создали компоненты
    }
    int res = 0;
    int iter = 0; // тут итератор для того, чтобы понять, что мы нашли n-1 ребро и можно остановится
    for (int i = 0; i < edges.size(); ++i) {
        int first_vert = edges[i].second.first, second_vert = edges[i].second.second, weight = edges[i].first;
        if(parent[first_vert] != parent[second_vert]){
            res += weight;
            ++iter;
            uniteSets(first_vert, second_vert, parent, list);
        }
        if(iter == n - 1){
            break;
        }
    }
    std::cout << res; // вывод
}

int main() {
    int n, m;
    std::cin >> n >> m;
    vector<pair<int, pair<int, int>>> edges;
    for (size_t i = 0; i < m; ++i) {
        int in, to, weight;
        std::cin >> in >> to >> weight; // вывод
        edges.push_back({weight, {in - 1, to - 1}});
    }
    std::sort(edges.begin(), edges.end()); // сортировка
    Kruscal(n, edges);
}
