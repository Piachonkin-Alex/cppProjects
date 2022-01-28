#include <iostream>
#include <vector>
#include <algorithm>

class Graph { // граф
public:
    Graph(int verts) : edges(verts) {};

    void AddEdge(int from, int in) { // добавление ребра
        if (from != in) {
            edges[from].push_back(in);
            edges[in].push_back(from);
        }
    }

    int CountVerts() {
        return edges.size();
    }

    void AdjacentThisVert(int vert, std::vector<int> &adjacents) { // связные с данной вершиной
        for (int i = 0; i < edges[vert].size(); ++i) {
            adjacents.push_back(edges[vert][i]);
        }
    }

    void DeleteEdge(int from, int in) { // удаление ребра
        auto iter1 = std::find(edges[from].begin(), edges[from].end(), in);
        auto iter2 = std::find(edges[in].begin(), edges[in].end(), from);
        if (iter2 != edges[in].end()) {
            edges[from].erase(iter1);
            edges[in].erase(iter2);
        }
    }

private:
    std::vector<std::vector<int>> edges;
};

void FindComponent(Graph &graph, int vert, std::vector<int> &used, std::vector<int> &component) { // поиск компоненты связности
    used[vert] = 1;
    component.push_back(vert);
    std::vector<int> adjacent_to_vert;
    graph.AdjacentThisVert(vert, adjacent_to_vert);
    for (int i = 0; i < adjacent_to_vert.size(); ++i) {
        if (!used[adjacent_to_vert[i]]) {
            FindComponent(graph, adjacent_to_vert[i], used, component);
        }
    }
}

bool CycleDfs(Graph &graph, std::vector<int> &used, std::vector<int> &cycle,  // нахождение цикла. при этом, этот алгоритм не ломается при точке сочленения
              int vert, std::vector<int> &prev, bool find = false) {
    used[vert] = 1;
    std::vector<int> adjacent_to_vert;
    graph.AdjacentThisVert(vert, adjacent_to_vert);
    for (int i = 0; i < adjacent_to_vert.size(); ++i) {
        if (adjacent_to_vert[i] != prev[vert] && used[adjacent_to_vert[i]]) {
            if (!find) {
                cycle.push_back(adjacent_to_vert[i]);
                find = true;
                while (adjacent_to_vert[i] != vert) {
                    cycle.push_back(vert);
                    vert = prev[vert];
                }
            }
            return true;
        }
        if (!used[adjacent_to_vert[i]]) {
            prev[adjacent_to_vert[i]] = vert;
            if (CycleDfs(graph, used, cycle, adjacent_to_vert[i], prev, find)) {
                return true;
            }
        }
    }
    return false;
}


bool FindCycle(Graph &graph, std::vector<int> &component, std::vector<int> &cycle) {  // тут просто для удобства чтоб в главной функции не составлять лишние массивы
    std::vector<int> used(graph.CountVerts()), prev(graph.CountVerts());
    return CycleDfs(graph, used, cycle, component[0], prev);

}

void BridgeDfs(Graph &graph, int now_time, int vert, int prev, std::vector<int> &used, std::vector<int> &in_time, // поиск мостов
               std::vector<int> &low_time, std::vector<std::pair<int, int>> &bridges) {
    used[vert] = 1;
    in_time[vert] = now_time, low_time[vert] = now_time;
    std::vector<int> adjacent_to_vert;
    graph.AdjacentThisVert(vert,adjacent_to_vert);
    for (int i = 0; i < adjacent_to_vert.size(); ++i) {
        if (adjacent_to_vert[i] == prev) {
            continue;
        }
        if (used[adjacent_to_vert[i]] == 1) {
            low_time[vert] = std::min(low_time[vert], low_time[i]);
        } else {
            BridgeDfs(graph, now_time + 1, adjacent_to_vert[i], vert, used, in_time, low_time, bridges);
            low_time[vert] = std::min(low_time[vert], low_time[adjacent_to_vert[i]]);
            if (low_time[adjacent_to_vert[i]] > in_time[vert]) {
                std::pair<int, int> bridge(vert, adjacent_to_vert[i]);
                bridges.push_back(bridge);
            }
        }
    }
}

void FindBridges(Graph &graph, std::vector<int> &component, std::vector<std::pair<int, int>> &bridges) { // аналогично для удобства
    std::vector<int> in_time(graph.CountVerts()), low_time(graph.CountVerts()), used(graph.CountVerts());
    BridgeDfs(graph, 0, component[0], -1, used, in_time, low_time, bridges);
}

