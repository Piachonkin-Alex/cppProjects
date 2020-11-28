#include <iostream>
#include <vector>

//Рик отправляет Морти в путешествие по N вселенным. У него есть список всех существующих однонаправленных телепортов.
// Чтобы Морти не потерялся, Рику необходимо узнать, между какими вселенными существуют пути, а между какими нет.
// Помогите ему в этом!
// Используем алгоритм Флойда. Если ребра нет -- расстояние бесконечность. Все)
const int INF = 100000000;

void Floyd(int n, std::vector<std::vector<int>> &edges) { // сообственно флойд
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (edges[i][j] > edges[i][k] + edges[k][j]) {
                    edges[i][j] = edges[i][k] + edges[k][j];
                }
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int n;
    std::cin >> n;
    std::vector<std::vector<int>> edges(n);
    for (int i = 0; i < n; ++i) {
        std::string a;
        std::cin >> a;
        for (int j = 0; j < n; ++j) {
            if (a[j] == '0') {
                edges[i].push_back(INF);
            } else {
                edges[i].push_back(1);
            }
        }
    }
    Floyd(n, edges); // ввод вывод
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << (edges[i][j] < INF ? 1 : 0);
        }
        std::cout << '\n';
    }
}
