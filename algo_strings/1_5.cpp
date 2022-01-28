#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

//Даны строки s и t. Постройте сжатое суффиксное дерево, которое содержит все суффиксы строки s и строки t.
// Найдите такое дерево, которое содержит минимальное количество вершин.

using std::map;
using std::pair;
using std::string;
using std::vector;

class SuffixTree {
public:
    explicit SuffixTree(const string &text);


    void chunkByIndexAndPrintTree(size_t index);// функция разделения суф дерева на два паттерна, разделенными
    // в позиции index вспомогательным знаком

private:
    struct tree_position {
        // структурка, определяющая положение в дереве
        size_t vert;      // следующая вершина
        size_t char_index;// количество символов, которое нужно пройти до вершины
        tree_position();

        tree_position(size_t vert_id, size_t index);
    };
    int addVertex(size_t left_bord, size_t right_bord, int parent);// функция добавления вершины

    tree_position getSuffixLink(const tree_position &position);// функция расчета суфф. ссылки

    void doNextStep(int index);// функция следующей фазы

    void dfsWithVertexDescription(int vertex_id, size_t divide_index, vector<int> &dfs_numeration, int &numerator);
    // Дфс, выводящий описание дерева.


    struct vertex;


    string pattern;                          // строка, для которого делаем суфф. дерево
    tree_position current_pos;               // текущая позиция в дереве
    vector<std::unique_ptr<vertex>> vertices;// вершины
};
SuffixTree::tree_position::tree_position() : vert(0), char_index(0) {}

SuffixTree::tree_position::tree_position(size_t vert_id, size_t index) : vert(vert_id), char_index(index) {}

struct SuffixTree::vertex {      // структура вершины
    size_t start = 0, finish = 0;// границы индексов паттерна, которые лежат на ребре между parent и данной вершиной
    map<char, int> edges;        // ребра
    int parent = -1;
    int suffix_link = -1;// суфф. ссылка

    size_t lengthFromParent() const {// длина ребра
        return finish - start;
    }

    vertex() = default;

    vertex(size_t left_bord, size_t right_bord, int parent) : start(left_bord), finish(right_bord), parent(parent),
                                                              suffix_link(-1) {}
};

int SuffixTree::addVertex(size_t left_bord, size_t right_bord, int parent) {
    int new_vert_index = vertices.size();// создание новой вершины по заданным данным
    vertices.push_back(std::make_unique<vertex>(left_bord, right_bord, parent));
    vertices[parent]->edges[pattern[left_bord]] = new_vert_index;
    return new_vert_index;// возвращаем номер добавленой вершины
}

SuffixTree::tree_position SuffixTree::getSuffixLink(const tree_position &position) {
    if (position.vert == 0) {
        return tree_position(-1, 0);// если мы находимся в корне, то суфф. ссылка у нас на джокер
    }
    int parent = vertices[position.vert]->parent;// родитель для текущей позиции
    int tail_length = vertices[position.vert]->lengthFromParent() - position.char_index;
    int suff_link_vert = parent;// текущая суфф. ссылка
    if (parent == 0) {
        --tail_length;// случай корня обрабатывается отдельно
    } else {
        suff_link_vert = vertices[parent]->suffix_link;// суфф. ссылка для родителя
    }
    while (tail_length > 0) {// пока хвост больше нуля, идем вперед
        int transition_vert = vertices[suff_link_vert]->edges[pattern[vertices[position.vert]->finish -
                                                                      tail_length - position.char_index]];
        suff_link_vert = transition_vert;
        tail_length -= vertices[suff_link_vert]->lengthFromParent();
    }
    return tree_position(suff_link_vert, -tail_length);
}

