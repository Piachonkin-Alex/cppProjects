#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using ll = long long;

//Дана строка длины n. Найти количество ее различных подстрок. Используйте
//суффиксный массив.
//
//Построение суффиксного массива выполняйте за O(n log n). Вычисление количества
//различных подстрок выполняйте за O(n).

// Используем везде лонги, так как в задаче не оговорено про ограничения
void zeroStep(uint32_t patt_size, const string &pattern, vector<ll> &suffix_arr,
              uint32_t &classes_number, vector<vector<ll>> &classes) {
    suffix_arr = vector<ll>(patt_size);
    vector<ll> sort_array(256); // сортировка подсчетом для одиночных символов
    for (uint32_t i = 0; i < patt_size; ++i) {
        ++sort_array[pattern[i]];
    }
    for (uint32_t i = 1; i < 256; ++i) {
        sort_array[i] += sort_array[i - 1];
    }
    for (uint32_t i = 0; i < patt_size; ++i) {
        suffix_arr[--sort_array[pattern[i]]] = i;
    }
    vector<ll> zero_step_classes(patt_size);
    zero_step_classes[suffix_arr[0]] = 0;
    classes_number = 1;
    for (uint32_t i = 1; i < patt_size; ++i) {
        if (pattern[suffix_arr[i]] != pattern[suffix_arr[i - 1]]) {
            ++classes_number;
        } // расчет классов эквивалентности для текущей стадии суф. массива
        zero_step_classes[suffix_arr[i]] = classes_number - 1;
    }
    classes.push_back(zero_step_classes);
}

void nextStep(uint32_t current_step, uint32_t &patt_size,
              vector<ll> &suffix_arr, uint32_t &number_of_classes,
              vector<vector<ll>> &classes) {
    vector<ll> permutation_by_second_element(patt_size);
    for (uint32_t i = 0; i < patt_size;
         ++i) { // сортировка по второму элементу. После этого достаточно провести
        // стабильную сортировку подсчетом по первому элементу
        permutation_by_second_element[i] =
                suffix_arr[i] - (1 << (current_step - 1));
        if (permutation_by_second_element[i] < 0) {
            permutation_by_second_element[i] += patt_size;
        }
    }
    vector<ll> sort_array(number_of_classes);
    for (uint32_t i = 0; i < patt_size; ++i) {
        ++sort_array[classes[current_step - 1][permutation_by_second_element[i]]];
    }
    for (uint32_t i = 1; i < number_of_classes; ++i) {
        sort_array[i] += sort_array[i - 1];
    } // стабильная сортировка подсчетом для первых элементов пары
    for (ll i = patt_size - 1; i >= 0;
         --i) { // uint32_t не можем взять, так как нужно попасть в -1
        suffix_arr[--sort_array[classes[current_step - 1]
        [permutation_by_second_element[i]]]] =
                permutation_by_second_element[i];
    }
    vector<ll> new_class(patt_size);
    new_class[suffix_arr[0]] = 0;
    number_of_classes = 1;
    for (uint32_t i = 1; i < patt_size; ++i) {
        uint32_t middle_elem_for_this =
                (suffix_arr[i] + (1 << (current_step - 1))) % patt_size;
        uint32_t middle_elem_for_prev =
                (suffix_arr[i - 1] + (1 << (current_step - 1))) % patt_size;
        if (classes[current_step - 1][suffix_arr[i - 1]] !=
            classes[current_step - 1][suffix_arr[i]] ||
            classes[current_step - 1][middle_elem_for_prev] !=
            classes[current_step - 1][middle_elem_for_this]) {
            ++number_of_classes;
        } // пересчет новых классов эквивалентности
        new_class[suffix_arr[i]] = number_of_classes - 1;
    }
    classes.push_back(new_class);
}

vector<ll> findLCP(uint32_t &patt_size, vector<ll> &suffix_arr,
                   vector<vector<ll>> &classes) {
    vector<ll> lcp(patt_size - 2); // поиск массива lcp. C учетом того, что
    // последний символ нас не интересует,
    // имеем что его длина равна patt_size - 2
    for (uint32_t i = 1; i < patt_size - 1; ++i) {
        ll this_element = suffix_arr[i];
        ll prev_element = suffix_arr[i + 1];
        for (ll k = classes.size() - 2; k >= 0; --k) {
            if (this_element >= patt_size || prev_element >= patt_size) {
                break;
            }
            if (classes[k][this_element] == classes[k][prev_element]) {
                lcp[i - 1] += 1 << k;
                this_element += 1 << k;
                prev_element += 1 << k;
            }
        }
    }
    return lcp;
}

ll solve(const string &pattern) {
    string pattern_with_$ = pattern + '\0';
    uint32_t patt_size = pattern.size() + 1;
    vector<ll> suffix_arr;
    vector<vector<ll>> classes; // добавляем пустой символ и инициализируем суфф.
    // массив и массив классов
    uint32_t current_classes_number;

    zeroStep(patt_size, pattern_with_$, suffix_arr, current_classes_number,
             classes);
    for (uint32_t j = 1; (1 << (j - 1)) < patt_size; ++j) {
        nextStep(j, patt_size, suffix_arr, current_classes_number, classes);
    } // делаем  log_2(patt_size) фаз

    vector<ll> lcp = findLCP(patt_size, suffix_arr, classes); // поиск массива lcp
    uint32_t result = patt_size * (patt_size - 1) / 2;
    // количество всех подстрок равно количеству всех подстрок (упорядоченных)
    // минус массив lcp количество всех упорядоченных подстрок равно n(n-1)/2
    // (арифм. прогрессия)
    for (uint32_t i = 0; i < patt_size - 2; ++i) {
        result -= lcp[i];
    }
    return result;
}

int main() {
    string pattern;
    std::cin >> pattern;
    std::cout << solve(pattern);
}
