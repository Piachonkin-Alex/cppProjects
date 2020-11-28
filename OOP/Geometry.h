#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <algorithm>


using std::vector;
using std::pair;

struct Point {
    double x;
    double y;

    Point(double x, double y) : x(x), y(y) {}

    Point() : x(0), y(0) {}

    void rotate(const Point &center, double angle) { // поворот точки относительно другой
        vector<double> vec = {x - center.x, y - center.y};  // вектор, соединяющий точки
        vector<double> rotate_vec(2);
        rotate_vec[0] = vec[0] * cos(angle) - vec[1] * sin(angle);  // вектор, получившийся при повороте
        rotate_vec[1] = vec[0] * sin(angle) + vec[1] * cos(angle);
        x = center.x + rotate_vec[0];
        y = center.y + rotate_vec[1];
    }

    void reflex(const Point &center) {  // симметрия отн точки
        vector<double> vec = {x - center.x, y - center.y};
        x = center.x - vec[0];
        y = center.y - vec[1];
    }

    void scale(const Point &center, double cofficient) { // гомотетия
        vector<double> vec = {x - center.x, y - center.y};
        x = center.x + vec[0] * cofficient;
        y = center.y + vec[1] * cofficient;
    }
};

bool operator==(const Point &first, const Point &second) {
    return (first.x == second.x && first.y == second.y);
}

bool operator!=(const Point &first, const Point &second) {
    return !(operator==(first, second));
}

Point operator-(const Point &first, const Point &second) {  // фактически это получение вектора, соединяющиего 2 точки
    return Point(first.x - second.x, first.y - second.y);
}

Point central(const Point &first, const Point &second) {  // точка посередине между двумя
    return Point((first.x + second.x) / 2, (first.y + second.y) / 2);
}

Point pointOnSegmentRelation(const Point &first, const Point &second,
                             double cofficient) { // точка на отрезке между двумя, которая делит в опр. отношении
    std::pair<double, double> vec_between = {second.x - first.x, second.y - first.y};
    return Point(first.x + cofficient * vec_between.first, first.y + cofficient * vec_between.second);
}

double Distance(const Point &first, const Point &second) { // расстояние между точками
    return sqrt(pow((first.x - second.x), 2) + pow((first.y - second.y), 2));
}

class CompareClass {
public:
    bool operator()(Point first,
                    Point second) {  // сравнение точек для сортировки (нужно будет для сравнивания векторов точек)
        if ((first.x > second.x) || (first.x == second.x && first.y > second.y)) {
            return true;
        } else {
            return false;
        }
    }
};

class Line {
public:
    double A;
    double B;
    double C;

    Line(const Point &first, const Point &second) { // через 2 точки
        assert(first != second);
        A = second.y - first.y;
        B = first.x - second.x;
        C = first.y * second.x - first.x * second.y;
    }

    Line(double angle_cof, double free_cof) { // угол и свободный
        A = angle_cof;
        B = -1;
        C = free_cof;
    }

    Line(double A, double B, double C) : A(A), B(B), C(C) {};  // для удобства просто коэффициентами (тесты)

    Line(const Point &point, double angle_cof) : Line(angle_cof, point.y - angle_cof * point.x) {} // через точку и угол

    Line() {}
};

bool operator==(const Line &first, const Line &second) {
    if (first.A * second.B - second.A * first.B == 0 && first.B * second.C - second.B * first.C) {
        return true;
    } else {
        return false;
    }
}

bool operator!=(const Line &first, const Line &second) {
    return !(operator==(first, second));
}

std::pair<double, double>
distVector(const Point &point, const Line &axis) { // вектор, который является расстояние от точки до прямой.
    std::pair<double, double> point_on_line;  // вычисление проходит полностью через формулу проекции.
    if (axis.B == 0) {
        point_on_line.second = 0;
        point_on_line.first = (-1) * axis.C / axis.A;
    } else { // находим точку на прямой
        point_on_line.first = 0;
        point_on_line.second = (-1) * axis.C / axis.B;
    }
    std::pair<double, double> connect_vec = {point_on_line.first - point.x,
                                             point_on_line.second - point.y}; // вектор, соед точку на прямой и исходную
    double constant =
            ((-1) * axis.B * connect_vec.first + axis.A * connect_vec.second) /
            (pow(axis.A, 2) + pow(axis.B, 2)); // коэфф в формуле проекции
    std::pair<double, double> projection = {constant * (-1) * axis.B, constant * axis.A}; // проекция
    std::pair<double, double> dist_vec = {connect_vec.first - projection.first,
                                          connect_vec.second - projection.second}; // искомы вектор
    return dist_vec;
}

