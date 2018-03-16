#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cfloat>

#include <GL/glut.h>
#include <utility>
#include <vector>

// dimensiunea ferestrei in pixeli
#define dim 300
// numarul maxim de iteratii pentru testarea apartenentei la mult.Julia-Fatou
#define NRITER_JF 5000
// modulul maxim pentru testarea apartenentei la mult.Julia-Fatou
#define MODMAX_JF 10000000
// ratii ptr. CJuliaFatou
#define RX_JF 0.01
#define RY_JF 0.01

unsigned char prevKey;
int nivel = 0;


//START P1

class CComplex {
public:
    CComplex() : re(0.0), im(0.0) {}
    CComplex(double re1, double im1) : re(re1 * 1.0), im(im1 * 1.0) {}
    CComplex(const CComplex &c) : re(c.re), im(c.im) {}
    ~CComplex() {}

    CComplex &operator=(const CComplex &c)
    {
        re = c.re;
        im = c.im;
        return *this;
    }

    double getRe() {return re;}
    void setRe(double re1) {re = re1;}

    double getIm() {return im;}
    void setIm(double im1) {im = im1;}

    double getModul() {return sqrt(re * re + im * im);}

    int operator==(CComplex &c1)
    {
        return ((re == c1.re) && (im == c1.im));
    }

    CComplex pow2()
    {
        CComplex rez;
        rez.re = powl(re * 1.0, 2) - powl(im * 1.0, 2);
        rez.im = 2.0 * re * im;
        return rez;
    }

    friend CComplex operator+(const CComplex &c1, const CComplex &c2);
    friend CComplex operator*(CComplex &c1, CComplex &c2);

    void print(FILE *f)
    {
        fprintf(f, "%.20f%+.20f i", re, im);
    }

private:
    double re, im;
};

CComplex operator+(const CComplex &c1, const CComplex &c2)
{
    CComplex rez(c1.re + c2.re, c1.im + c2.im);
    return rez;
}

CComplex operator*(CComplex &c1, CComplex &c2)
{
    CComplex rez(c1.re * c2.re - c1.im * c2.im,
                 c1.re * c2.im + c1.im * c2.re);
    return rez;
}

class C2coord
{
public:
    C2coord()
    {
        m.x = m.y = 0;
    }

    C2coord(double x, double y)
    {
        m.x = x;
        m.y = y;
    }

    C2coord(const C2coord &p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
    }

    C2coord &operator=(C2coord &p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
        return *this;
    }

    int operator==(C2coord &p)
    {
        return ((m.x == p.m.x) && (m.y == p.m.y));
    }

protected:
    struct SDate
    {
        double x,y;
    } m;
};

class CPunct : public C2coord
{
public:
    CPunct() : C2coord(0.0, 0.0)
    {}

    CPunct(double x, double y) : C2coord(x, y)
    {}

    CPunct &operator=(const CPunct &p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
        return *this;
    }

    void getxy(double &x, double &y)
    {
        x = m.x;
        y = m.y;
    }

    int operator==(CPunct &p)
    {
        return ((m.x == p.m.x) && (m.y == p.m.y));
    }

    void marcheaza()
    {
        glBegin(GL_POINTS);
        glVertex2d(m.x, m.y);
        glEnd();
    }

    void print(FILE *fis)
    {
        fprintf(fis, "(%+f,%+f)", m.x, m.y);
    }

};

class CVector : public C2coord
{
public:
    CVector() : C2coord(0.0, 0.0)
    {
        normalizare();
    }

    CVector(double x, double y) : C2coord(x, y)
    {
        normalizare();
    }

    CVector &operator=(CVector &p)
    {
        m.x = p.m.x;
        m.y = p.m.y;
        return *this;
    }

    int operator==(CVector &p)
    {
        return ((m.x == p.m.x) && (m.y == p.m.y));
    }

    CPunct getDest(CPunct &orig, double lungime)
    {
        double x, y;
        orig.getxy(x, y);
        CPunct p(x + m.x * lungime, y + m.y * lungime);
        return p;
    }

    void rotatie(double grade)
    {
        double x = m.x;
        double y = m.y;
        double t = 2 * (4.0 * atan(1.0)) * grade / 360.0;
        m.x = x * cos(t) - y * sin(t);
        m.y = x * sin(t) + y * cos(t);
        normalizare();
    }

    void deseneaza(CPunct p, double lungime)
    {
        double x, y;
        p.getxy(x, y);
        glColor3f(1.0, 0.1, 0.1);
        glBegin(GL_LINE_STRIP);
        glVertex2d(x, y);
        glVertex2d(x + m.x * lungime, y + m.y * lungime);
        glEnd();
    }