void FindSegment(Graph &graph, int vert, std::vector<int> &used, std::vector<int> &contact, std::vector<int> &segment) {
    if (std::find(segment.begin(), segment.end(), vert) == segment.end()) { // поиск сегмента
        segment.push_back(vert); // если данной вершины в сегменте нет -- добавляем ее
    }
    if (contact[vert]) { // если вершина контактна -- выходим
        return;
    }
    used[vert] = 1;
    std::vector<int> adjacent_to_vert;
    graph.AdjacentThisVert(vert,adjacent_to_vert);
    for (int i = 0; i < adjacent_to_vert.size(); ++i) {
        if (!used[adjacent_to_vert[i]]) {
            FindSegment(graph, adjacent_to_vert[i], used, contact, segment); // повтор операции для всех вершин, соединенных с данной
        }
    }
}

void FindAllSegments(Graph &graph, std::vector<int> &component, std::vector<int> contact,
                     std::vector<std::vector<int>> &all_segments) { // поиск всех сегментов
    std::vector<int> used(graph.CountVerts());
    for (int i = 0; i < component.size(); ++i) {
        if (!used[component[i]] && !contact[component[i]]) { // запускаем для всех неконтактных вершин
            std::vector<int> new_seg;
            FindSegment(graph, component[i], used, contact, new_seg);
            all_segments.push_back(new_seg);
        } else if (contact[component[i]]) {
            std::vector<int> adjacent_to_vert;
            graph.AdjacentThisVert(component[i], adjacent_to_vert);
            for (int j = 0; j < adjacent_to_vert.size(); ++j) {
                if (contact[adjacent_to_vert[j]] && adjacent_to_vert[j] > component[i]) {
                    std::vector<int> new_seg = {component[i], adjacent_to_vert[j]};
                    all_segments.push_back(new_seg);
                }
            }
        }
    }
}

void FingPath(Graph &graph, int vert, int prev, std::vector<int> &used, std::vector<int> &contact,
              std::vector<int> &path, std::vector<int> &segment) { // поиск пути в сегменте
    if (!contact[vert]) {
        used[vert] = 1;
    }
    std::vector<int> adjacent_to_vert;
    graph.AdjacentThisVert(vert, adjacent_to_vert);
    for (int i = 0; i < adjacent_to_vert.size(); ++i) {
        if (adjacent_to_vert[i] != prev &&
            std::find(segment.begin(), segment.end(), adjacent_to_vert[i]) != segment.end()) { // если вершина не предыдущая и лежит в сегменте
            if (contact[adjacent_to_vert[i]]) { // если вершина контактна -- добавить в путь
                path.push_back(adjacent_to_vert[i]);
                break;
            }
            FingPath(graph, adjacent_to_vert[i], vert, used, contact, path, segment);
            if (!path.empty()) { // если путь не пустой -- добавить следующую
                path.push_back(adjacent_to_vert[i]);
                break;
            }
        }
    }
}

