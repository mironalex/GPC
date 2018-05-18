#include <cstdlib>
#include <cmath>

#include <GL/freeglut.h>
#include <cassert>
#include <ctime>

// dimensiunea ferestrei in pixeli
#define dim 300
#define EPS 0.001

unsigned char prevKey;

enum EObiect {cubw, cubs, sferaw, sferas} ob = cubw;


enum Axis {
    X, // X axis
    Y, // Y axis
    Z  // Z axis
};

double step(double angle, double maxStep) {
    if(angle > 0) {
        return angle > maxStep ? maxStep : angle;
    }
    return -angle > maxStep ? -maxStep : angle;
}

class Point {
    void setCoords(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

public:
    double x, y, z;

    Point(double x, double y, double z) {
        this->setCoords(x, y, z);
    }

    Point(Point *a) {
        this->setCoords(a->x, a->y, a->z);
    }

    void rotate(double radians, Axis axis) {
        if(radians < 0) {
            radians = 2.0 * M_PI + radians;
        }

        double sr = sin(radians), cr = cos(radians);
        switch(axis) {
            case X:
                this->setCoords(x, y * cr + z * sr, z * cr - y * sr);
                break;
            case Y:
                this->setCoords(x * cr - z * sr, y, x * sr + z * cr);
                break;
            case Z:
                this->setCoords(x * cr + y * sr, y * cr - x * sr, z);
                break;
        }
    }

    void rotate(double radians, Axis axis, Point origin) {
        this->operator-=(origin);
        this->rotate(radians, axis);
        this->operator+=(origin);
    }

    Point& operator +=(Point p) {
        this->x += p.x;
        this->y += p.y;
        this->z += p.z;
        return *this;
    }

    Point& operator -=(Point p) {
        this->x -= p.x;
        this->y -= p.y;
        this->z -= p.z;
        return *this;
    }

    void display() {
        glVertex3f(this->x, this->y, this->z);
    }

    bool operator==(const Point &other) {
        return this->x == other.x &&
               this->y == other.y &&
               this->z == other.z;
    }
};

class Shape {
public:
    virtual Point* getGravityCenter()=0;
};

class Line : public  Shape {
    Point *a, *b;

public:
    Line(Point *a, Point *b) {
        this->a = a;
        this->b = b;
    }

    Point* getGravityCenter() override {
        return new Point(
                (this->a->x + this->b->x) / 2.0,
                (this->a->y + this->b->y) / 2.0,
                (this->a->z + this->b->z) / 2.0
        );
    }
};

class Triangle : public Shape {
public:
    Point *a, *b, *c;

    Triangle(Point* a, Point* b, Point* c) {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void rotate(double radians, Axis axis) {
        this->a->rotate(radians, axis);
        this->b->rotate(radians, axis);
        this->c->rotate(radians, axis);
    }

    void rotate(double radians, Axis axis, Point origin) {
        this->a->rotate(radians, axis, origin);
        this->b->rotate(radians, axis, origin);
        this->c->rotate(radians, axis, origin);
    }

    bool alignOz(double stepRadians, bool change_gl_matrix) {
        Point *center = this->getGravityCenter();
        if(change_gl_matrix) {
            glMatrixMode(GL_MODELVIEW);
            glTranslated(-center->x, -center->y, -center->z);
        }
        bool rotation = false;

        this->operator-=(*center);

        if(abs(this->b->y - this->a->y) > EPS) {
            double angle = atan2(this->b->y - this->a->y, this->a->z - this->b->z);
            double actualRotationAngle = step(angle, stepRadians);
            this->rotate(actualRotationAngle, X);
            if(change_gl_matrix) {
                glMatrixMode(GL_MODELVIEW);
                glRotated(actualRotationAngle * 180.0 / M_PI, 1, 0, 0);
            }
            rotation = true;
        }
        else if(abs(this->b->x - this->a->x) > EPS) {
            double angle = atan2(this->a->x - this->b->x, this->a->z - this->b->z);
            double actualRotationAngle = step(angle, stepRadians);
            this->rotate(actualRotationAngle, Y);
            if(change_gl_matrix) {
                glMatrixMode(GL_MODELVIEW);
                glRotated(actualRotationAngle * 180.0 / M_PI, 0, 1, 0);
            }
            rotation = true;
        }

        this->operator+=(*center);
        if(change_gl_matrix) {
            glMatrixMode(GL_MODELVIEW);
            glTranslated(center->x, center->y, center->z);
        }
        return rotation;
    }

    void alignToOyz(double stepRadians, double moveStep, bool change_gl_matrix) {
        if(!this->alignOz(stepRadians, change_gl_matrix)) {
            if(abs(this->b->x - this->c->x) > EPS) {
                Point *center = Line(this->a, this->b).getGravityCenter();

                this->operator-=(*center);
                if(change_gl_matrix) {
                    glMatrixMode(GL_MODELVIEW);
                    glTranslated(-center->x, -center->y, -center->z);
                }

                double angle = atan2(-1.0 * this->c->x, this->c->y);
                double actualRotationAngle = step(angle, stepRadians);
                if(change_gl_matrix) {
                    glMatrixMode(GL_MODELVIEW);
                    glRotated(actualRotationAngle * 180.0 / M_PI, 0, 0, 1);
                }
                this->rotate(actualRotationAngle, Z);

                this->operator+=(*center);
                if(change_gl_matrix) {
                    glMatrixMode(GL_MODELVIEW);
                    glTranslated(center->x, center->y, center->z);
                }
            }
            else {
                if(this->a->x != 0 || this->a->y != 0) {
                    this->operator-=(new Point(
                            step(this->a->x, moveStep),
                            step(this->a->y, moveStep),
                            0
                    ));
                }
                else {
                    if(this->c->z != 0) {
                        this->operator-=(new Point(
                                0,
                                0,
                                step(this->c->z, moveStep)
                        ));
                    }
                }
            }
        }
    }

    void display() {
        glBegin(GL_TRIANGLES);
            this->a->display();
            this->b->display();
            this->c->display();
        glEnd();
    }

    Point* getGravityCenter() override {
        return new Point(
                (this->a->x + this->b->x + this->c->x) / 3.0,
                (this->a->y + this->b->y + this->c->y) / 3.0,
                (this->a->z + this->b->z + this->c->z) / 3.0
        );
    }

    Triangle& operator +=(const Point &p) {
        (*this->a) += p;
        (*this->b) += p;
        (*this->c) += p;
        return *this;
    }

    Triangle& operator -=(const Point &p) {
        (*this->a) -= p;
        (*this->b) -= p;
        (*this->c) -= p;
        return *this;
    }
};

class MatrixRotatedTriangle : public Triangle {
    Triangle *t_initial;

public:
    MatrixRotatedTriangle(Point *a, Point *b, Point *c) : Triangle(a, b, c) {
        this->t_initial = new Triangle(
                new Point(a),
                new Point(b),
                new Point(c)
        );
    }

