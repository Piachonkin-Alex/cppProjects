#include<bits/stdc++.h>
//Шрек и Осёл уже были на пути домой. Им оставалось только преодолеть лес,
// который отделял их от болота. Но они поссорились, поэтому не хотят идти вместе.
//
//Лес представляет собой опушки, пронумерованы числами от 1 до n и соединенные m дорожками
// (может быть несколько дорожек соединяющих две опушки, могут быть дорожки, соединяющие опушку с собой же).
// Из-за ссоры, если по дорожке прошел один из друзей, то второй по той же дорожке уже идти не может.
// Сейчас друзья находятся на опушке с номером s, а болото Шрека — на опушке с номером t.
// Помогите Шреку и Ослу добраться до болота.

//В первой строке файла записаны четыре целых числа — n, m, s и t (2 ≤ n ≤ 10^5, 0 ≤ m ≤ 10^5, s!=t). В следующих m строках записаны пары чисел.
// Пара чисел (x, y) означает, что есть дорожка с опушки x до опушки y (из-за особенностей местности дорожки односторонние).
using std::vector;
using std::pair;


// решение простое -- если поток между стартовой и конечной меньше 2 -- нету 2 непересек путей. иначе есть.
// строить пути будем дфсом. после того, как один построили -- его удалили
const int INF = 1e6;
vector<int> used;

struct Edge { // ребро
    int from, to;
    int cap, flow;

    Edge(int from, int to, int cap) : from(from), to(to), cap(cap), flow(0) {};
};

// опишу тут как все работает с хранением. есть массив ребер edges. есть массив head на n элементов. в ячейке i он показывает,
// какое по счету в массиве edges первое ребро стартом из i. формально говоря, g[i][0] == edges[head[i]].
// массив next делаем так, что g[i][1] == edges[next[head[i]]], g[i][2] == edges[next[next[head[i]]]] ..., ну и когда
// next[next[...[head[i]]...] == -1 -> ребра стартом из i закончились.
//

void addEdge(int from, int to, vector<Edge> &edges, vector<int> &head, vector<int> &next) {
    edges.push_back(Edge(from, to, 1)); // добавляем прямое ребро.
    next.push_back(head[from]);
    head[from] = edges.size() - 1;

    edges.push_back(Edge(to, from, 0)); // добавляем обратное ребро нулевого веса
    next.push_back(head[to]);
    head[to] = edges.size() - 1;
}

int findFlow(int vert, int destination, int flow, vector<Edge> &edges, vector<int> &head, vector<int> &next) {
    if (vert == destination) return flow; // Флойд-Фалкерсон.
    used[vert] = 1;
    for (int num = head[vert]; num != -1; num = next[num]) {
        int to = edges[num].to;
        int cap = edges[num].cap;
        if (!used[to] && cap > 0) {
            int min_res = findFlow(to, destination, std::min(flow, cap), edges, head, next);
            if (min_res > 0) {
                edges[num].cap -= min_res;
                edges[num].flow += min_res;
                edges[num ^ 1].cap += min_res; // тут xor позволяет выйти на обратное ребро
                edges[num ^ 1].flow -= min_res;
                return min_res;
            }
        }
    }
    return 0;
}

void find_path(int vert, int from, int destination, vector<Edge> &edges, vector<int> &head, vector<int> &next,
               vector<int> &used_edge) { // поиск пути по потоку.
    if (used[vert]) {
        return;
    }
    used[vert] = 1;
    used_edge[vert] = from; // сюда записываем какое ребро использовали для восстановления пути
    if (vert == destination) {
        return;
    }
    for (int num = head[vert]; num != -1; num = next[num]) {
        if (edges[num].flow > 0) {
            find_path(edges[num].to, num, destination, edges, head, next, used_edge);
        }
    }
}

void deletePath(int final, vector<Edge> &edges, vector<int> &used_edge) { // удаление пути
    vector<int> path;
    path.push_back(final);
    int num_edge = used_edge[final];
    while (true) {
        if (num_edge == -1) {
            break;
        }
        path.push_back(edges[num_edge].from); // восстановление пути по массиву used_edge.
        edges[num_edge].flow -= 1; // удаление пути через вычет потока (так как путь ищем по нему)
        num_edge = used_edge[edges[num_edge].from];
    }
    std::reverse(path.begin(), path.end());
    for (int i = 0; i < path.size(); ++i) { // вывод
        std::cout << path[i] + 1 << ' ';
    }
    std::cout << std::endl;
}

int main() {
    int n, m, s, t;
    std::cin >> n >> m >> s >> t;
    vector<Edge> edges;
    vector<int> head(n, -1);
    vector<int> next;
    for (int i = 0; i < m; ++i) { // ввод и добавление ребер
        int from, to;
        std::cin >> from >> to;
        addEdge(from - 1, to - 1, edges, head, next);
    }
    used.assign(n, 0);
    int max_flow = 0;
    int iter_flow = 0;
    while ((iter_flow = findFlow(s - 1, t - 1, INF, edges, head, next)) > 0) {
        used.assign(n, 0);
        max_flow += iter_flow; // подсчет макс потока
    }
    if (max_flow < 2) { // меньше 2 -- нет
        std::cout << "NO";
        return 0;
    }
    std::cout << "YES" << '\n';
    vector<int> used_edge(n, -1);
    for (int j = 0; j < 2; ++j) {
        used.assign(n, 0); // поиск пути 2 раза
        find_path(s - 1, -1, t - 1, edges, head, next, used_edge);
        deletePath(t - 1, edges, used_edge);
    }
}