void reflexion(Point &point, const Line &axis) { // симметрия отн точки
    std::pair<double, double> dist_vec = distVector(point, axis);
    point.x += 2 * dist_vec.first;
    point.y += 2 * dist_vec.second;
};

Point projection(const Point &point, const Line &line) { // проекция точки на прямую
    std::pair<double, double> dist_vec = distVector(point, line);
    return Point(point.x + dist_vec.first, point.y + dist_vec.second);
}

Point intersection(const Line &first, const Line &second) { // пересечение прямых через правило Крамера
    double main_det = first.A * second.B - second.A * first.B;
    assert(main_det != 0);
    double first_det = (-1) * (first.C * second.B - second.C * first.B);
    double second_det = (-1) * (first.A * second.C - second.A * first.C);
    return Point(first_det / main_det, second_det / main_det);
}


class Shape { // абстрактный класс (почти) фигура
public:
    virtual bool containsPoint(const Point &point) const = 0;

    virtual bool operator==(const Shape &another) const {
        return 0;
    };

    virtual long double area() const = 0;

    virtual double perimeter() const = 0;

    virtual bool isCongruentTo(const Shape &another) const {
        return 0;
    };

    virtual bool isSimilarTo(const Shape &another) const {
        return 0;
    };

    virtual void rotate(const Point &center, double angle) = 0;

    virtual void reflex(const Point &center) = 0;

    virtual void reflex(const Line &axis) = 0;

    virtual void scale(const Point &center, double coefficient) = 0;

    virtual ~Shape() {}
};

class Polygon : public Shape { // многоугольник
protected:
    vector<Point> vertices;
    double Area; // пришлось хранить площадь всегда и находить ее при инициализации. иначе точность все била

    template<typename ... Args>
    void addPoints(const Point &first, Args ... args) { // добавление точек в вершины многоугольника
        vertices.push_back(first);
        addPoints(args ...);
    }

    void addPoints() { // перегразка на 0 членов
        return;
    }

public:
    int verticesCount() { // подсчет кол-ва вершин
        return vertices.size();
    }

    vector<Point> getVertices() { // все вершины
        return vertices;
    }

    template<typename ... Args>
    Polygon(const Point &first, Args ... args) { // инициализация через запятую
        addPoints(first, args ...);
        Area = _Area();
    }

    Polygon(std::vector<Point> &points) { // через массив
        vertices = points;
    }

    Polygon() {};

