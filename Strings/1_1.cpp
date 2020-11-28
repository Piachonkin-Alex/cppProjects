#include <iostream>
#include <string>
#include <vector>
// Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n.
// Время O(n + p), доп. память – O(p).
// p ≤ 30000, n ≤ 300000.
// Использовать один из методов:
// Вариант 1. С помощью префикс-функции;

//Шаблон, символ перевода строки, строка.

//Позиции вхождения шаблона в строке.




using std::string;
using std::vector;

vector<int> calcPatternPi(string &pattern) {
    vector<int> pref_func(pattern.size());
    // вычисление префикс-функции для шаблона
    int prev = 0;
    for (int i = 1; i < pref_func.size(); ++i) {
        while (prev > 0 && pattern[i] != pattern[prev]) {
            prev = pref_func[prev - 1];
        }

        if (pattern[i] == pattern[prev]) {
            pref_func[i] = ++prev;
        }
    }
    return pref_func;
}

void findPositions(string &pattern, vector<int> &pref_pi, string &text) {
    // вычисление префикс-фунции для текста,
    // используя шаблон, а также вывод индексов, где начинается шаблон.
    int prev = 0;
    for (size_t index = 0; index < text.size(); ++index) {
        while (prev > 0 && text[index] != pattern[prev]) {
            prev = pref_pi[prev - 1];
        }

        if (text[index] == pattern[prev]) {
            ++prev;
        }
        if (prev == (pattern.size() - 1)) {
            // если значение префикс функции равно длине шаблона без #, то мы нашли шаблон
            int position = static_cast<int>(index) - pattern.size() + 2;
            std::cout << position << ' ';
        }
    }
}

void solve(string &pattern, string &text) {
    vector<int> pref_func = calcPatternPi(pattern);
    // расчет префикс-функции для шаблона
    pattern += '#';
    // добавляем несуществующий символ в строке.
    findPositions(pattern, pref_func, text);
    // вычисляем префикс-функцию для текста, параллельно получая ответ
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    string pattern, text;       // ввод
    std::cin >> pattern >> text;// решение
    solve(pattern, text);
}

