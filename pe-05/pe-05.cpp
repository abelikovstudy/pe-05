#include <iostream>
#include <ostream>
#include <random>
#include <array>
#include <chrono>
#include <functional>

std::random_device rd;
std::mt19937 rng(rd());

double func1(double x, double y)
{
    return (3 * pow(x, 2)) + (x * y) + (2 * pow(y, 2)) - x - (4 * y); // f(0,1) = -2
}

double func2(double x, double y)
{
    return pow(x, 2) + pow(y, 2) + (0.5 * sin(10 * x)) + (0.5 * cos(10 * y));
}

double func3(double x, double y)
{
    return 5 * pow(x, 2) + 5 * pow(y, 2) - 10; //f(0,0) = -10
}

struct Point {
    double x;
    double y;
    double result;
    Point(double _x, double _y, int type)
    {
        x = _x;
        y = _y;
        switch (type)
        {
        case 1:
            result = func1(x, y);
            break;
        case 3:
            result = func3(x, y);
            break;
        }
    }
    Point() {
        x = 0.0;
        y = 0.0;
        result = 0.0;
    }
    Point(double _x, double _y, std::function<double(double, double)> function)
    {
        x = _x;
        y = _y;
        result = function(x, y);
    }
};
struct Result {
    Point res;
    std::chrono::duration<double> time;
    Point dest;
    double fDiff;
    Result(Point _res, std::chrono::duration<double> _time, Point _dest)
    {
        res = _res;
        time = _time;
        dest = _dest;
        fDiff = res.result - dest.result;
    }
};

bool operator <(const Point& p1, const Point& p2) {
    return p1.result < p2.result;
}
bool operator >(const Point& p1, const Point& p2) {
    return p1.result > p2.result;
}
bool operator ==(const Point& p1, const Point& p2) {
    return p1.result == p2.result;
}
std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << "f(" << p.x << "," << p.y << ") = " << p.result;
    return os;
}

double diffPoints(Point p1, Point p2) {
    return p1.x - p2.x + p1.y - p2.y;
}
double diff(Point p1, Point p2) { return p1.result - p2.result; }

int urnd(int s, int e) {
    std::uniform_int_distribution<int> uni(s, e);
    return uni(rng);
}

Result anneal_sim(double startX, double startY, std::function<double(double, double)> f, double temp, double tempDiff, double coordMin, double coordMax, int iters, Point dest) {
    std::chrono::time_point<std::chrono::system_clock> timeStart, timeEnd;
    timeStart = std::chrono::system_clock::now();
    Point start(startX, startY, f);
    double start_energy = start.result;

    Point shift(startX, startY, f);
    double shift_energy = shift.result;

    while (iters) {

        Point attempt(urnd(coordMin, coordMax) * .1, urnd(coordMin, coordMax) * .1, f);
        double attempt_energy = attempt.result;
        double dEnergy = attempt_energy - start_energy;
        if (dEnergy < 0 || exp(-dEnergy / temp) >(rng() / (double)RAND_MAX)) {
            start = attempt;
            start_energy = attempt_energy;
        }
        if (attempt_energy < shift_energy) {
            shift = attempt;
            shift_energy = attempt_energy;
        }
        iters -= 1;
        temp *= tempDiff;
    }

    timeEnd = std::chrono::system_clock::now();
    Result res(shift, timeEnd - timeStart, dest);
    return res;
}

void prettyPrint(Result res) {
    std::cout << res.res
        << " | Diff f(x,y): "
        << res.fDiff << "% | Time: " << res.time.count() << std::endl;
}

int main()
{
    std::srand(std::time(nullptr));
    Point dest1(0, 1, func1);
    Point dest2(-0.1, 0.3, func2);
    Point dest3(0, 0, func3);

    std::cout << std::endl;
    std::cout << "============================= Annealing Simulation================================" << std::endl;
    prettyPrint(anneal_sim(0, 0, func1, 1000, 0.95, -50, 50, 10000, dest1));
    prettyPrint(anneal_sim(0, 0, func2, 1000, 0.95, -50, 50, 100000, dest2));
    prettyPrint(anneal_sim(0, 0, func3, 1000, 0.95, -50, 50, 100000, dest3));

}