    bool isConvex() const { // выпуклость
        bool res = true;
        double product = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            int minim = (i - 1 + vertices.size()) % vertices.size();
            int middle = i % vertices.size();
            int bigger = (i + 1) % vertices.size();
            Point vector_ab = vertices[middle] - vertices[minim]; // находим два вектора сторон при угле
            Point vector_bc = vertices[bigger] - vertices[middle];
            double this_product =
                    vector_ab.x * vector_bc.y - vector_ab.y * vector_bc.x; // проверяем угол основываясь на предыдущем
            if (this_product * product >= 0) {
                product = this_product;
            } else {
                res = false;
                break;
            }
        }
        return res;
    }

    virtual bool containsPoint(const Point &point) const { // содержит ли точку. здесь через горизонтальный луч
        bool res = false;
        int prev = vertices.size() -
                   1; // вершина в многоугольнике <-> горизонтальный луч влево пересекает неч. число сторон
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (((vertices[i].y < point.y && vertices[prev].y >= point.y) ||
                 (vertices[prev].y < point.y && vertices[i].y >= point.y))) { // это собственно проверка этого)
                double prod = vertices[i].x + (point.y - vertices[i].y) / (vertices[prev].y - vertices[i].y) *
                                              (vertices[prev].x - vertices[i].x);
                if (prod < point.x) {
                    res = !res;
                } else if (prod == point.x) { // на границе -- вернули сразу
                    res = true;
                    break;
                }
            }
            prev = i;
        }
        for (size_t i = 0; i < vertices.size(); ++i) { // отдельно проверяем вершины
            if (point == vertices[i]) {
                res = true;
                break;
            }
        }
        return res;
    }

    virtual bool operator==(const Polygon &another) const {  // равны
        vector<Point> first_vert = vertices;
        vector<Point> another_vert = another.vertices;
        std::sort(first_vert.begin(), first_vert.end(), CompareClass());
        std::sort(another_vert.begin(), another_vert.end(), CompareClass());
        return (first_vert == another_vert &&
                isCongruentTo(another)); // они равны <-> равны в геом. смысле и все координаты точек равны
    }

    virtual bool operator!=(const Polygon &another) {
        return !(operator==(another));
    }

    virtual double perimeter() const { // периметр -- очевидно
        double res = 0;
        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            res += Distance(vertices[i], vertices[i + 1]);
        }
        res += Distance(vertices[vertices.size() - 1], vertices[0]);
        return res;
    }

    long double _Area() const { // площадь -- через суммы направленных трапеций. Это для подсчета при инициализации
        long double res = 0;
        res += (vertices.back().x - vertices[0].x) * (vertices.back().y + vertices[0].y);
        for (size_t i = 1; i < vertices.size(); ++i) {
            res += (vertices[i - 1].x - vertices[i].x) * (vertices[i - 1].y + vertices[i].y);
        }
        return abs(res) / 2;
    }

    virtual long double area() const { // вывод
        return Area;
    }

    virtual bool isSimilarTo(const Polygon &another) const { // подобие
        if (vertices.size() != another.vertices.size()) { // не равно число вершин -- нет подобия
            return false;
        }
        // в данном блоке проверялось, что стороны подобны по длине в порядке возрастания
        std::vector<double> first_dist, another_dist;
        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t prev = (vertices.size() + i - 1) % vertices.size();
            first_dist.push_back(Distance(vertices[i], vertices[prev]));
            another_dist.push_back(Distance(another.vertices[i], another.vertices[prev]));
        }
        std::sort(first_dist.begin(), first_dist.end());
        std::sort(another_dist.begin(), another_dist.end());
        double cof = -1;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (cof == -1 || abs(first_dist[i] / another_dist[i] - cof) < 0.0001) {
                continue;
            } else {
                return false;
            }
        }
        // в данном блоке мы ищем такую вершину во втором многоугольнике,
        // что расстояния от нее до соответствующих вершин подобны расстояниям от вершины 0 в исходном многоугольнике до соответствующих вершин

        bool res = false;
        for (size_t i = 0; i < vertices.size(); ++i) {
            bool subres = true;
            double coff = -1;
            for (size_t j = 1; j < vertices.size(); ++j) {
                size_t now_point = (i + j) % vertices.size();
                double dist_first = Distance(vertices[0], vertices[j]);
                double dist_second = Distance(another.vertices[i], another.vertices[now_point]);
                if (coff == -1 || abs(dist_first / dist_second - coff) < 0.001) {
                    coff = dist_first / dist_second;
                } else {
                    subres = false;
                    break;
                }
            }
            if (subres) {
                res = true;
                break;
            }
        }

        if (res) { // нашли -- подобны
            return true;
        }
        //Повторение кода, можно было бы спрятать в inline функцию, и читать было бы приятнее, так как функция слишком длинная
        vector<Point> reverse_vert = vertices; // надо еще проверить и обратное направление
        std::reverse(reverse_vert.begin(), reverse_vert.end());
        for (size_t i = 0; i < vertices.size(); ++i) {
            bool subres = true;
            double coff = -1;
            for (size_t j = 1; j < vertices.size(); ++j) {
                size_t now_point = (i + j) % vertices.size();
                double dist_first = Distance(reverse_vert[0], reverse_vert[j]);
                double dist_second = Distance(another.vertices[i], another.vertices[now_point]);
                if (coff == -1 || abs(dist_first / dist_second - coff) < 0.001) {
                    coff = dist_first / dist_second;
                } else {
                    subres = false;
                    break;
                }
            }
            if (subres) {
                res = true;
                break;
            }
        }

        return res;
    }

    bool isCongruentTo(const Polygon &another) const { // равны в геом. смысле
        if (!isSimilarTo(another)) { // должно быть подобие
            return false;
        }
        double max_side_first = Distance(vertices.back(), vertices[0]);
        double max_side_second = Distance(another.vertices.back(), another.vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            max_side_first = std::max(max_side_first, Distance(vertices[i - 1], vertices[i]));
            max_side_second = std::max(max_side_second, Distance(another.vertices[i - 1], another.vertices[i]));
        }
        return (abs(max_side_first - max_side_second) < 0.0001 ? 1 : 0); // ну и большие стороны равны, тогде коэф. подобия -- 1
    }

    virtual void reflex(const Point &center) { // симметрия относительно точки -- делаем симметрию на все вершины
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].reflex(center);
        }
    }

    virtual void reflex(const Line &axis) { // симметрия относительно прямой -- тоже для всех вершин
        for (size_t i = 0; i < vertices.size(); ++i) {
            reflexion(vertices[i], axis);
        }
    }

    virtual void rotate(const Point &center, double angle) { // поворот -- тоже
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].rotate(center, angle);
        }
    }

    virtual void scale(const Point &center, double coefficient) { // гомотетия. не забываем площадь умножеть на коэфф. в квадрать
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].scale(center, coefficient);
        }
        Area *= pow(coefficient, 2);
    }
};