    void display() {
        glBegin(GL_TRIANGLES);
            this->t_initial->a->display();
            this->t_initial->b->display();
            this->t_initial->c->display();
        glEnd();
    }
};

class Cube : public Shape {
    Point* v[0][0][0];

    Cube(Point *center, double size) {
        for(int x = 0; x < 2; x++) {
            for(int y = 0; y < 2; y++) {
                for(int z = 0; z < 2; z++) {
                    this->v[x][y][z] = new Point(
                        center->x + size * ((double)x - 0.5),
                        center->y + size * ((double)y - 0.5),
                        center->z + size * ((double)z - 0.5)
                    );
                }
            }
        }

        assert(*center == *this->getGravityCenter());
    }

    Point* getGravityCenter() override {
        double sum_x = 0;
        double sum_y = 0;
        double sum_z = 0;

        for(int x = 0; x < 2; x++) {
            for (int y = 0; y < 2; y++) {
                for (int z = 0; z < 2; z++) {
                    sum_x += this->v[x][y][z]->x;
                    sum_y += this->v[x][y][z]->y;
                    sum_z += this->v[x][y][z]->z;
                }
            }
        }

        return new Point(
                sum_x / 8.0,
                sum_y / 8.0,
                sum_z / 8.0
        );
    }

    void rotate(double radians, Axis axis) {
        for(int x = 0; x < 2; x++) {
            for (int y = 0; y < 2; y++) {
                for (int z = 0; z < 2; z++) {
                    this->v[x][y][z]->rotate(radians, axis);
                }
            }
        }
    }

    void rotateXY(double radians) {
        Point zero = *this->v[0][0][0];
        this->operator-=(zero);

        this->rotate(M_PI / 2.0, Y);
        this->rotate(radians, X);
        this->rotate(-M_PI / 2.0, Y);

        this->operator+=(zero);
    }

    Cube& operator-=(Point &point) {
        for(int x = 0; x < 2; x++) {
            for (int y = 0; y < 2; y++) {
                for (int z = 0; z < 2; z++) {
                    *this->v[x][y][z] -= point;
                }
            }
        }

        return *this;
    }

    Cube& operator+=(Point &point) {
        for(int x = 0; x < 2; x++) {
            for (int y = 0; y < 2; y++) {
                for (int z = 0; z < 2; z++) {
                    *this->v[x][y][z] += point;
                }
            }
        }

        return *this;
    }

    void display() {
    }
};

void DisplayAxe() {
    int X, Y, Z;
    X = Y = 200;
    Z = 200;

    glLineWidth(2);

    // axa Ox - verde
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0,0,0);
        glVertex3f(X,0,0);
    glEnd();

    // axa Oy - albastru
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0,0,0);
        glVertex3f(0,Y,0);
    glEnd();

