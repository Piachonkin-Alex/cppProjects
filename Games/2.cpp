// Два игрока играют в настольную игру.
// Игровое поле представляет собой квадратный лабиринт, 8× 8 клеток.
// В некоторых клетках располагаются стенки. Один игрок управляет
// фишкой-терминатором, а второй — фишкой-беглецом. Игроки ходят по очереди,
// ходы пропускать нельзя (гарантируется, что ход всегда возможен).
// За один ход игрок может переместить свою фишку в любую из свободных клеток,
// расположенных рядом с исходной по горизонтали, вертикали или по диагонали
// (то есть ходом короля). Терминатор, кроме того, может стрелять в беглеца
// ракетами. Выстрел идет по прямой в любом направлении по горизонтали,
// вертикали или диагонали. Если беглец оказывается на линии выстрела
// терминатора и не прикрыт стенками, то терминатор незамедлительно делает
// выстрел (вне зависимости от того, чей ход), и беглец проигрывает.
// Начальное положение фишек задано. Первый ход делает беглец. Он выигрывает,
// если сделает ход с восьмой строки за пределы игрового поля, так как
// остальные границы поля окружены стенками.
//
//Вопрос задачи: может ли беглец выиграть при оптимальной игре обеих сторон?
//
//Формат ввода
//Во входном файле задано игровое поле. Свободная клетка обозначена цифрой 0,
// а клетка со стенкой — цифрой 1. Клетка, в которой находится беглец,
// обозначена цифрой 2, а клетка с терминатором — цифрой 3.
//
//Формат вывода
//В выходной файл выведите число 1, если беглец выигрывает, и -1 — в противном
//случае.

#include <bits/stdc++.h>

using namespace std;

struct state {
  int runner = 0;
  int terminator = 0;
  int step = 0;

  state() = default;

  state(int runner, int term, int step)
      : runner(runner), terminator(term), step(step) {}
};

const int sz = 8;
vector<state> arr[sz * sz][sz * sz][2];
int degree[sz * sz][sz * sz][2];
int win[sz * sz][sz * sz][2];
int lose[sz * sz][sz * sz][2];

void constructGraph(const vector<string> &field) {
  for (int i = 0; i < sz * sz; ++i) {
    int stri = i / sz;
    int stolbi = i % sz;
    if (field[stri][stolbi] == '1') {
      continue;
    }
    for (int j = 0; j < sz * sz; ++j) {
      int strj = j / sz;
      int stolbj = j % sz;
      if (field[strj][stolbj] == '1') {
        continue;
      }
      if (stolbi == stolbj) {
        int minim = min(stri, strj);
        int maxim = max(stri, strj);
        bool flag = true;
        for (int t = minim; t <= maxim; ++t) {
          if (field[t][stolbi] == '1') {
            flag = false;
            break;
          }
        }
        if (flag) {
          lose[i][j][0] = 1, win[i][j][1] = 1;
        }
      }
      if (stri == strj) {
        int minim = min(stolbi, stolbj);
        int maxim = max(stolbi, stolbj);
        bool flag = true;
        for (int t = minim; t <= maxim; ++t) {
          if (field[stri][t] == '1') {
            flag = false;
            break;
          }
        }
        if (flag) {
          lose[i][j][0] = 1, win[i][j][1] = 1;
        }
      }
      if (abs(stri - strj) == abs(stolbj - stolbi)) {
        int q = (stri - strj >= 0) ? -1 : 1;
        int w = (stolbi - stolbj >= 0) ? -1 : 1;
        int iter = 0;
        bool flag = true;
        while (true) {
          int now_str = stri + q * iter;
          int now_stob = stolbi + w * iter;
          if (field[now_str][now_stob] == '1') {
            flag = false;
            break;
          }
          if (now_str == strj) {
            break;
          }
          ++iter;
        }
        if (flag) {
          lose[i][j][0] = 1, win[i][j][1] = 1;
        }
      }
    }
  }
}

void doWins(const vector<string> &field) {
  for (int i = 0; i < sz; ++i) {
    if (field[sz - 1][i] == '1') {
      continue;
    }
    for (int j = 0; j < sz * sz; ++j) {
      if (field[j / sz][j % sz] == '1') {
        continue;
      }
      if (!lose[(sz - 1) * sz + i][j][0]) {
        win[sz * (sz - 1) + i][j][0] = 1;
      }
    }
  }
}