class Rectangle : public Polygon {
protected:
    double length;
    double width;
public:
    Rectangle(const Point &first, const Point &third, double cofficient) {
        if (cofficient < 1) {  // делаем коэффициент точно больше 1
            cofficient = 1 / cofficient;
        }
        double sin_a = cofficient / sqrt(pow(cofficient, 2) + 1);  // синус и косинус угла поворота до маленькой стороны слева
        double cos_a = 1 / sqrt(pow(cofficient, 2) + 1);
        Point copy_third = third; // copy_third -- копия третьей точки, которую мы повернем относительно первой точки так, чтобы она была на прямой искомой стороны
        vector<double> vec = {copy_third.x - first.x, copy_third.y - first.y};
        vector<double> rotate_vec(2);
        rotate_vec[0] = vec[0] * cos_a - vec[1] * sin_a;
        rotate_vec[1] = vec[0] * sin_a + vec[1] * cos_a;
        copy_third.x = first.x + rotate_vec[0];
        copy_third.y = first.y + rotate_vec[1]; // сообственно получили точку на искомой стороге
        Line side = Line(first, copy_third); // прямая искомой стороны
        std::pair<double, double> dist_vector = distVector(third, side); // вектор расстояния от третьей точки до второй стороны
        Point second = Point(third.x + dist_vector.first, third.y + dist_vector.second); // прибавили этот вектор к 3 точке -- нашли вторую точку на искомой стороне
        Point center = Point((first.x + third.x) / 2, (first.y + third.y) / 2); // центр диоганали
        Point fourth = second;
        fourth.reflex(center); // 4 точка -- симметрия относительно центра диоганали второй точки
        Polygon::addPoints(first, second, third, fourth); // добавили точки в многоугольник
        length = Distance(second, third); // посчитали длину и ширину
        width = Distance(first, second);
    }

    std::pair<Line, Line> dioganals() { // диоганали
        Line first = Line(Polygon::vertices[0], Polygon::vertices[2]);
        Line second = Line(Polygon::vertices[1], Polygon::vertices[3]);
        return {first, second};
    }

    Point center() { // центра диоганалей
        return Point((Polygon::vertices[0].x + Polygon::vertices[2].x) / 2,
                     (Polygon::vertices[0].y + Polygon::vertices[2].y) / 2);
    }

    virtual long double area() const { // площадь
        return length * width;
    }

    virtual double perimeter() const { // периметр
        return 2 * (length + width);
    }

    virtual bool isSimilarTo(const Rectangle &another) const { // ну подобие найдем быстрее чем в многоугольнике
        return (length * another.width == another.length * width);
    }

    virtual bool isCongruentTo(const Rectangle &another) const { // подобие тоже
        return (length == another.length && width == another.width);
    }

    virtual void scale(const Point &center, double cofficient) { // гомотетия
        Polygon::scale(center, cofficient);
        width *= abs(cofficient);
        length *= abs(cofficient);
    }
};


class Ellipse : public Shape {
protected:
    Point first_focus;
    Point second_focus;
    Point centr;
    double a;
    double b;
public:

    Ellipse() {};

    Ellipse(const Point &fir, const Point &sec, double dst) { // инициализация. тут просто пишем формулы для a и b
        first_focus = fir, second_focus = sec;
        centr = Point((first_focus.x + second_focus.x) / 2, (first_focus.y + second_focus.y) / 2);
        a = dst / 2;
        b = sqrt(a * a - pow(Distance(fir, sec) / 2, 2));
    }

    Point center() {
        return centr;
    }

    double eccentricity() {
        return sqrt(1 - pow(b / a, 2));
    }

