#include <iostream>
#include <vector>

//Алгоритм -- по факту, в данной задачче нам нужно проверить граф звонков на ацикличность, а также, если граф ацикличен, провести топ-сорт


bool In_white(std::vector<char> &colors) {
    for (int j = 0; j < colors.size(); ++j) {
        colors[j] = 'w';                               // тут идет перекраска цветов в белый (начальное состояние)
    }
    return true;
}

bool Dfs(std::vector<std::vector<int>> &list, int start_vert, std::vector<char> &colors, std::vector<int> &res) { // поиск в глубину, проверяющий ацикличность
    colors[start_vert] = 'g';  // перекраска в серый цвет

    for (int i = 0; i < list[start_vert].size(); ++i) {
        if(colors[list[start_vert][i]] == 'g'){
            return false;    // если есть ребро в серую вершину -- мы проиграли
        } else if(colors[list[start_vert][i]] == 'w'){
            if(!Dfs(list, list[start_vert][i],colors,res)){
                return false; // если цвет следующей вершины -- белый, и в ней нашлась ацикличность -- мы програли
            }
        }
    }

    colors[start_vert] = 'b';  // красим в черный
    res.push_back(start_vert); // пушим в массив ответов
    return true;  // если мы дошли до этого момента, в данной слабой компоненте связности циклов нет!
}
void Topological_sort(std::vector<std::vector<int>> &list, std::vector<char> &colors, std::vector<int> &res){ // топ-сорт
    res.clear(); // так как в начале мы проверяли только ацикличность, но записывались ответы, их надо очистить
    In_white(colors); // красим все вершины обратно в белый
    for(int i  = 0; i < list.size();++i){
        if(colors[i] == 'w'){
            bool a = Dfs(list,i,colors,res); // дфс для топ-сорта
        }
    }
}
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);


    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> list(n);
    std::vector<bool> in_vert(n, false);
    for (int i = 0; i < m; ++i) {   // ввод данных
        int a, b;
        std::cin >> a >> b;
        in_vert[b] = true;
        list[a].push_back(b);
    }



    std::vector<int> res; // массив результатов при полож ответе
    std::string ans = "YES";
    std::vector<char> colors(n, 'w'); // цвета вершин с исходным


    for (int i = 0; i < n; ++i) {
        if (i == 0 || !in_vert[i]) {
            if (!Dfs(list,i,colors,res)) {    // проверяем ацикличность
                ans = "NO";
                break;
            }
        }
    }

    std:: cout << ans << '\n';
   if (ans == "YES"){
       Topological_sort(list,colors,res);  // ответ положительный -- топ-сорт + вывод
       for (int i = n-1; i > -1; --i) {
           std::cout << res[i] << ' ';
       }
   }
}
