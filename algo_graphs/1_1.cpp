#include <iostream>
#include <vector>
#include <queue>
//Леон и Матильда собрались пойти в магазин за молоком.
//Их хочет поймать Стэнсфилд, поэтому нашим товарищам нужно сделать это как можно быстрее.
// Каково минимальное количество улиц, по которым пройдёт хотя бы один из ребят (либо Матильда, либо Леон, либо оба вместе)?

// Решение -- Рассмотрим вершину а, из которой леон и матильда идут уже вместе за молоком. тогда, формула расстояния будет
// равна сумме расстояний от а до молока, от а до стартовой клетки леона и от а до стартовой клетки матильды.
// значит, задача сводится к поиску клетки, откуда эта сумма минимальна. Значит, делаем 3 бфса из вершин с молоком леоном и матильдой,
// считая расстояния от них до каждой из клеток и суммируем резы. минимум -- наш ответ.


void Bfs(std::vector<std::vector<int>> &list, std::vector<int> &distances, int start) {
    std::queue<int> q;  // стандартный бфс, только в конце мы складываем полученные расстояния с теми расстояниями, которые были расчитпны ранее от других вершин.
    q.push(start);
    std::vector<bool> used(list.size(), false);
    std:: vector <int> dist(list.size());
    dist[start] = 0;
    used[start] = true;

    while (!q.empty()) {   // не знаю, что написать типичный бфс
        int vert = q.front();
        q.pop();
        for (int i = 0; i < list[vert].size(); ++i) {
            int next = list[vert][i];
            if (!used[next]) {
                used[next] = true;
                q.push(next);
                dist[next] = dist[vert] + 1;   // подсчет расстояний
            }
        }
    }

    for(int i = 0; i < list.size();++i){
        distances[i] += dist[i];   // сложение с предыдущими
    }
}

int main() {
    int n, m;
    int leon, matilda, milk;
    std::cin >> n >> m;
    std::cin >> leon >> matilda >> milk; // ввод
    std::vector<std::vector<int>> list(n);
    for (int i = 0; i < m; ++i) {
        int a, b;
        std::cin >> a >> b;
        list[a - 1].push_back(b - 1);
        list[b - 1].push_back(a - 1);
    }


    std::vector<int> distances(n,0); // массив на сумму дистанций
    Bfs(list, distances, milk - 1);
    Bfs(list, distances, leon - 1);   // расчет расстояний от матильды леона и молока
    Bfs(list, distances, matilda - 1);
    int minimal = distances[0];
    for (int i = 1; i < n; ++i) {
        if (distances[i] < minimal) {
            minimal = distances[i];   // подсчет минимума
        }
    }
    std::cout << minimal; // вывод.
}