void SuffixTree::doNextStep(int index) {
    int found_link = -1;// номер вершины, ожидающей суфф. ссылку
    while (true) {
        if (current_pos.vert == -1) {// мы находимся в джокере
            current_pos = tree_position(0, 0);
            return;
        }
        if (current_pos.char_index == 0) {// находимся в вершине
            if (found_link != -1) {       // если есть вершина, ожидающая суфф. ссылку
                vertices[found_link]->suffix_link = current_pos.vert;
                found_link = -1;
            }
            if (!vertices[current_pos.vert]->edges.count(pattern[index])) {// если ребра нет, то добавили вершину и перешли по суфф. ссылке
                addVertex(index, INT32_MAX, current_pos.vert);
                current_pos = getSuffixLink(current_pos);
            } else {// если есть ребро, то просто переходим по нему
                size_t edge = vertices[current_pos.vert]->edges[pattern[index]];
                current_pos = tree_position(edge, vertices[edge]->lengthFromParent() - 1);
                return;
            }
        } else {// мы находимся посередине ребра
            if (pattern[vertices[current_pos.vert]->finish - current_pos.char_index] == pattern[index]) {
                --current_pos.char_index;// если следующая буква по ребру совпадает с текущей, просто идем по ней
                return;
            } else {// иначе создаем 2 новые вершины. одна посередение ребра, другая новый лист из этой вершины
                int new_vert_on_edge = addVertex(vertices[current_pos.vert]->start,
                                                 vertices[current_pos.vert]->finish - current_pos.char_index,
                                                 vertices[current_pos.vert]->parent);
                addVertex(index, INT32_MAX, new_vert_on_edge);
                vertices[current_pos.vert]->start = vertices[current_pos.vert]->finish - current_pos.char_index;
                vertices[current_pos.vert]->parent = new_vert_on_edge;
                vertices[new_vert_on_edge]->edges[pattern[vertices[current_pos.vert]->start]] = current_pos.vert;
                if (found_link != -1) {// если была вершина, ожидающая суфф. ссылку, то для нее суфф. ссылка
                    // это новая вершина на ребре
                    vertices[found_link]->suffix_link = new_vert_on_edge;
                }
                found_link = new_vert_on_edge;// для новой вершины на ребре будем искать суфф. ссылку
                current_pos = getSuffixLink(tree_position(new_vert_on_edge, 0));
            }
        }
    }
}

SuffixTree::SuffixTree(const string &text) {
    pattern = text;
    vertices.push_back(std::make_unique<vertex>(0, 0, -1));
    for (int index = 0; index < pattern.size(); ++index) {
        doNextStep(index);// делаем шаги по одному
    }
}

void SuffixTree::dfsWithVertexDescription(int vertex, size_t divide_index, vector<int> &dfs_numeration, int &numerator) {
    dfs_numeration[vertex] = ++numerator;// увеличиваем номер в обходе
    std::cout << ((vertices[vertex]->parent == 0) ? (0) : (dfs_numeration[vertices[vertex]->parent])) << ' ';
    // выводим предка
    if (vertices[vertex]->start <= divide_index && vertices[vertex]->finish > divide_index) {
        vertices[vertex]->finish = divide_index + 1;// если встретили доллар на ребре
        vertices[vertex]->edges.clear();
    }
    if (vertices[vertex]->start <= divide_index) {// попали в первую строку
        std::cout << 0 << ' ' << vertices[vertex]->start << ' ' << vertices[vertex]->finish << '\n';
    } else {
        std::cout << 1 << ' ' << vertices[vertex]->start - divide_index - 1 << ' ';
        size_t real_right_bord = (vertices[vertex]->finish == INT32_MAX) ? (pattern.size() - 1)
                                                                   : (vertices[vertex]->finish - 1);
        // попали во вторую строку. Нужно проверить правую границу на бесконечность
        std::cout << real_right_bord - divide_index << '\n';
    }
    for (pair<char, int> new_vertex : vertices[vertex]->edges) {// продолжаем дфс
        dfsWithVertexDescription(new_vertex.second, divide_index, dfs_numeration, numerator);
    }
}

void SuffixTree::chunkByIndexAndPrintTree(size_t divide_index) {
    std::cout << vertices.size() << '\n';// выводим число вершин
    vector<int> dfs_numeration(vertices.size(), -1);
    int numerator = 0;                                     //
    for (pair<char, int> new_vertex : vertices[0]->edges) {// идем дфсом и выводим информацию о ребрах и вершинах
        dfsWithVertexDescription(new_vertex.second, divide_index, dfs_numeration, numerator);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    string pattern1, pattern2;
    std::cin >> pattern1 >> pattern2;
    string common_pattern = pattern1 + pattern2;
    SuffixTree a = SuffixTree(common_pattern);
    a.chunkByIndexAndPrintTree(pattern1.size() - 1);
    // по идее, здесь можно сделать так, чтобы не передавать лишний раз эту информацию в класс. Но хочется в
    // качестве класса иметь чистое суфф. дерево, которое можно будет применять к различным задачам (где будет
    // одна, а не две строки, например).
}


