
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/glut.h>
#include <utility>
#include <vector>
#include <functional>

// dimensiunea ferestrei in pixeli
#define dim 300

#define PI (4 * atan(1.0))

using namespace std;

unsigned char prevKey;

double function21(double x) {
    if (x == 0) return 1;
    else {
        return fabs(x - round(x)) / x;
    }
}

pair<double, double> function22(double t, double a, double b) {
    pair<double, double> result;
    result.first = (b + a * cos(t)) * cos(t);
    result.second = (b + a * cos(t)) * sin(t);

    return result;
}

// concoida lui Nicomede (concoida dreptei)
// $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. sau
// $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. unde
// $t \in (-\pi / 2, \pi / 2)$
void Display1() {
    double xmax, ymax, xmin, ymin;
    double a = 1, b = 2;
    double ratia = 0.05;
    double t;

    // calculul valorilor maxime/minime ptr. x si y
    // aceste valori vor fi folosite ulterior la scalare
    xmax = a - b - 1;
    xmin = a + b + 1;
    ymax = ymin = 0;
    for (t = -PI / 2 + ratia; t < PI / 2; t += ratia) {
        double x1, y1, x2, y2;
        x1 = a + b * cos(t);
        xmax = (xmax < x1) ? x1 : xmax;
        xmin = (xmin > x1) ? x1 : xmin;

        x2 = a - b * cos(t);
        xmax = (xmax < x2) ? x2 : xmax;
        xmin = (xmin > x2) ? x2 : xmin;

        y1 = a * tan(t) + b * sin(t);
        ymax = (ymax < y1) ? y1 : ymax;
        ymin = (ymin > y1) ? y1 : ymin;

        y2 = a * tan(t) - b * sin(t);
        ymax = (ymax < y2) ? y2 : ymax;
        ymin = (ymin > y2) ? y2 : ymin;
    }

    xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

    // afisarea punctelor propriu-zise precedata de scalare
    glColor3f(1, 0.1, 0.1); // rosu
    glBegin(GL_LINE_STRIP);
    for (t = -PI / 2 + ratia; t < PI / 2; t += ratia) {
        double x, y;
        x = (a + b * cos(t)) / xmax;
        y = (a * tan(t) + b * sin(t)) / ymax;

        glVertex2f(x, y);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (t = -PI / 2 + ratia; t < PI / 2; t += ratia) {
        double x1, y1, x2, y2;
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2f(x2, y2);
    }
    glEnd();
}

// graficul functiei
// $f(x) = \bar sin(x) \bar \cdot e^{-sin(x)}, x \in \langle 0, 8 \cdot \pi \rangle$,
void Display2() {
    double pi = 4 * atan(1.0);
    double xmax = 8 * pi;
    double ymax = exp(1.1);
    double ratia = 0.05;

    // afisarea punctelor propriu-zise precedata de scalare
    glColor3f(1, 0.1, 0.1); // rosu
    glBegin(GL_LINE_STRIP);
    for (double x = 0; x < xmax; x += ratia) {
        double x1, y1;
        x1 = x / xmax;
        y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;

        glVertex2d(x1, y1);
    }
    glEnd();
}


// 2.1 Alex
void Display3() {
    //""""""SCALARE"""""""""
    double ymax = -10e9;
    double ymin = 10e9;
    double xmax = 30;
    double ratia = 0.05;

    for (double x = 0; x < 100; x += ratia) {
        double result = function21(x);
        if (result > ymax) ymax = result;
        if (result < ymin) ymin = result;
    }

    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);

    for (double x = 0; x < 100; x += ratia) {
        double y;
        y = function21(x) / ymax;

        glVertex2d(x / xmax, y);
    }
    glEnd();
}


// 2.2.1 Alex
void Display4() {
    double ymax = -10e9;
    double ymin = 10e9;
    double xmax = -10e9;
    double xmin = 10e9;
    double a = 0.3;
    double b = 0.2;
    double ratia = 0.05;

    //scalare?

    for (double t = -PI; t < PI; t += ratia) {
        pair<double, double> result = function22(t, a, b);
        if (result.first < xmin) xmin = result.first;
        if (result.first > xmax) xmax = result.first;
        if (result.second < ymin) ymin = result.second;
        if (result.second > ymax) ymax = result.second;
    }

    xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);


    glColor3f(1.0, 0, 0);
    glBegin(GL_LINE_LOOP);

    for (double t = -PI; t < PI; t += ratia) {
        pair<double, double> result = function22(t, a, b);
        glVertex2d(result.first / xmax, result.second / ymax);
    }
    glEnd();

}

// 2.2.2 Alex
void Display5() {

}

double max(std::vector<double> &d) {
    int max_idx = 0;
    for (int idx = 0; idx < d.size(); idx++) {
        if (d[idx] > d[max_idx]) {
            max_idx = idx;
        }
    }
    return d[max_idx];
}

