#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>

// Шаблон поиска задан строкой длины m, в которой кроме обычных символов могут встречаться символы “?”.
// Найти позиции всех вхождений шаблона в тексте длины n. Каждое вхождение шаблона предполагает,
// что все обычные символы совпадают с соответствующими из текста, а вместо символа “?” в
// тексте встречается произвольный символ. Гарантируется, что сам “?” в тексте не встречается.
//Время работы - O(n + m + Z), где Z - общее число вхождений подстрок шаблона
// “между вопросиками” в исходном тексте. m ≤ 5000, n ≤ 2000000.



using std::pair;
using std::queue;
using std::string;
using std::vector;

// все STL-контейнеры, которые нам понадобятся

const int SIZE = 26;// размер алфавита

const int DIFF_INT_CHAR = 97;// int('a') = 97 -> для обращения по индексу 'a' нужно отнять 97


class AhoCorasick {
public:
    AhoCorasick();

    void addPattern(const string &pattern);// добавление паттерна в бор

    void constructSuffixAndLinks();// построение суфф. ссылок

    void constructExitLinks();// построение выходных ссылок

    vector<vector<int>> findCoincedences(const string &str); /*эта функция в данной задаче не нужна, но она возвращает
     для каждого паттерна массив индексов его вхождений. */

    ~AhoCorasick() = default;

    class vertex;

protected:
    vector<std::unique_ptr<vertex>> vertices;
    vertex *root;           // корень
    vector<string> patterns;// паттерны

    vertex *transition(vertex *vert, char letter);// функция перехода

    void findSuffixLink(vertex *vert);// вычисление суфф. ссылки для вершины (с учетом того, что вычислен прошлый уровень)

    void findExitLink(vertex *vert);// вычисление вых. ссылки для вершины
};

class PatternsQuestion : private AhoCorasick {// класс решения задачи. Наследуется от автомата
public:
    explicit PatternsQuestion(const string &pattern);// ввод шаблона

    void findAllCoincedences(const string &text);// ответ по тексту

private:
    string question_pattern;
    std::vector<int> templates_placement;// здесь хранятся сдвиги подшаблонов
};


class AhoCorasick::vertex {
public:
    vertex() : parent_edge('#') {// конструктор пустой вершины (это только корень)
        edges = vector<vertex *>(SIZE, nullptr);
        parent = this;
    };

    vertex(char letter, vertex *parent) : parent_edge(letter),
                                          parent(parent) {
        edges = vector<vertex *>(SIZE, nullptr);
    };
    vector<vertex *> edges;
    vertex *suffix_link = nullptr;
    vertex *exit_link = nullptr;
    vertex *parent;
    char parent_edge;
    vector<int> terminal_indices = vector<int>();
};

AhoCorasick::AhoCorasick() {
    vertices.push_back(std::make_unique<vertex>());
    root = vertices[0].get();
}

void AhoCorasick::addPattern(const string &pattern) {
    patterns.push_back(pattern);
    vertex *now_vert = root;
    for (char letter : pattern) {
        // идем по буквам
        if (now_vert->edges[letter - DIFF_INT_CHAR] == nullptr) {
            // если листа еще не создано -- создаем
            now_vert->edges[letter - DIFF_INT_CHAR] = new vertex(letter, now_vert);
            vertices.push_back(std::unique_ptr<vertex>(now_vert->edges[letter - DIFF_INT_CHAR]));
        }
        now_vert = now_vert->edges[letter - DIFF_INT_CHAR];
    }
    now_vert->terminal_indices.push_back(patterns.size() - 1);
    // в листе, котором остановились, добавляем индекс паттерна
}


AhoCorasick::vertex *AhoCorasick::transition(vertex *vert, char letter) {
    if (vert == nullptr) {
        return root;
    }
    if (vert->edges[letter - DIFF_INT_CHAR] != nullptr) {
        return vert->edges[letter - DIFF_INT_CHAR];
    } else if (vert == vert->parent) {// проверка на то, в корне ли мы (условие parent->vert == vert верно только там)
        return vert;
    } else {
        return vert->edges[letter - DIFF_INT_CHAR] = transition(vert->suffix_link, letter);
    }
}