    std::pair<Line, Line> directrices() { // высчет дирректрис
        assert(a != b); // окружность -- не определено
        Line big_axis = Line(first_focus, second_focus); // большая полуось
        double change_x = (big_axis.B * a) / (eccentricity() * sqrt(pow(big_axis.A, 2) + pow(big_axis.B, 2)));
        double change_y = (big_axis.A * a) / (eccentricity() * sqrt(pow(big_axis.A, 2) + pow(big_axis.B, 2)));
        Point dir_first_point = Point(centr.x - change_x, centr.y + change_y); // подсчет точек принадлежащих первой и второй дирректрисе
        Point dir_second_point = Point(centr.x + change_x, centr.y - change_y);
        if (big_axis.A == 0) {
            Line dir1 = Line(dir_first_point, Point(dir_first_point.x, dir_first_point.y + 1));
            Line dir2 = Line(dir_second_point, Point(dir_second_point.x, dir_second_point.y + 1));
            return {dir1, dir2};
        } else {  // здесь мы уже конструируем дирректрисы используя полученные точки у уравнение прямой большой полуоси
            Line dir1 = Line(dir_first_point, big_axis.B / big_axis.A);
            Line dir2 = Line(dir_second_point, big_axis.B / big_axis.A);
            return {dir1, dir2};
        }
    }

    std::pair<Point, Point> focuses() {
        assert(a != b);
        return {first_focus, second_focus};
    }

    virtual long double area() const {
        return M_PI * a * b;
    }

    virtual double perimeter() const { // вторая формула Рамануджана
        return M_PI * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
    }

    virtual bool containsPoint(const Point &point) const { // расстояние до фокусов меньше или равно 2*a
        if (Distance(point, first_focus) + Distance(point, second_focus) <= 2 * a) {
            return true;
        } else {
            return false;
        }
    }

    virtual bool operator==(const Ellipse &another) const { // равентво эллипсов. Важно проверить случай, когда эллипс это окружность. Фокусы тогда могут не совпадать
        if (a == another.a && b == another.b) {
            if (a == b && centr == another.centr) {
                return true;
            }
            if ((first_focus == another.first_focus && second_focus == another.second_focus) ||
                (first_focus == another.second_focus && second_focus == another.first_focus)) {
                return true;
            }
        }
        return false;
    }

    virtual bool isSimilarTo(const Ellipse &another) const { // подобие
        if (a * another.b == b * another.a) {
            return true;
        }
        return false;
    }

    virtual bool isCongruentTo(const Ellipse &another) const { // равенство в геом. плане
        if (a == another.a && b == another.b) {
            return true;
        }
        return false;
    }

    virtual void rotate(const Point &center, double angle) { //поворот
        first_focus.rotate(center, angle);
        second_focus.rotate(center, angle);
        centr.rotate(center, angle);
    }

    virtual void reflex(const Point &center) { // симметрия отн. точки
        first_focus.reflex(center);
        second_focus.reflex(center);
        centr.reflex(center);
    }

    virtual void reflex(const Line &axis) { // симметрия отн. прямой
        reflexion(first_focus, axis);
        reflexion(second_focus, axis);
        reflexion(centr, axis);
    }

    virtual void scale(const Point &center, double cofficient) { // гомотетия
        first_focus.scale(center, cofficient);
        second_focus.scale(center, cofficient);
        centr.scale(center, cofficient);
        a *= abs(cofficient), b *= abs(cofficient);
    }
};

class Circle : public Ellipse {
public:
    Circle(const Point &center, double radius) { // окружность. радиус не храним. это Ellipse::a
        Ellipse::a = radius;
        Ellipse::b = radius;
        Ellipse::centr = center;
    }

    double radius() const {
        return Ellipse::a;
    }

    Point center() const {
        return centr;
    }

    virtual long double area() const {
        return M_PI * pow(Ellipse::a, 2);
    }

    virtual double perimeter() const {
        return 2 * M_PI * Ellipse::a;
    }

    virtual bool operator==(const Circle &another) const {
        return (centr == another.centr && radius() == another.radius());
    }

    virtual bool isSimilarTo(const Circle &another) const {
        return true;
    }

    virtual bool isCongruentTo(const Circle &another) const {
        return radius() == another.radius();
    }

    virtual void rotate(const Point &center, double angle) {
        Ellipse::centr.rotate(center, angle);
        centr.rotate(center, angle);
    }

    virtual void reflex(const Point &center) {
        Ellipse::centr.reflex(center);
        centr.reflex(center);
    }

    virtual void reflex(const Line &axis) {
        reflexion(Ellipse::centr, axis);
        reflexion(centr, axis);
    }

    virtual void scale(const Point &center, double cofficient) {
        Ellipse::scale(center, cofficient);
    }
};

class Square : public Rectangle { // квадрат
public:
    Square(const Point &first, const Point &third) : Rectangle(first, third, 1) {}; // прямоугольник с коэфф. 1