    void print(FILE *fis)
    {
        fprintf(fis, "%+fi %+fj", C2coord::m.x, C2coord::m.y);
    }

private:
    void normalizare()
    {
        double d = sqrt(C2coord::m.x * C2coord::m.x + C2coord::m.y * C2coord::m.y);
        if (d != 0.0)
        {
            C2coord::m.x = C2coord::m.x * 1.0 / d;
            C2coord::m.y = C2coord::m.y * 1.0 / d;
        }
    }
};

class Turtle1{
public:
    void patrat(double lungime, double x, double y){
        CVector v = CVector(1.0,0);
        CPunct p = CPunct(x,y);
        for(int i = 0; i < 4; i++){
            v.deseneaza(p,lungime);
            p = v.getDest(p, lungime);
            v.rotatie(90);
        }
    }
    void fractal(double lungime, int nivel, double x, double y)
    {
        if (nivel == 0)
        {
            patrat(1.98, -0.99, -0.99);
        }
        else
        {
            fractal(lungime/3, nivel - 1, x, y);
            fractal(lungime/3, nivel - 1, x+lungime/3, y);
            fractal(lungime/3, nivel - 1, x+(2*lungime/3), y);
            fractal(lungime/3, nivel - 1, x, y+lungime/3);
            patrat(lungime/3, x+lungime/3, y+lungime/3);
            fractal(lungime/3, nivel - 1, x+(2*lungime/3), y+lungime/3);
            fractal(lungime/3, nivel - 1, x, y+(2*lungime/3));
            fractal(lungime/3, nivel - 1, x+lungime/3, y+(2*lungime/3));
            fractal(lungime/3, nivel - 1, x+(2*lungime/3), y+(2*lungime/3));

        }
    }

    void afisare(double lungime, int nivel)
    {
        CVector v(1, 0);
        fractal(2, nivel, -1, -1);
    }
};

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

double normalize_single(double point,
                        double min,
                        double max,
                        double interval_min,
                        double interval_max) {

    double normalized_point = (point - min) / (max - min);
    return interval_min + (interval_max - interval_min) * normalized_point;
}

std::pair<double, double> normalize(
    std::pair<double, double> point,
    std::pair<double, double> limx,
    std::pair<double, double> limy,
    std::pair<double, double> normalization_interval) {
    return std::make_pair(
            normalize_single(point.first, limx.first, limx.second,
                             normalization_interval.first, normalization_interval.second),
        normalize_single(point.second, limy.first, limy.second,
                         normalization_interval.first, normalization_interval.second)
    );
}

//Multimea Mandlebrot -> Vali
void Display7(){
    int nivel = 30;
    double d = 0.005;

    glBegin(GL_POINTS);
    for (double r = -1; r <= 1; r+=d) {
        for (double c = -1; c <= 1; c+=d) {
            auto viewport_point = std::make_pair(r, c);
            double x = normalize_single(
                    viewport_point.first,
                    -1, 1,
                    -2, 1
            );

            auto o = std::make_pair(
                    x,
                    viewport_point.second
            );

            auto v = std::make_pair(0.0, 0.0);
            int level = 0;
            while (v.first * v.first + v.second * v.second <= 4.0 && level < nivel) {
                double x = v.first * v.first - v.second * v.second + o.first;
                v.second = 2.0 * v.first * v.second + o.second;
                v.first = x;

                level++;
            }

            if (level >= nivel) {
                glColor3d(1.0, 0.0, 0.0);
                glVertex2d(viewport_point.first, viewport_point.second);
            }
            else {
                double interpolation_factor = 0;
                if (level > nivel / 3) {
                    interpolation_factor = (double)level * (0.3 / nivel);
                }
                glColor3d(1.0, 1.0 - interpolation_factor, 1.0 - interpolation_factor);
                glVertex2d(viewport_point.first, viewport_point.second);
            }
        }
    }
    glEnd();
}

//Turtle 1 -> Alex
void Display8(){
    Turtle1 turtle;
    turtle.afisare(0.05, nivel);

    char c[3];
    sprintf(c, "%2d", nivel);
    glRasterPos2d(-0.98,-0.98);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

    glRasterPos2d(-1.0,-0.9);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'T');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '1');

    nivel++;
}