void AhoCorasick::findSuffixLink(vertex *vert) {
    if (vert->parent == vert) {// корень
        vert->suffix_link = nullptr;
    } else {
        vert->suffix_link = transition(vert->parent->suffix_link, vert->parent_edge);
    }
}

void AhoCorasick::findExitLink(vertex *vert) {
    if (vert->suffix_link == nullptr) {// если корень
        return;
    }
    if (!vert->suffix_link->terminal_indices.empty()) {
        vert->exit_link = vert->suffix_link;// в суфф. ссылке есть терминал
    } else {
        vert->exit_link = vert->suffix_link->exit_link;
    }
}

void AhoCorasick::constructSuffixAndLinks() {
    // строим ссылки через BFS
    queue<vertex *> q;
    q.push(root);
    while (!q.empty()) {
        vertex *vert = q.front();
        findSuffixLink(vert);
        findExitLink(vert);
        q.pop();
        for (int i = 0; i < SIZE; ++i) {
            if (vert->edges[i] != nullptr) {
                q.push(vert->edges[i]);
            }
        }
    }
}

vector<vector<int>> AhoCorasick::findCoincedences(const string &str) {
    vector<vector<int>> answer(patterns.size());
    vertex *now_vert = root;
    for (int i = 0; i < str.size(); ++i) {
        char letter = str[i];
        now_vert = transition(now_vert, letter);
        vertex *ex_link = now_vert;
        while (ex_link != nullptr) {
            for (int terminal : ex_link->terminal_indices) {
                answer[terminal].push_back(i - patterns[terminal].size() + 1);
            }
            ex_link = ex_link->exit_link;
        }
    }
    return answer;
}

PatternsQuestion::PatternsQuestion(const string &pattern) {
    question_pattern = pattern;
    string now_str;
    for (int i = 0; i < pattern.size(); ++i) {
        // разделение шаблона с '?' на подшаблоны
        if (pattern[i] != '?') {
            now_str += pattern[i];
        } else if (!now_str.empty()) {
            templates_placement.push_back(i - now_str.size());
            addPattern(now_str);
            now_str = "";
        }
    }
    if (!now_str.empty()) {
        templates_placement.push_back(pattern.size() - now_str.size());
        addPattern(now_str);
    }
    constructSuffixAndLinks();
    // постоение автомата после добавления всех паттернов
}

void PatternsQuestion::findAllCoincedences(const string &text) {
    vector<int> control_arr(text.size());// это массив, по которому считается ответ.
    // здесь в каждом индексе мы храним количество подшаблонов, которые с учетом сдвига начинаются здесь.
    // Если все подшаблоны начинаются из этого индекса, то  начиная с этого индекса лежит искомый шаблон.
    vertex *now_vert = root;
    // текущая вершина
    for (int i = 0; i < text.size(); ++i) {
        char letter = text[i];
        now_vert = transition(now_vert, letter);// переход по символу
        vertex *exit_link = now_vert;
        while (exit_link != nullptr) {// проход по выходным ссылкам, а в них -- по всем терминалам
            for (int pattern_index : exit_link->terminal_indices) {
                int true_index = i + 1 - patterns[pattern_index].size() - templates_placement[pattern_index];
                if (true_index >= 0) {
                    ++control_arr[true_index];
                }
            }
            exit_link = exit_link->exit_link;
        }
    }

    for (int j = 0; j <= static_cast<int>(control_arr.size()) - static_cast<int>(question_pattern.size()); ++j) {
        if (control_arr[j] == patterns.size()) {// вывод индексов вхождения исходного шаблона
            std::cout << j << ' ';
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    string pattern, text;
    std::cin >> pattern >> text;
    PatternsQuestion a(pattern);
    a.findAllCoincedences(text);
}