bool IsComponentPlanar(Graph &original_graph, std::vector<int> &component) { // проверка компоненту на планарность
    Graph graph = original_graph;
    std::vector<int> cycle;
    if (!FindCycle(graph, component, cycle)) {
        return true;
    } // поиск цикла
    std::vector<int> used(graph.CountVerts()), contact(graph.CountVerts());
    for (int i = 0; i < cycle.size(); ++i) {
        contact[cycle[i]] = 1;
    } // вершины цикла -- контактны
    graph.DeleteEdge(cycle[0], cycle[cycle.size() - 1]);
    for (int j = 1; j < cycle.size(); ++j) {
        graph.DeleteEdge(cycle[j], cycle[j - 1]);
    } // удаление ребер между контактными вершнами
    std::vector<std::vector<int>> segments; // массив сегментов
    FindAllSegments(graph, component, contact, segments);
    std::vector<std::vector<int>> faces; // грани
    faces.push_back(cycle);
    faces.push_back(cycle); // первые две из них
    while (!segments.empty()) {
        std::vector<std::vector<int>> segments_faces(segments.size()); // подходяшие грани для каждого сегмента
        for (int i = 0; i < segments.size(); ++i) {
            std::vector<int> contact_verts;
            for (int j = 0; j < segments[i].size(); ++j) {
                if (contact[segments[i][j]]) {
                    contact_verts.push_back(segments[i][j]); // добавление контактных верщин в сегменте
                }
            }
            for (int t = 0; t < faces.size(); ++t) { // подсчет граней для каждого сегмента
                std::vector<int> &face = faces[t];
                bool in_this_face = true;
                for (int m = 0; m < contact_verts.size(); ++m) {
                    if (std::find(face.begin(), face.end(), contact_verts[m]) == face.end()) {
                        in_this_face = false;
                        break;
                    }
                }
                if (in_this_face) {
                    segments_faces[i].push_back(t);
                }
            }
            if (segments_faces[i].size() == 0) {
                return false;
            } // если для некоторого сегмента мнво граней равно ноль -- проиграли
        }  // это был подсчет того, какие сегметны в какие грани помещаются
        int min_seg = 0;
        for (int q = 1; q < segments_faces.size(); ++q) {
            if (segments_faces[min_seg].size() > segments_faces[q].size()) {
                min_seg = q;
            } // сегмент с минимальным числом граней
        }
        int first_contact_vert = 0;
        while (!contact[segments[min_seg][first_contact_vert]]) {
            ++first_contact_vert;
        } // ищем контактную вершину
        std::vector<int> used(graph.CountVerts()), path;
        FingPath(graph, segments[min_seg][first_contact_vert], -1, used, contact, path, segments[min_seg]);
        path.push_back(segments[min_seg][first_contact_vert]); // нашли путб между контактными вершинами
        int face_to_split = segments_faces[min_seg][0]; // грань, на которую будем разбивать
        std::vector<int> face_first, face_second; // грани, которые получаться при разбитии
        auto iter1 = find(faces[face_to_split].begin(), faces[face_to_split].end(), path[0]);
        auto iter2 = find(faces[face_to_split].begin(), faces[face_to_split].end(), path[path.size() - 1]);
        if (int(std::distance(iter1, iter2)) < 0) {
            std::swap(iter1, iter2);
        } // нашли в разбивающейся гране начало и конец пути
        face_first.insert(face_first.begin(), iter1, iter2 + 1); // первая часть вершин в первую грань
        if (face_first[face_first.size() - 1] != path[0]) {
            std::reverse(path.begin(), path.end()); // разворот пути
        }
        face_first.insert(face_first.end(), path.begin() + 1, path.end() - 1); // вторая часть вершин в первую грань
        std::reverse(path.begin(), path.end());

        face_second.insert(face_second.begin(), faces[face_to_split].begin(), iter1 + 1);
        face_second.insert(face_second.end(), path.begin() + 1, path.end() - 1);  // добавление вершин во вторую грань
        face_second.insert(face_second.end(), iter2, faces[face_to_split].end());
        faces.erase(faces.begin() + face_to_split); // удаление разбитой грани
        faces.push_back(face_first);
        faces.push_back(face_second); // добавление полученной
        for (int t = 1; t < path.size(); ++t) {
            contact[path[t]] = 1;
            graph.DeleteEdge(path[t - 1], path[t]); // удаление ребер путь
        }
        contact[path[0]] = 1;
        segments.clear();
        FindAllSegments(graph, component, contact, segments); // заново ищем сегменты
    }
    return true;
}

bool GraphPlanar(Graph &graph) {
    std::vector<int> used(graph.CountVerts());
    std::vector<std::vector<int>> components;
    std::vector<std::pair<int, int>> all_bridges;
    for (int i = 0; i < used.size(); ++i) {
        if (!used[i]) {
            std::vector<int> new_comp;
            FindComponent(graph, i, used, new_comp); // разбитие на компоненты
            components.push_back(new_comp);
        }
    }
    for (int j = 0; j < components.size(); ++j) {
        std::vector<std::pair<int, int>> comp_bridges;
        FindBridges(graph, components[j], comp_bridges); // поиск мостов
        for (int t = 0; t < comp_bridges.size(); ++t) {
            all_bridges.push_back(comp_bridges[t]);
        }
    }
    for (int i = 0; i < all_bridges.size(); ++i) {
        graph.DeleteEdge(all_bridges[i].first, all_bridges[i].second); //удаление мостов
    }
    components.clear();
    for (int j = 0; j < used.size(); ++j) { // на компоненты нужно разделить заново осле поиска мостов
        used[j] = 0;
    }
    for (int j = 0; j < used.size(); ++j) {
        if (!used[j]) {
            std::vector<int> new_comp;
            FindComponent(graph, j, used, new_comp); // заново разбили
            components.push_back(new_comp);
        }
    }
    for (int q = 0; q < components.size(); ++q) {
        if (!IsComponentPlanar(graph, components[q])) {
            return false; // проверили все компоненты
        }
    }
    return true; // победили
}

int main() {
    int n, m;
    std::cin >> n >> m;
    Graph graph(n);
    for (int i = 0; i < m; ++i) {
        int from, in;
        std::cin >> from >> in;
        graph.AddEdge(from, in);
    }
    int j = 0;
    if (GraphPlanar(graph)) {
        std::cout << "YES";
    } else {
        std::cout << "NO";
    }
}