double min(std::vector<double> &d) {
    int min_idx = 0;
    for (int idx = 0; idx < d.size(); idx++) {
        if (d[idx] < d[min_idx]) {
            min_idx = idx;
        }
    }
    return d[min_idx];
}

void normalize(std::vector<double> &x, double scale) {
    double x_min = min(x);
    double x_max = max(x);
    double norm_factor = (x_max - x_min) / 2.0;
    double offset = (x_max + x_min) / 2.0;

    for (double &idx : x) {
        idx = scale * (idx - offset) / norm_factor;
    }
}

void DisplayFunction(
        vector<std::function<pair<double, double>(double)> > &funcs,
        double domain_min,
        double domain_max,
        double ratio,
        GLenum line_param,
        double scale_x,
        double scale_y) {
    vector<double> X, Y;

    for (std::function<pair<double, double>(double)> func : funcs) {
        double domain_x = domain_min;
        while (domain_x < domain_max) {
            double x, y;
            std::tie(x, y) = func(domain_x);
            X.push_back(x);
            Y.push_back(y);
            domain_x += ratio;
        }
    }

    normalize(X, scale_x);
    normalize(Y, scale_y);

    glColor3f(1.0, 0, 0);
    glBegin(line_param);
    for (int idx = 0; idx < X.size(); idx++) {
        glVertex2d(X[idx], Y[idx]);
    }
    glEnd();
}


// 2.2.3 Vali
void Display6() {
    vector<function<pair<double, double>(double)>> v;
    v.emplace_back([](double t) {
        double a = 0.1, b = 0.2;
        double x = a * t - b * sin(t);
        double y = a - b * cos(t);

        return std::make_pair(x, y);
    });

    DisplayFunction(v, -4*M_PI, 4*M_PI, 0.05, GL_LINE_STRIP, 1.0, 0.5);
}

// 2.2.4 Vali
void Display7() {
    vector<function<pair<double, double>(double)>> v;
    v.emplace_back([](double t) {
        double r = 0.3, R = 0.1;
        double x = (R + r) * cos(r / R * t) - r * cos(t + r / R * t);
        double y = (R + r) * sin(r / R * t) - r * sin(t + r / R * t);

        return std::make_pair(x, y);
    });
    DisplayFunction(v, 0, 2 * M_PI, 0.05, GL_LINE_LOOP, 1.0, 1.0);
}

// 2.2.5 Vali
void Display8() {
    vector<function<pair<double, double>(double)>> v;
    v.emplace_back([](double t) {
        double r = 0.3, R = 0.1;
        double x = (R - r) * cos(r / R * t) - r * cos(t - r / R * t);
        double y = (R - r) * sin(r / R * t) - r * sin(t - r / R * t);

        return std::make_pair(x, y);
    });
    DisplayFunction(v, 0, 2 * M_PI, 0.05, GL_LINE_LOOP, 1.0, 1.0);
}

// 3.1 Alex
void Display9() {
    vector<function<pair<double, double>(double)>> v;
    v.emplace_back([](double t) {
        double a = 0.02;
        double r = a * exp(1+t);
        double x = r * cos(t);
        double y = r * sin(t);

        return std::make_pair(x, y);
    });
    DisplayFunction(v, 0, 2.2, 0.05, GL_LINE_STRIP, 0.8, 0.8);
}

// 3.2 Alex
void Display10() {
    vector<function<pair<double, double>(double)>> v;
    v.emplace_back([](double t) {
        double a = 0.4;
        double r = a * sqrt(2 * cos(2 * t));
        double x = r * cos(t);
        double y = r * sin(t);

        return std::make_pair(x, y);
    });
    v.emplace_back([](double t) {
        double a = 0.4;
        double r = -a * sqrt(2 * cos(2 * t));
        double x = r * cos(t);
        double y = r * sin(t);

        return std::make_pair(x, y);
    });
    DisplayFunction(v, -M_PI_4, M_PI_4, 0.05, GL_LINE_LOOP, 0.8, 0.8);
}

void Init(void) {

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glLineWidth(1);

//   glPointSize(4);

    glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (prevKey) {
        case '1':
            Display1();
            break;
        case '2':
            Display2();
            break;
        case '3':
            Display3();
            break;
        case '4':
            Display4();
            break;
        case '5':
            Display5();
            break;
        case '6':
            Display6();
            break;
        case '7':
            Display7();
            break;
        case '8':
            Display8();
            break;
        case '9':
            Display9();
            break;
        case '0':
            Display10();
            break;
        default:
            break;
    }

    glFlush();
}

void Reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void KeyboardFunc(unsigned char key, int x, int y) {
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y) {
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);

    glutInitWindowSize(dim, dim);

    glutInitWindowPosition(100, 100);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutCreateWindow(argv[0]);

    Init();

    glutReshapeFunc(Reshape);

    glutKeyboardFunc(KeyboardFunc);

    glutMouseFunc(MouseFunc);

    glutDisplayFunc(Display);

    glutMainLoop();

    return 0;
}