    Circle circumscribedCircle() { // описанная окружность
        return Circle(Rectangle::center(), Rectangle::length / sqrt(2));
    }

    Circle insrcibedCircle() { // вписанная
        return Circle(Rectangle::center(), Rectangle::length / 2);
    }

};

class Triangle : public Polygon {
public:
    Triangle(const Point &first, const Point &second, const Point &third) : Polygon(first, second, third) {};

    Point centroid() { // точка пересечения медиан. находим центра одной стороны и берем 2/3 отрезка от вершины с помощью pointOnSegmentRelation
        Point side_center = central(Polygon::vertices[0], Polygon::vertices[1]);
        double cofficient = double(2) / double(3);
        return pointOnSegmentRelation(Polygon::vertices[2], side_center, cofficient);
    }

    Point orthocenter() { // ортоцентр. нашли две прямые высот и их пересечение
        Line first_side = Line(Polygon::vertices[1], Polygon::vertices[2]);
        Line second_side = Line(Polygon::vertices[0], Polygon::vertices[2]);
        Point projection_first = projection(Polygon::vertices[0], first_side);
        Point projection_second = projection(Polygon::vertices[1], second_side); // проекции точек на стороны
        Line first_height = Line(Polygon::vertices[0], projection_first);
        Line second_height = Line(Polygon::vertices[1], projection_second);
        return intersection(first_height, second_height);
    }

    Circle circumscribedCircle() { // описанная окружность. точка пересечения серединных пенпендикуляров
        Line first_side = Line(Polygon::vertices[0], Polygon::vertices[1]); // прямые сторон
        Line second_side = Line(Polygon::vertices[1], Polygon::vertices[2]);
        Point center_first = central(Polygon::vertices[0], Polygon::vertices[1]); // середины сторон
        Point center_second = central(Polygon::vertices[1], Polygon::vertices[2]);
        Line first_penpedicular, second_penpendicular;
        if (first_side.A != 0) {
            first_penpedicular = Line(center_first, first_side.B / first_side.A);
        } else {
            first_penpedicular = Line(center_first, Point(center_first.x, center_first.y + 1));
        }
        if (second_side.A != 0) { // просчет пенпендикуляров
            second_penpendicular = Line(center_second, second_side.B / second_side.A);
        } else {
            second_penpendicular = Line(center_second, Point(center_second.x, center_second.y + 1));
        }
        Point center = intersection(first_penpedicular, second_penpendicular); // пересечение 2 серединных пенпендикуляров
        double radius = Distance(center, Polygon::vertices[0]); // подсчет радиуса
        return Circle(center, radius);
    }

    Circle inscribedCircle() { // вписанная окружность. точка пересечения биссектрисс.
        double dist_first_side = Distance(Polygon::vertices[0], Polygon::vertices[1]);
        double dist_second_side = Distance(Polygon::vertices[1], Polygon::vertices[2]); // просчет длин сторон
        double dist_third_side = Distance(Polygon::vertices[2], Polygon::vertices[0]);
        Point first_bisector_point = pointOnSegmentRelation(Polygon::vertices[1], Polygon::vertices[2],
                                                            dist_first_side / (dist_first_side + dist_third_side));
        Point second_bisector_point = pointOnSegmentRelation(Polygon::vertices[0], Polygon::vertices[2], // просчет точек на сторонах, принадлежащих биссектрисам
                                                             dist_first_side / (dist_first_side + dist_second_side));
        Line bisector_1 = Line(Polygon::vertices[0], first_bisector_point);
        Line bisector_2 = Line(Polygon::vertices[1], second_bisector_point); // конструкция биссектрисс
        Point center = intersection(bisector_1, bisector_2);
        double radius = 2 * area() / (dist_first_side + dist_second_side + dist_third_side); // просцет центра и радиуса
        return Circle(center, radius);
    }

    Line EulerLine() { // прямая Эйлера. на ней лежит ортоцентр и центр масс
        return Line(orthocenter(), centroid());
    }

    Circle ninePointsCircle() { // окружность Эйлера. Радиус в два раза меньше радиуса описанной окружности. центр --
        // центра отрезка между центром описанной окружности и ортоцентром.
        Circle circumscribed = circumscribedCircle();
        Point center_circumscribed = circumscribed.center();
        Point orthocentr = orthocenter();
        Point center = central(center_circumscribed, orthocentr);
        return Circle(center, circumscribed.radius() / 2);
    }
};