class Turtle2
{
public:
    void arboreTurtle2(double lungime,
                       int nivel,
                       double factordiviziune,
                       CPunct p,
                       CVector v)
    {
        assert(factordiviziune != 0);
        CPunct p1, p2;
        if (nivel == 0)
        {
        }
        else
        {
            v.rotatie(-45);
            v.deseneaza(p, lungime);
            p1 = v.getDest(p, lungime);
            arboreTurtle2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

            v.rotatie(90);
            v.deseneaza(p, lungime);
            p1 = v.getDest(p, lungime);


            v.rotatie(-45);
            v.deseneaza(p1, lungime);
            CPunct p3 = p1;
            p1 = v.getDest(p1, lungime);
            //arboreTurtle2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

            v.rotatie(-90);
            v.deseneaza(p1, lungime / 2);
            p2 = p1;
            p1 = v.getDest(p1, lungime / 2);
            arboreTurtle2(lungime * factordiviziune, nivel -1, factordiviziune, p1, v);

            p1 = p2;
            v.rotatie(120);
            v.deseneaza(p1, lungime / 2 );
            p1 = v.getDest(p1, lungime / 2);
            arboreTurtle2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);



            p1 = p3;
            v.rotatie(25);
            v.deseneaza(p1, lungime);
            p1 = v.getDest(p1, lungime);
            arboreTurtle2(lungime * factordiviziune, nivel - 1, factordiviziune, p1, v);

        }
    }

    void afisare(double lungime, int nivel)
    {
        CVector v(0.0, -1.0);
        CPunct p(0.0, 1.0);

        v.deseneaza(p, 0.25);
        p = v.getDest(p, 0.25);
        arboreTurtle2(lungime, nivel, 0.4, p, v);
    }
};


//Turtle 2 -> Alex
void Display9(){
    Turtle2 turtle;

    char c[3];
    sprintf(c, "%2d", nivel);
    glRasterPos2d(-0.98,-0.98);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'N');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'i');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'v');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '=');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[0]);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c[1]);

    glRasterPos2d(-1.0,-0.9);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'a');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'b');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'o');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'u');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'r');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 't');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'l');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'e');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ' ');
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '2');

    glPushMatrix();
    glLoadIdentity();
    glScaled(0.4, 0.4, 1);
    glTranslated(-0.5, 1, 0.0);
    turtle.afisare(1, nivel);
    glPopMatrix();
    nivel++;
}

void EasterEgg(CPunct &origin, CVector &cVector, double lungime, int nivel) {
    if(nivel == 0) {
        cVector.deseneaza(origin, lungime);
        origin = cVector.getDest(origin, lungime);
        return;
    }

    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(-60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(-60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(-60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
    cVector.rotatie(-60);
    EasterEgg(origin, cVector, lungime / 4.0, nivel - 1);
}

void DrawImage2(CPunct &origin, CVector &cVector, double lungime, int nivel, int direction) {
    if(nivel == 0) {
        cVector.deseneaza(origin, lungime);
        origin = cVector.getDest(origin, lungime);
        return;
    }

    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction);
    cVector.rotatie(60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction * -1);
    cVector.rotatie(60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction);
    cVector.rotatie(-60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction * -1);
    cVector.rotatie(-60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction);
    cVector.rotatie(-60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction * -1);
    cVector.rotatie(-60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction);
    cVector.rotatie(60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction * -1);
    cVector.rotatie(60 * direction);
    DrawImage2(origin, cVector, lungime / 4.0, nivel - 1, direction);
}

//Turtle 3 -> Vali
void Display10(){
    CPunct origin(-0.99, -1.0);
    CVector direction(0.0, 2.0);
    double lungime = 2.0;

    glBegin(GL_LINE);
    DrawImage2(origin, direction, lungime, nivel, -1);
    glEnd();

    nivel++;
}


void Init(void) {

    glClearColor(1.0,1.0,1.0,1.0);

    glLineWidth(1);

    //glPointSize(3);

    glPolygonMode(GL_FRONT, GL_LINE);
}

void DisplaySolutions(void){
    switch(prevKey){
        case '0':
            glClear(GL_COLOR_BUFFER_BIT);
            nivel = 0;
            fprintf(stderr, "nivel = %d\n", nivel);
        case '1':
            glClear(GL_COLOR_BUFFER_BIT);
            Display7();
            break;
        case '2':
            glClear(GL_COLOR_BUFFER_BIT);
            Display8();
            break;
        case '3':
            glClear(GL_COLOR_BUFFER_BIT);
            Display9();
            break;
        case '4':
            glClear(GL_COLOR_BUFFER_BIT);
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

    glutDisplayFunc(DisplaySolutions);

    glutMainLoop();

    return 0;
}