void gg(const vector<string> &field) {
  for (int i = 0; i < sz * sz; ++i) {
    int stri = i / sz;
    int stolbi = i % sz;
    if (field[stri][stolbi] == '1') {
      continue;
    }
    for (int j = 0; j < sz * sz; ++j) {
      int strj = j / sz;
      int stolbj = j % sz;
      if (field[strj][stolbj] == '1') {
        continue;
      }
      if (stri != 0 && field[stri - 1][stolbi] != '1' && !win[i - sz][j][0] &&
          !lose[i - sz][j][0]) {
        arr[i][j][1].emplace_back(i - sz, j, 0);
        ++degree[i - sz][j][0];
      }
      if (stolbi != 0 && field[stri][stolbi - 1] != '1' && !win[i - 1][j][0] &&
          !lose[i - 1][j][0]) {
        arr[i][j][1].emplace_back(i - 1, j, 0);
        ++degree[i - 1][j][0];
      }
      if (stri != sz - 1 && field[stri + 1][stolbi] != '1' &&
          !win[i + sz][j][0] && !lose[i + sz][j][0]) {
        arr[i][j][1].emplace_back(i + sz, j, 0);
        ++degree[i + sz][j][0];
      }
      if (stolbi != sz - 1 && field[stri][stolbi + 1] != '1' &&
          !win[i + 1][j][0] && !lose[i + 1][j][0]) {
        arr[i][j][1].emplace_back(i + 1, j, 0);
        ++degree[i + 1][j][0];
      }
      if (stri != 0 && stolbi != 0 && field[stri - 1][stolbi - 1] != '1' &&
          !win[i - sz - 1][j][0] && !lose[i - sz - 1][j][0]) {
        arr[i][j][1].emplace_back(i - sz - 1, j, 0);
        ++degree[i - sz - 1][j][0];
      }
      if (stri != 0 && stolbi != sz - 1 && field[stri - 1][stolbi + 1] != '1' &&
          !win[i - sz + 1][j][0] && !lose[i - sz + 1][j][0]) {
        arr[i][j][1].emplace_back(i - sz + 1, j, 0);
        ++degree[i - sz + 1][j][0];
      }
      if (stri != sz - 1 && stolbi != 0 && field[stri + 1][stolbi - 1] != '1' &&
          !win[i + sz - 1][j][0] && !lose[i + sz - 1][j][0]) {
        arr[i][j][1].emplace_back(i + sz - 1, j, 0);
        ++degree[i + sz - 1][j][0];
      }
      if (stri != sz - 1 && stolbi != sz - 1 &&
          field[stri + 1][stolbi + 1] != '1' && !win[i + sz + 1][j][0] &&
          !lose[i + sz + 1][j][0]) {
        arr[i][j][1].emplace_back(i + sz + 1, j, 0);
        ++degree[i + sz + 1][j][0];
      }
      if (strj != 0 && field[strj - 1][stolbj] != '1' && !win[i][j - sz][1] &&
          !lose[i][j - sz][1]) {
        arr[i][j][0].emplace_back(i, j - sz, 1);
        ++degree[i][j - sz][1];
      }
      if (stolbj != 0 && field[strj][stolbj - 1] != '1' && !win[i][j - 1][1] &&
          !lose[i][j - 1][1]) {
        arr[i][j][0].emplace_back(i, j - 1, 1);
        ++degree[i][j - 1][1];
      }
      if (strj != sz - 1 && field[strj + 1][stolbj] != '1' &&
          !win[i][j + sz][1] && !lose[i][j + sz][1]) {
        arr[i][j][0].emplace_back(i, j + sz, 1);
        ++degree[i][j + sz][1];
      }
      if (stolbj != sz - 1 && field[strj][stolbj + 1] != '1' &&
          !win[i][j + 1][1] && !lose[i][j + 1][1]) {
        arr[i][j][0].emplace_back(i, j + 1, 1);
        ++degree[i][j + 1][1];
      }
      if (strj != 0 && stolbj != 0 && field[strj - 1][stolbj - 1] != '1' &&
          !win[i][j - sz - 1][1] && !lose[i][j - sz - 1][1]) {
        arr[i][j][0].emplace_back(i, j - sz - 1, 1);
        ++degree[i][j - sz - 1][1];
      }
      if (strj != 0 && stolbj != sz - 1 && field[strj - 1][stolbj + 1] != '1' &&
          !win[i][j - sz + 1][1] && !lose[i][j - sz + 1][1]) {
        arr[i][j][0].emplace_back(i, j - sz + 1, 1);
        ++degree[i][j - sz + 1][1];
      }
      if (strj != sz - 1 && stolbj != 0 && field[strj + 1][stolbj - 1] != '1' &&
          !win[i][j + sz - 1][1] && !lose[i][j + sz - 1][1]) {
        arr[i][j][0].emplace_back(i, j + sz - 1, 1);
        ++degree[i][j + sz - 1][1];
      }
      if (strj != sz - 1 && stolbj != sz - 1 &&
          field[strj + 1][stolbj + 1] != '1' && !win[i][j + sz + 1][1] &&
          !lose[i][j + sz + 1][1]) {
        arr[i][j][0].emplace_back(i, j + sz + 1, 1);
        ++degree[i][j + sz + 1][1];
      }
    }
  }
}

int used[sz * sz][sz * sz][2];

void dfs(int run, int term, int who) {
  used[run][term][who] = 1;
  for (state st : arr[run][term][who]) {
    if (!used[st.runner][st.terminator][st.step]) {
      /*if (win[st.runner][st.terminator][st.step] ||
          lose[st.runner][st.terminator][st.step]) {
        continue;
      }*/
      if (lose[run][term][who]) {
        win[st.runner][st.terminator][st.step] = 1;
      } else if (--degree[st.runner][st.terminator][st.step] == 0) {
        lose[st.runner][st.terminator][st.step] = 1;
      } else {
        continue;
      }
      dfs(st.runner, st.terminator, st.step);
    }
  }
}

int main() {
  int runner, term;
  vector<string> field(sz);
  for (int i = 0; i < sz; ++i) {
    cin >> field[i];
  }
  for (int i = 0; i < sz; ++i) {
    for (int j = 0; j < sz; ++j) {
      if (field[i][j] == '2') {
        runner = sz * i + j;
      } else if (field[i][j] == '3') {
        term = sz * i + j;
      }
    }
  }
  constructGraph(field);
  doWins(field);
  gg(field);
  for (int i = 0; i < sz * sz; ++i) {
    for (int j = 0; j < sz * sz; ++j) {
      for (int q = 0; q < 2; ++q) {
        if ((win[i][j][q] || lose[i][j][q]) && !used[i][j][q]) {
          dfs(i, j, q);
        }
      }
    }
  }
  if (win[runner][term][0] == 1) {
    cout << 1 << '\n';
  } else {
    cout << -1 << '\n';
  }
}