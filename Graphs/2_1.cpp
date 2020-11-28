//Рик и Морти снова бороздят просторы вселенных, но решили ограничиться только теми, номера которых меньше M. 
//Они могут телепортироваться из вселенной с номером z во вселенную (z+1) mod M за a бутылок лимонада или во вселенную (z^2+1) mod M за b бутылок лимонада.
// Рик и Морти хотят добраться из вселенной с номером x во вселенную с номером y. Сколько бутылок лимонада отдаст Рик за такое путешествие, если он хочет потратить их как можно меньше?

//Формат ввода
//В строке подряд даны количество бутылок a за первый тип телепортации, количество бутылок b за второй тип телепортации, 
//количество вселенных M, номер стартовой вселенной x, номер конечной вселенной y.

//Выведите одно число — количество бутылок лимонада, которые отдаст Рик за такое путешествие.

#include <iostream>
#include <vector>
#include <queue>

const int INF = 1000000000;// Для этого есть специальные константы по типу INT_MAX, которые кстати ещё и подставляются под нужный компилятор.
//В целом же для нахождения минимума лучше брать первый элемент диапазона, или в твоём случае сумму длинн всех рёбер, что точно будет max

class CompareClass {
public:
    bool operator()(std::pair<int, int> first, std::pair<int, int> second) { // сравнение для очереди
        return first.second > second.second;
    }
};

int Dist(int start, int end, long long n, int a, int b) {
    std::vector<int>  dist(n, INF);
    dist[start] = 0;
    std::pair<int, int> first(start, 0);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, CompareClass> queue; // очередь
    queue.push(first);
    while (!queue.empty()) {
        long long vert = queue.top().first;
        long long dist_vert = queue.top().second;
        queue.pop();
        if(dist_vert > dist[vert]){
            continue;
        }
        long long adj_first = (vert + 1 + n) % n;
        long long adj_second = (vert * vert + 1 + n) % n; // наши два ребра
        if (dist[vert] + a < dist[adj_first]) {
            dist[adj_first] = dist[vert] + a;
            queue.push(std::pair<int, int>(adj_first, dist[adj_first]));
        }
        if (dist[vert] + b < dist[adj_second]) {
            dist[adj_second] = dist[vert] + b;
            queue.push(std::pair<int, int>(adj_second, dist[adj_second]));
        }
    }

    return dist[end]; // ответ
}

int main() {
    int a, b, start, end;
    long long n;
    std::cin >> a >> b >> n >> start >> end;
    std::cout << Dist(start, end, n, a, b);


}
