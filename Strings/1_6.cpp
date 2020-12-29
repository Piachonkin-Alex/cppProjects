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
using ll = long long;
class SuffixTree {
public:
    explicit SuffixTree(const string &text);

    string FindKStatisticsOfTwOStrings(ll k, size_t divide_index);// фцнкция, выводящая k-ую общую подстроку,
    // если такова имеется, иначе пустую


private:
    struct tree_position {
        // структурка, определяющая положение в дереве
        size_t vert;      // следующая вершина
        size_t char_index;// количество символов, которое нужно пройти до вершины
        tree_position();

        tree_position(size_t vert_id, size_t index);
    };

    int addVertex(size_t left_bord, size_t right_bord, int parent);// функция добавления вершины

    tree_position getSuffixLink(tree_position position);// функция расчета суфф. ссылки

    void doNextStep(int index);// функция следующей фазы

    pair<bool, bool> deleteBadVertices(int vertex, size_t index);// является ли подстрока,
    // соответсвующая данной вершине, подстрокой первой и второй строки, которые разделены в позиции index.
    // если нет, то эту вершину надо выбросить

    void getSubtreesCardinalitiesOfSubstrings(int vertex, vector<ll> &cardinalities);//  подсчет мощностей поддеревьев


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

SuffixTree::tree_position SuffixTree::getSuffixLink(tree_position position) {
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


pair<bool, bool> SuffixTree::deleteBadVertices(int vertex, size_t index) {
    bool find_substr_of_first_pattern = false;
    bool find_substr_of_second_pattern = false;
    vector<char> remove_edges;// выкинутые дети
    for (pair<char, int> new_vertex : vertices[vertex]->edges) {
        if (vertices[new_vertex.second]->edges.empty()) {// если вершина лист
            if (vertices[new_vertex.second]->start > index) {
                find_substr_of_second_pattern = true;// соответствует второй строке
            } else {
                find_substr_of_first_pattern = true;// соответсвует первой строке
            }
            remove_edges.push_back(new_vertex.first);// лист очевидно удаляется всегда
        } else {                                     // иначе вычисляем рекурсивно
            pair<bool, bool> res_of_child = deleteBadVertices(new_vertex.second, index);
            if (res_of_child.first) {
                find_substr_of_first_pattern = true;
            }
            if (res_of_child.second) {
                find_substr_of_second_pattern = true;
            }
            if (!(res_of_child.first && res_of_child.second)) {
                remove_edges.push_back(new_vertex.first);// если для ребенка хотя бы одна из двух подстрок не найдена, удаляем ее.
            }
        }
    }
    for (char edge : remove_edges) {// удаление
        vertices[vertex]->edges.erase(edge);
    }
    return {find_substr_of_first_pattern, find_substr_of_second_pattern};
}


void SuffixTree::getSubtreesCardinalitiesOfSubstrings(int vertex, vector<ll> &cardinalities) {
    if (vertices[vertex]->edges.empty()) {
        cardinalities[vertex] = 0;// попали в лист -> мощность 0
    } else {
        for (pair<char, int> new_vertex : vertices[vertex]->edges) {// для каждого ребенка прибавляем его мощность и длину ребра
            cardinalities[vertex] += vertices[new_vertex.second]->lengthFromParent();
            getSubtreesCardinalitiesOfSubstrings(new_vertex.second, cardinalities);
            cardinalities[vertex] += cardinalities[new_vertex.second];
        }
    }
}

string SuffixTree::FindKStatisticsOfTwOStrings(ll k, size_t divide_index) {
    deleteBadVertices(0, divide_index);// удаляем лишние вершины
    vector<ll> subtrees_cardinalities(vertices.size(), 0);
    getSubtreesCardinalitiesOfSubstrings(0, subtrees_cardinalities);
    string result;
    ll current_statistic = 0;
    int now_vertex = 0;// вычисляем k-ую статистику как в дереве поиска
    while (true) {
        for (pair<char, int> new_vertex : vertices[now_vertex]->edges) {
            if (current_statistic + subtrees_cardinalities[new_vertex.second] +
                        vertices[new_vertex.second]->lengthFromParent() <
                k) {// проверка на то, нужно ли заходить в поддерево ребенка
                current_statistic += subtrees_cardinalities[new_vertex.second] + vertices[new_vertex.second]->lengthFromParent();
            } else {
                if (current_statistic + vertices[new_vertex.second]->lengthFromParent() >= k) {
                    // проверяем, заканчивается ли статистика на этом ребре
                    std::copy(pattern.begin() + vertices[new_vertex.second]->start,
                              pattern.begin() + vertices[new_vertex.second]->start + k - current_statistic,
                              std::back_inserter(result));// копируем через std::copy ту часть ребра, которая нужна
                    return result;
                } else {// копируем все ребро
                    std::copy(pattern.begin() + vertices[new_vertex.second]->start,
                              pattern.begin() + vertices[new_vertex.second]->finish, std::back_inserter(result));
                    now_vertex = new_vertex.second;
                    current_statistic += vertices[new_vertex.second]->lengthFromParent();
                    break;
                }
            }
        }
        if (now_vertex == 0) {// k-статистики нет, если мы прошли всех детей корня, и не зашли ни в одного из них
            return "";
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    string pattern1, pattern2;
    ll k;
    std::cin >> pattern1 >> pattern2;
    std::cin >> k;
    string common_pattern = pattern1 + "#" + pattern2 + "$";
    SuffixTree a = SuffixTree(common_pattern);
    string res = a.FindKStatisticsOfTwOStrings(k, pattern1.size());
    if (res.empty()) {
        std::cout << -1;
    } else {
        std::cout << res;
    }
}

