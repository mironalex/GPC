
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/glut.h>

// dimensiunea ferestrei in pixeli
#define dim 300

#define PI (4 * atan(1.0))

unsigned char prevKey;

double function21(double x){
    if (x == 0) return 1;
    else{
        return fabs(x - round(x))/x;
    }
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
    for (t = - PI/2 + ratia; t < PI / 2; t += ratia) {
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
    glColor3f(1,0.1,0.1); // rosu
    glBegin(GL_LINE_STRIP);
    for (t = - PI/2 + ratia; t < PI / 2; t += ratia) {
        double x, y;
        x = (a + b * cos(t)) / xmax;
        y = (a * tan(t) + b * sin(t)) / ymax;

        glVertex2f(x,y);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (t = - PI/2 + ratia; t < PI / 2; t += ratia) {
        double x1, y1, x2, y2;
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2f(x2,y2);
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
    glColor3f(1,0.1,0.1); // rosu
    glBegin(GL_LINE_STRIP);
    for (double x = 0; x < xmax; x += ratia) {
        double x1, y1;
        x1 = x / xmax;
        y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;

        glVertex2d(x1,y1);
    }
    glEnd();
}


// 2.1 Alex
void Display3(){
    //""""""SCALARE"""""""""
    double ymax = -10e9;
    double ymin = 10e9;
    double xmax = 30;
    double ratia = 0.05;

    for(double x = 0; x < 100; x += ratia){
        double result = function21(x);
        if (result > ymax) ymax = result;
        if (result < ymin) ymin = result;
    }

    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);

    for(double x = 0; x < 100; x+= ratia){
        double y;
        y = function21(x) / ymax;

        glVertex2d(x / xmax,y);
    }
    glEnd();
}


// 2.2.1 Alex
void Display4(){

}

// 2.2.2 Alex
void Display5(){

}

// 2.2.3 Vali
void Display6(){

}

// 2.2.4 Vali
void Display7(){

}

// 2.2.5 Vali
void Display8(){

}

// 3.1 Alex
void Display9(){

}

// 3.2 Vali
void Display10(){

}

void Init(void) {

    glClearColor(1.0,1.0,1.0,1.0);

    glLineWidth(1);

//   glPointSize(4);

    glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    switch(prevKey) {
        case '1':
            Display1();
            break;
        case '2':
            Display2();
            break;
        case '3':
            Display3();
        case '4':
            Display4();
        case '5':
            Display5();
        case '6':
            Display6();
        case '7':
            Display7();
        case '8':
            Display8();
        case '9':
            Display9();
        case '0':
            Display10();
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

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitWindowSize(dim, dim);

    glutInitWindowPosition(100, 100);

    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);

    glutCreateWindow (argv[0]);

    Init();

    glutReshapeFunc(Reshape);

    glutKeyboardFunc(KeyboardFunc);

    glutMouseFunc(MouseFunc);

    glutDisplayFunc(Display);

    glutMainLoop();

    return 0;
}