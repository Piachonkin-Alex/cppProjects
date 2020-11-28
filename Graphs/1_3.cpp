#include <iostream>
#include <vector>
//Фрэнку опять прилетел новый заказ. Однако в этот раз город играет по очень странным правилам:
//все дороги в городе односторонние и связывают только офисы нанимателей перевозчика.
//Множество офисов, между любыми двумя из которых существует путь, образуют квартал,
// если нельзя добавить никакие другие, чтобы условие выполнялось. Фрэнку интересно,
// каково минимальное количество односторонних дорог нужно ещё построить, чтобы весь город стал квраталом.

// Замечание: квартал -- компонента сильной связности!!!
// Решение -- если граф изначально не является кварталом (в этом случае ответ очевидно 0)
// , то ответом будет максимум из количества стоков и истоков для кварталов (другими словами, максимум из числа кварталов
// в графе, в которое нет ни одного ребра, и числа кварталов в графе, из которого нет ни одного ребра. Очевидно, что такое
// число ребер необходимо. Достаточность доказывается индукцией по числу компонент связности (там тривиально рассматривается
// 3 случая).


class Graph { // граф
private:
    int num;  // число вершин
    std::vector<std::vector<int>> edges; // список смежности
    std::vector<bool> used;  // использована вершина или нет
    std::vector<int> tout;  // времена выхода

public:
    Graph(int num) : num(num), edges(num), used(num, false) {};

    void Add_Edge(int in, int to) { // добавление ребра
        edges[in].push_back(to);
    }

    void First_Dfs(int vert) { // первый дфс для посчета времен выхода для поиска компонент сильной связности
        if (used[vert]) {
            return;
        }
        used[vert] = true;
        for (int i = 0; i < edges[vert].size(); ++i) {
            if (!used[edges[vert][i]]) {
                First_Dfs(edges[vert][i]);
            }
        }
        tout.push_back(vert);
    }

    void Second_Dfs(int vert, std::vector<int> &component) {   // второй дфс для составления компонент сильной связности
        if (used[vert]) {
            return;
        }
        component.push_back(vert);
        used[vert] = true;
        for (int i = 0; i < edges[vert].size(); ++i) {
            if (!used[edges[vert][i]]) {
                Second_Dfs(edges[vert][i], component);
            }
        }
    }

    std::vector<int> &Take_Tout() {  // извлечение массива времен выхода
        return tout;
    }

    int Origins_vert(int vert, std::vector<int> &in_which_comp) {
        // функция подсчета числа ребер из вершины в другие компоненты связности (не в компоненту связности этой вершины)
        int res = 0;
        for (int i = 0; i < edges[vert].size(); ++i) {
            if (in_which_comp[edges[vert][i]] != in_which_comp[vert]) {
                res++;
            }
        }
        return res;
    }
};


void Construct_Components(int n, std::vector<std::vector<int>> &components, Graph *init, Graph *trans,
                          std::vector<int> &in_which_comp) { // постройка компонент сильной связности
    for (int j = 0; j < n; ++j) {   // первый дфс на подсчет времен в обычном графе
        init->First_Dfs(j);
    }

    std::vector<int> tout_time = init->Take_Tout();
    for (int i = n - 1; i > -1; --i) {  // второй дфс по временам в транспонированном графе и постройка компонент связности
        std::vector<int> component;
        trans->Second_Dfs(tout_time[i], component);
        if (component.size()) {
            components.push_back(component);
        }
    }

    for (int i = 0; i < components.size(); ++i) { // заполнение массива, хранящего номера компонент связности для вершин
        for (int v : components[i]) {
            in_which_comp[v] = i;
        }
    }
}

int Find_Origins(std::vector<std::vector<int>> &components, Graph *graph, std::vector<int> &in_which_comp) {
    int res = 0;  // подсчет числа истоков для графа. компонента связности -- исток <-> колво ребер из всех вершин в
    // другие компоненты связности - 0
    for (int i = 0; i < components.size(); ++i) {
        int verts_without_bad_edges = 0;
        for (int j = 0; j < components[i].size(); ++j) {
            verts_without_bad_edges += graph->Origins_vert(components[i][j], in_which_comp);
        }
        if (!verts_without_bad_edges) {
            res++;
        }
    }
    return res;
}

int main() {
    int n, m;
    std::cin >> n >> m;
    Graph *init_graph = new Graph(n);
    Graph *trans_graph = new Graph(n);
    for (int i = 0; i < m; ++i) {
        int from, to;
        std::cin >> from >> to;  // ввод
        if (from != to) {
            init_graph->Add_Edge(from - 1, to - 1);
            trans_graph->Add_Edge(to - 1, from - 1);
        }
    }

    std::vector<std::vector<int>> components;
    std::vector<int> in_which_comp(n);
    Construct_Components(n, components, init_graph, trans_graph, in_which_comp);
    int origins = Find_Origins(components, init_graph, in_which_comp);
    int stock = Find_Origins(components, trans_graph, in_which_comp);  // тут финт -- число стоков = число истоков
    // в транспонированном графе
    delete init_graph;
    delete trans_graph;
    if (components.size() == 1) {
        std::cout << 0;
    } else {
        std::cout << std::max(origins, stock);
    }
}
