#include <iostream>
#include <vector>
#include <queue>
//C. Wake Me Up When Rickember Ends
//Ограничение времени	0.25 секунд
//Ограничение памяти	256Mb
//Ввод	стандартный ввод или input.txt
//Вывод	стандартный вывод или output.txt
//Сейчас Рику надо попасть из вселенной с номером S во вселенную с номером F. Он знает все существующие телепорты,
// и казалось бы нет никакой проблемы. Но, далеко не секрет, что за свою долгую жизнь Рик поссорился много с кем.
// Из своего личного опыта он знает, что при телепортациях есть вероятность, что его заставят ответить за свои слова.
//Если Рик знает вероятности быть прижатым к стенке на всех существующих телепортациях,
// помогите ему посчитать минимальную вероятность, что он всё-таки столкнется с неприятностями.

// Идея -- минимум шанс быть избитым <-> макс шанс быть не избитым. Запускаем Дейкстру по максимуму шанса быть не избитым
// Используем правило умножения (для вероятностей так)
const long double INF = -1.00000000000;

class CompareClass {
public:
    bool operator()(std::pair<int, long double> first, std::pair<int,long double> second) { // сравнение
        return first.second < second.second;
    }
};

long double MinProbability(int n, int start, int end, std::vector<std::vector<std::pair<int, long double>>> &edges) {
    std::vector<long double> dist(n, INF);
    dist[start] = 1.0000000000;
    std::pair<int, long double> first(start, dist[start]);
    std::priority_queue <std::pair <int,long double>, std::vector <std::pair<int,long double >>, CompareClass> queue; // очередь
    // Учитывая длинну строки в шаблонных скобках, было бы приятно прочитать от чего именно очередь (идейно, не в смысле от векторов)
    queue.push(first);
    while (!queue.empty()) {
        int vert = queue.top().first;
        long double now_dist = queue.top().second; // извлечение из очереди
        queue.pop();
        if (now_dist < dist[vert]) {
            continue;
        }
        for (size_t i = 0; i < edges[vert].size(); ++i) {
            int next = edges[vert][i].first;
            long double good_prob = edges[vert][i].second;
            if (now_dist * good_prob > dist[next]) { // подсчет новой вероятности
                dist[next] = now_dist * good_prob;
                queue.push({next, dist[next]});
            }
        }
    }
    return 1.00000000 - dist[end]; // вывод мин шанса быть избитым
}

int main() {
    int n, m, start, end;
    std::cin >> n >> m >> start >> end;
    std::vector<std::vector<std::pair<int, long double >>> edges(n);
    for (int i = 0; i < m; ++i) {
        int s, f;
        long double p = 0.00000000000;
        std::cin >> s >> f >> p;
        edges[s - 1].push_back({f - 1, 1 - p/100});
        edges[f - 1].push_back({s - 1, 1- p/100});
    }
    long double res =  MinProbability(n, start - 1, end - 1, edges);
    std::cout << res;
}
