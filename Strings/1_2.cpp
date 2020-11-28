#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// #define _DEBUG
// Строка называется палиндромом, если она одинаково читается как слева направо,
// так и справа налево. Например, «abba» – палиндром, а «roman» – нет.
//
//Для заданной строки s длины n (1 ≤ n ≤ 10^5) требуется подсчитать число пар (i, j),
// 1 ≤ i < j ≤ n, таких что подстрока s[i..j] является палиндромом.

//Входной файл содержит одну строку s длины n, состоящюю из маленьких латинских букв.

//В выходной файл выведите искомое число подстрок-палиндромов.

// Решение -- алгорит Манакера. Идея -- рассматривать центр и радиус палинжрома,
// а также использовать информацию последнего полученного палиндрома. (аналогично z-функции)
// Рассматривается 2 случая -- четная и нечетная длина

// Примечание. 10^5*10^5 = 10^10 = long long

using std::string;
using std::vector;
using ll = long long;

ll CalcOddPalindromes(const string &s) {
    vector<int> maxi_radius(s.size());
    // палиндром максимального радиуса, центром которого является i-ый символ
    int l = 0, r = 0;
    ll answer = 0;

    for (int i = 0; i < s.size(); ++i) {
        int border = (i <= r) ? (std::min(maxi_radius[l + r - i], r - i) + 1) : 1;
        // здесь мы и используем информацию из последнего палиндрома
        // замечание. Проверять выход за границу последнего найденного палиндрома
        // можно только по правой стороне. Нетрудно заметить, что если бы у нас был выход
        // за границу слева, то последний палиндром найден неправильно
        while (i - border >= 0 && i + border < s.size() && s[i - border] == s[i + border]) {
            ++border;
        }
        maxi_radius[i] = --border;
        answer += maxi_radius[i];
        if (i + border > r) {
            r = i + border;
            l = i - border;
        }
    }
    return answer;
}

ll CalcEvenPalindromes(const string &s) {
    vector<int> max_radius(s.size());
    int l = 0, r = -1;
    ll answer = 0;
    // при четной длине палиндромов нужно выбирать смещенный на одну клетку центр.
    // В данном решении центром считается клетка правее
    for (int i = 0; i < s.size(); ++i) {
        int border = (i <= r) ? (std::min(max_radius[l + r - i + 1], r - i + 1) + 1) : 1;
        while (i - border >= 0 && i + border - 1 < s.size() && s[i - border] == s[i + border - 1]) {
            ++border;
        }
        max_radius[i] = --border;
        answer += max_radius[i];
        if (i + border - 1 > r) {
            l = i - border;
            r = i + border - 1;
        }
    }
    return answer;
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
#ifdef _DEBUG
    for (int i = 1; i <= 3; ++i) {
        std::ifstream input_file("input" + std::to_string(i) + ".txt");
        std::ifstream answer_file("output" + std::to_string(i) + ".txt");
        string input_string;
        input_file >> input_string;

        if (!input_string.empty()) {
            ll odd = CalcOddPalindromes(input_string);
            ll even = CalcEvenPalindromes(input_string);
            ll right_answer = -1;
            answer_file >> right_answer;

            if (right_answer >= 0) {
                assert(right_answer == odd + even);
            }
        }
    }
#else
    string text;
    std::cin >> text;
    ll number_odd_palindroms = CalcOddPalindromes(text);
    ll number_even_palindroms = CalcEvenPalindromes(text);
    std::cout << number_odd_palindroms + number_even_palindroms;
#endif
}
