#include <iostream>
#include <vector>
#include <set>
#include <assert.h>
// минимальный остов Примой

//Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно. 
//Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается тремя натуральными числами 
//bi, ei и wi — номера концов ребра и его вес соответственно (1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100 000). n ≤ 5 000, m ≤ 100 000.

Граф является связным.



const int INF = 1000000000;
using std::vector;
using std::pair;

void Prima(int n, vector<vector<pair<int, int>>> &edges) {
    vector<int> min_edge(n, INF), used(n, 0);
    long res = 0;
    min_edge[0] = 0;
    std::set<pair<int, int>> q; // очередь на минимальное следуюещее ребро
    q.insert({0, 0});
    for (size_t i = 0; i < n; ++i) {
        if (q.empty()) {
            std::cout << "No MST"; // тут этого нет, но для красоты надо
            return;
        }
        int vert = q.begin()->second;
        used[vert] = 1;
        res += q.begin()->first; // извлекаем из очереди и прибавляем результат
        q.erase(q.begin());
        for (size_t j = 0; j < edges[vert].size(); ++j) {
            int to = edges[vert][j].first;
            int weight = edges[vert][j].second; // закидываем в очередь все инцидентные ребра
            if (weight < min_edge[to] && !used[to]) {
                q.erase({min_edge[to], to});
                min_edge[to] = weight;
                q.insert({min_edge[to], to});
            }
        }
    }
    std::cout << res; // вывод
}

int main() {
    int n, m;
    std::cin >> n >> m;
    vector<vector<pair<int, int>>> edges(n);
    for (size_t i = 0; i < m; ++i) {
        int in, to, weight;
        std::cin >> in >> to >> weight;
        edges[in - 1].push_back({to - 1, weight}); // ввод
        edges[to - 1].push_back({in - 1, weight});
    }
    Prima(n, edges);
}