    // axa Oz - rosu
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0,0,0);
        glVertex3f(0,0,Z);
    glEnd();

    glLineWidth(1);
}

double rand_double() {
    return (double)rand() / RAND_MAX;
}

double rand_double(double a, double b) {
    return a + (b - a) * rand_double();
}

// cub wireframe
Triangle *t, *t_rotated;
void Display1(bool rotate) {
    glColor3f(0.5,0.5,0.5);

    if(t == nullptr) {
        srand(time(nullptr));

        double x = rand_double(-1.0, 1.0);
            double z = rand_double(-1.0, 1.0);

        auto *a = new Point(rand_double(-1.0, -0.5),
                            rand_double(-1.0, -0.5),
                            rand_double(-1.0, -0.5));

        auto *b = new Point(rand_double(-0.5, 0.4),
                            rand_double(-0.5, 0.4),
                            rand_double(-0.5, 0.4));

        auto *c = new Point(rand_double(0.4, 1.0),
                            rand_double(0.4, 1.0),
                            rand_double(0.4, 1.0));

        t = new Triangle(a, b, c);
    }


    if (rotate) {
        t->alignToOyz(15.0 * M_PI / 180.0, 0.02, false);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    DisplayAxe();
    t->display();
}

double degrees = 0;

// cub solid
void Display2() {
    glClear(GL_COLOR_BUFFER_BIT);
    DisplayAxe();

    double degreeStep = 3.0;
    degrees += degreeStep;
    if(degrees >= 360.0) {
        degrees -= 360;
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glColor3f(1,0,0);

        glRotated(degrees, 1, 1, 1);

        glutWireCube(1);
    glPopMatrix();
}

// sfera wireframe
void Display3() {
    glColor3f(0,0,1);
    glutWireSphere(1, 10, 10);
}

// sfera solida
void Display4() {
    glColor3f(0,0,1);
    glutSolidSphere(1, 10, 10);
}

void DisplayObiect()
{
    switch (ob) {
    case cubw:
        Display1(false);
        break;
    case cubs:
        Display2();
        break;
    case sferaw:
        Display3();
        break;
    case sferas:
        Display4();
        break;
    default:
        break;
    }
}

// rotatia cu un unghi de 10 grade in raport cu axa x
void DisplayX() {
    glMatrixMode(GL_MODELVIEW);
    glRotated(10,1,0,0);
}

// rotatia cu un unghi de 10 grade in raport cu axa y
void DisplayY() {
    glMatrixMode(GL_MODELVIEW);
    glRotated(10,0,1,0);
}

// rotatia cu un unghi de 10 grade in raport cu axa z
void DisplayZ() {
  glMatrixMode(GL_MODELVIEW);
  glRotated(10,0,0,1);
}

// Translatia cu 0.2, 0.2, 0.2
void DisplayT() {
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0.2, 0.2, 0.2);
}

// Scalarea cu 1.2, 1.2, 1.2
void DisplayS() {
  glMatrixMode(GL_MODELVIEW);
  glScalef(1.2, 1.2, 1.2);
}

void Init(void) {
  glClearColor(1, 1, 1, 1);
  glLineWidth(2);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-10, 10, -10, 10, 30, -30);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotated(20, 1, 0, 0);
  glRotated(-20, 0, 1, 0);
}

void Display(void) {
  switch(prevKey)
  {
  case 'a':
    DisplayAxe();
    break;
  case '0':
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(20, 1, 0, 0);
    glRotated(-20, 0, 1, 0);
    break;
  case '1':
    Display1(true);
    ob = cubw;
    break;
  case '2':
    Display2();
    ob = cubs;
    break;
  case '3':
    Display3();
    ob = sferaw;
    break;
  case '4':
    Display4();
    ob = sferas;
    break;
  case 'x':
    glClear(GL_COLOR_BUFFER_BIT);
    DisplayX();
    DisplayAxe();
    DisplayObiect();
    break;
  case 'y':
    glClear(GL_COLOR_BUFFER_BIT);
    DisplayY();
    DisplayAxe();
    DisplayObiect();
    break;
  case 'z':
    glClear(GL_COLOR_BUFFER_BIT);
    DisplayZ();
    DisplayAxe();
    DisplayObiect();
    break;
  case 't':
    glClear(GL_COLOR_BUFFER_BIT);
    DisplayT();
    DisplayAxe();
    DisplayObiect();
    break;
  case 's':
    glClear(GL_COLOR_BUFFER_BIT);
    DisplayS();
    DisplayAxe();
    DisplayObiect();
    break;
  default:
    break;
  }
  glutSwapBuffers();
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

   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

   glutCreateWindow (argv[0]);

   Init();

   glutReshapeFunc(Reshape);

   glutKeyboardFunc(KeyboardFunc);

   glutMouseFunc(MouseFunc);

   glutDisplayFunc(Display);

   glutMainLoop();

   return 0;
}