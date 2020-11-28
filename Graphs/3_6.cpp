#include<bits/stdc++.h>
//Шрек и Фиона пригласили всех своих друзей на свою свадьбу.
// На церемонии они хотят рассадить их всех на две непустые части так, чтобы количество
// знакомств между двумя частями было минимальным. Всего приглашенных на свадьбу n, а каждое знакомство обоюдно.
//
//Вам дан граф, в котором ребро означает знакомство между людьми. Помогите Шреку и
// Фионе поделить гостей на две непустые части.
using std::vector;
using std::pair;
// сообственно идея -- у нас есть теорема что при нахождении макс потока по Флойду-Фалкерсону у нас в ост сети можно
// вычислить мин. разрез в сети. ну мы просто ищем минимальный из этих мин. разрезов сети.

// В первой строке входного файла записано целое число n () — число гостей. Каждая из следующих n строк содержит по n символов.
// i-й символ j-й из этих строк равен «1», если между вершинами i и j есть ребро, и «0» в противном случае. 
//Заданная таким образом матрица смежности является антирефлексивной (на главной диагонали стоят нули) и симметричной (относительно главной диагонали).

//Выведите в выходной файл две строки. На первой выведите номера вершин, попавших в левую часть, через пробел, 
//а на второй — номера вершин, попавших в правую часть, также через пробел. Номера вершин можно выводить в любом порядке.
const int INF = 1e6;
vector<int> used;
// идею хранения графа читать в задаче 3.5
struct Edge {
    int from, to;
    int cap, flow;

    Edge(int from, int to, int cap) : from(from), to(to), cap(cap), flow(0) {};
};


void addEdge(int from, int to, vector<Edge> &edges, vector<int> &head, vector<int> &next) {
    edges.push_back(Edge(from, to, 1));
    next.push_back(head[from]);
    head[from] = edges.size() - 1;

    edges.push_back(Edge(to, from, 0));
    next.push_back(head[to]);
    head[to] = edges.size() - 1;
}

int findFlow(int vert, int destination, int flow, vector<Edge> &edges, vector<int> &head, vector<int> &next) {
    if (vert == destination) return flow;
    used[vert] = 1;
    for (int num = head[vert]; num != -1; num = next[num]) {
        int to = edges[num].to;
        int cap = edges[num].cap;
        if (!used[to] && cap > 0) {
            int min_res = findFlow(to, destination, std::min(flow, cap), edges, head, next);
            if (min_res > 0) {
                edges[num].cap -= min_res;
                edges[num].flow += min_res;
                edges[num ^ 1].cap += min_res;
                edges[num ^ 1].flow -= min_res;
                return min_res;
            }
        }
    }
    return 0;
}

void findMin(int n, vector<Edge> &edges, vector<int> &head, vector<int> &next) {
    int min_res = INF;
    vector<int> first_comp;
    vector<int> second_comp;
    for (int i = 1; i < n; ++i) { // смотрим на n потоков -- 0 -> 1, 0 -> 2, ..., 0 -> n-1 и берем минимум
        vector<Edge> copy_edges = edges; // копия графа
        used.assign(n, 0);
        while (true) {
            used.assign(n, 0);
            if (findFlow(0, i, INF, copy_edges, head, next) == 0) {
                break; // тут все сделано так, чтобы на массив used сохранился после пробега по ост сети, благодаря
                // чему мы получим наш разрез
            }
        }
        vector<int> fir_comp;
        vector<int> sec_comp;
        for (int i = 0; i < n; ++i) {
            if (used[i]) {
                fir_comp.push_back(i); // разброс вершин по множествам
            } else {
                sec_comp.push_back(i);
            }
        }
        int weight = 0;
        for (int i = 0; i < fir_comp.size(); ++i) {
            for (int num = head[fir_comp[i]]; num != -1; num = next[num]) {
                if (!used[edges[num].to] && edges[num].cap == 1) { // подсчет числа связей
                    ++weight;
                }
            }
        }
        if (weight < min_res) { // если оказалось меньше чем текущий минимум -- все меняем
            min_res = weight;
            first_comp = fir_comp;
            second_comp = sec_comp;
        }
    }
    for (int i = 0; i < first_comp.size(); ++i) {
        std::cout << first_comp[i] + 1 << ' ';
    }
    std::cout << '\n'; // вывод
    for (int i = 0; i < second_comp.size(); ++i) {
        std::cout << second_comp[i] + 1 << ' ';
    }
}

int main() {
    int n;
    std::cin >> n;
    vector<Edge> edges;
    vector<int> head(n, -1);
    vector<int> next;
    for (int i = 0; i < n; ++i) {
        std::string line;
        std::cin >> line;
        for (int j = 0; j < n; ++j) { // ввод
            if (line[j] == '1') {
                addEdge(i, j, edges, head, next);
            }
        }
    }
    findMin(n, edges, head, next);
}
