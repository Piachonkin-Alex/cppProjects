// Последние достижения в технологии синтеза ДНК позволили провести
// эксперимент по созданию биороботов.
//
//Для облегчения задачи создания ПО для управления роботами было принято решение,
// что их ДНК будет состоять из M = 2n символов для некоторого n ≥ 2.
// Кроме этого, по техническим причинам это будет не обычная строка,
// а циклическая, то есть её можно начинать читать с любой позиции.
//
//Одной из целей эксперимента является изучение мутаций биороботов.
// В результате продолжительных наблюдений было найдено много различных
// видов роботов. Для понимания процесса мутации учёным необходимо решить
// следующую задачу. Для ДНК двух роботов требуется определить коэффициент
// их похожести. Он вычисляется, как максимальное количество совпадающих
// символов при наилучшем совмещении этих ДНК. Чем больше символов совпадает,
// тем лучше совмещение.
//
//Требуется написать программу, которая найдёт наилучшее совмещение двух ДНК.
//
//Входные данные
//
//В первой строке входного файла записано одно число M (4 ≤ M ≤ 131072).
// В следующих двух строках записаны ДНК двух роботов. Обе ДНК — строки,
// состоящие ровно из M символов из множества {'A', 'C', 'G', 'T'}.
//
//Выходные данные
//
//В выходной файл выведите два числа — максимальное количество совпадающих
// символов и значение оптимального сдвига — неотрицательное количество
// символов второй ДНК, которые необходимо перенести из конца строки в
// её начало для достижения наилучшего совмещения.
#include<iostream>
#include<vector>
#include<math.h>
#include<map>
#include <algorithm>

using std::vector;
using ll = long long;


class ComplexNumber {
public:
  double real, imag;

  ComplexNumber(double realx, double imgx) : real(realx), imag(imgx) {}

  ComplexNumber(double realx) : real(realx), imag(0) {}

  ComplexNumber() = default;

  ComplexNumber operator+(const ComplexNumber &other) {
    return ComplexNumber(real + other.real, imag + other.imag);
  }


  ComplexNumber operator-(ComplexNumber other) {
    real -= other.real;
    imag -= other.imag;
    return *this;
  }

  ComplexNumber operator*(ComplexNumber other) {
    return ComplexNumber(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
  }

  ComplexNumber operator/(double coeff) {
    return ComplexNumber(real / coeff, imag / coeff);
  }

  ComplexNumber operator*=(ComplexNumber other) {
    *this = *this * other;
    return *this;
  }

  ComplexNumber operator/=(double coeff) {
    *this = *this / coeff;
    return *this;
  }
};


static const int lgn = 18;
static const int n = 1 << lgn;
vector<ComplexNumber> roots;
vector<int> bit_sort;

class Polynom {
public:

  Polynom(const vector<ComplexNumber> &dat) {
    data_ = dat;
    data_.resize(n);
  }

  Polynom(const vector<int> &data) {
    for (int coef: data)
      data_.emplace_back(coef);
    data_.resize(n);
  }

  vector<ComplexNumber> do_fft(vector<ComplexNumber> data) {
    for (int i = 0; i < n; ++i) {
      if (i < bit_sort[i]) {
        std::swap(data[i], data[bit_sort[i]]);
      }
    }

    for (int iter = 0; iter < lgn; iter++) {
      for (int pos = 0; pos < n; pos += (2 << iter)) {
        for (int offset = 0; offset < (1 << iter); offset++) {
          ComplexNumber first = data[pos + offset];
          ComplexNumber second = data[pos + offset + (1 << iter)] * roots[offset << (lgn - (iter + 1))];
          data[pos + offset] = first + second;
          data[pos + offset + (1 << iter)] = first - second;
        }
      }
    }
    return data;
  }

  vector<ComplexNumber> inv_fft(vector<ComplexNumber> &data) {
    data = do_fft(data);
    for (int i = 0; i < n; ++i) {
      data[i] /= n;
    }
    std::reverse(data.begin() + 1, data.end());
    return data;
  }

  Polynom operator*(const Polynom &other) {
    auto fft_a = do_fft(data_);
    auto fft_b = do_fft(other.data_);
    for (int i = 0; i < n; ++i) {
      fft_a[i] *= fft_b[i];
    }
    return inv_fft(fft_a);
  }

  vector<ComplexNumber> data_;
};

void do_precalc() {
  roots.resize(n);
  bit_sort.resize(n, 0);
  for (int i = 0; i < n; ++i) {
    roots[i] = ComplexNumber(cos(2.l * acos(-1.0) * i / n), sin(2.l * acos(-1.0) * i / n));
  }
  for (int mask = 0; mask < n; ++mask) {
    for (int j = 0; j < lgn; ++j) {
      bit_sort[mask] |= ((mask >> j) & 1) << (lgn - j - 1);
    }
  }
}

std::pair<int, int> calcResByPolinomCoeffs(int m, vector<vector<int>> coeffs) {
  int answer = -1, top_permutation_id = -1;
  for (int i = 0; i < m; ++i) {
    int cur = 0;
    for (int j = 0; j < coeffs.size(); ++j) {
      cur += coeffs[j][i];
    }
    if (cur > answer) {
      answer = cur;
      top_permutation_id = i;
    }
  }
  return std::make_pair(answer, top_permutation_id);
}

vector<vector<int>>
calcCoeffs(int m, const std::string &first, const std::string &second, const vector<char> &characters) {
  vector<vector<int>> coeffs(characters.size(), vector<int>(m));
  for (int cur = 0; cur < characters.size(); ++cur) {
    int str_size = static_cast<int>(first.size());
    vector<int> coeffs1(str_size), coeffs2(str_size);
    for (int i = 0; i < str_size; ++i) {
      coeffs1[i] = (first[i] == characters[cur]);
      coeffs2[i] = (second[i] == characters[cur]);
    }
    Polynom ans = Polynom(coeffs1) * Polynom(coeffs2);
    for (int i = m - 1; i < 2 * m - 1; i++) {
      coeffs[cur][i - m + 1] = static_cast<int>(round(ans.data_[i].real));
    }
  }
  return coeffs;
}


void solve() {
  do_precalc();
  int m;
  std::cin >> m;
  std::string first, second;
  std::cin >> first >> second;
  std::reverse(second.begin(), second.end());
  first += std::string(m, '\n');
  second += second;
  vector<char> characters = {'A', 'C', 'T', 'G'};
  vector<vector<int>> coeffs = calcCoeffs(m, first, second, characters);
  std::pair<int, int> res = calcResByPolinomCoeffs(m, coeffs);
  std::cout << res.first << ' ' << res.second << '\n';
}

int main() {
  solve();
}

