#include<bits/stdc++.h>
// Вам дан неориентированный граф, состоящий из n вершин. На каждой вершине записано число;
// число, записанное на вершине i, равно ai. Изначально в графе нет ни одного ребра.
//
//Вы можете добавлять ребра в граф за определенную стоимость.
// За добавление ребра между вершинами x и y надо заплатить ax + ay монет.
// Также существует m специальных предложений, каждое из которых характеризуется тремя числами x, y и w,
// и означает, что можно добавить ребро между вершинами x и y за w монет.
// Эти специальные предложения не обязательно использовать: если существует такая пара вершин x и y,
// такая, что для нее существует специальное предложение, можно все равно добавить ребро между ними за ax + ay монет.
//
//Сколько монет минимально вам потребуется, чтобы сделать граф связным?
// Граф является связным, если от каждой вершины можно добраться до любой другой вершины, используя только ребра этого графа.
using std::vector;
using std::pair;



int findSet(int vert, vector<int> &parent) {
    return parent[vert] == vert ? vert : parent[vert] = findSet(parent[vert], parent);
}
 // снова функции системы непересекающихся множеств. Важно -- правильная оформление findSet -- ~ O(1)
void uniteSets(int parent_fir, int parent_sec, vector<int> &parent) {
    parent[parent_fir] = parent_sec;
}

class Edge { // ребро с весом
public:
    int in, to;
    long long w;

    Edge() {}

    Edge(long in, long to, long long w) {
        in = in;
        to = to;
        w = w;
    }
};

bool compare(Edge a, Edge b) { // компаратор на сортировку ребер
    return a.w < b.w;
}

void Kruscal(int n, int m, Edge *edges) { // Крускал
    vector<int> parent(n);
    for (int i = 0; i < n; ++i) parent[i] = i;
    long long res = 0;
    int iter = 0;
    for (int i = 0; i < n + m - 1; ++i) {
        Edge now_edge = edges[i];
        int parent_fir = findSet(now_edge.in, parent), parent_sec = findSet(now_edge.to, parent);
        if (parent_fir != parent_sec) {
            res += now_edge.w;
            uniteSets(parent_fir, parent_sec, parent);
        }
    }
    std::cout << res;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);  // ввод вершин
    int n, m;
    std::cin >> n >> m;
    //Абзацы(
    int min_index = -1;
    long long min_weight = 1e18;// Лучше бы взял первый элемент массива
    vector<long long> verts(n);
    for (int i = 0; i < n; ++i) {
        long long w;
        std::cin >> w;
        verts[i] = w;
        if (w < min_weight) {
            min_weight = w; // здесь важный факт -- мы ищем вершину с наим стоимостью потому что без спец предложений
            // очевидно стоить мин. остов надо по ней
            min_index = i;
        }
    }
    
    Edge edges[m + n - 1];
    for (int j = 0; j < m; ++j) {
        long long in, to, w;
        //Заметь, по условию спецпредложения могут быть не выгодными, и можно было бы их проверять по дороге, что могло бы сэкономить на рёбрах
        std::cin >> in >> to >> w; // ввод спец предложений
        edges[j].in = in - 1, edges[j].to = to - 1, edges[j].w = w;
    }
    int vert_iter = 0;
    for (int i = 0; i < n; ++i) {
        if (i == min_index) { // добавление ребер из минимальной вершины ребер
            continue;
        }
        edges[m + vert_iter].to = min_index, edges[m + vert_iter].in = i;
        edges[m + vert_iter].w = min_weight + verts[i];
        ++vert_iter;
    }
    std::sort(edges, edges + m + n - 1, compare); // сортировка
    Kruscal(n, m, edges); // Крускал
}
