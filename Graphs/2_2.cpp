#include <iostream>
#include <vector>
#include <queue>
//Рику необходимо попасть на межвселенную конференцию. За каждую телепортацию он платит бутылками лимонада,
// поэтому хочет потратить их на дорогу как можно меньше (он же всё-таки на конференцию едет!).
// Однако после K перелетов подряд Рика начинает сильно тошнить, и он ложится спать на день. Ему известны все существующие телепортации.
// Теперь Рик хочет найти путь (наименьший по стоимости в бутылках лимонада), учитывая, что телепортация не занимает времени,
// а до конференции осталось 10 минут (то есть он может совершить не более K перелетов)!
const int INF = 1000000000;

class CompareClass {
public:
    bool operator()(std::vector<int> first, std::vector<int> second) { // сравнение для очереди
        return first[0] > second[0];
    }
};

int Dist(int n, int start, int end, std::vector<std::vector<std::pair<int, int>>> &edges, int k) {
    std::vector<int> dist(n, INF);
    dist[start] = 0;
    std::vector<int> first = {0, start, 0};
    std::priority_queue<std::vector<int>, std::vector<std::vector<int>>, CompareClass> queue; // очередь
    queue.push(first);
    while (!queue.empty()) {
    // Интересная мысль добавить в Дейкстру ограничитель на длинну пути, но тогда немного запутанно становится с кучей массивов. Проще было бы
    // Всё же реализовать Беллмана-Форда, хотя по факту Дейкстра с ограничением по путям примерно так и будет идти, только экономнее по итерациям и затратнее по памяти.
        int vert = queue.top()[1], this_dist = queue.top()[0], now_k = queue.top()[2]; // извлекли данные с верхней вершины
        queue.pop();
        if (now_k + 1 < k) { // проверяем условие на текущеее к, чтобы не добавить лишнее
            for (int i = 0; i < edges[vert].size(); ++i) {
                int next = edges[vert][i].first, weight = edges[vert][i].second;
                if (this_dist + weight < dist[next]) {
                    dist[next] = this_dist + weight;
                    queue.push({this_dist + weight, next, now_k + 1});
                }
            }
        } else {
            for (int i = 0; i < edges[vert].size(); ++i) {
                int next = edges[vert][i].first, weight = edges[vert][i].second;
                if (this_dist + weight < dist[next]) {
                    dist[next] = this_dist + weight;
                }
            }
        }
    }
    return (dist[end] != INF ? dist[end] : -1); // вывод ответа
}


int main() {
    int n, m, k, start, end;
    std::cin >> n >> m >> k >> start >> end;
    std::vector<std::vector<std::pair<int, int>>> edges(n);
    for (int i = 0; i < m; ++i) {
        int s, f, p;
        std::cin >> s >> f >> p;
        std::pair <int,int> par;
        par.first = f - 1, par.second = p;
        edges[s - 1].push_back(par);
    }
    int a = 0;
    std::cout << Dist(n, start - 1, end - 1, edges, k);
}

