#include <cstdlib>

#include <GL/glut.h>
#include <utility>
#include <vector>
#include <tuple>
#include <cmath>

// dimensiunea ferestrei in pixeli
#define dim 300

class Grid {
    int num_lines_;
    int num_columns_;
    double viewport_start_x_;
    double viewport_start_y_;
    double viewport_size_x_;
    double viewport_size_y_;

    double static GetViewportCoordOnGrid(int axis_size,
                                         double viewport_offset,
                                         double viewport_grid_size,
                                         int point_coord) {
        double quotent = (double)point_coord / axis_size;
        return viewport_offset + quotent * viewport_grid_size;
    }

public:
    void writePixel(int x, int y) {
        double viewport_x, viewport_y;
        std::tie(viewport_x, viewport_y) = this->GetViewportFromInteger(x, y);


        int triangleAmount = 50;
        double radius = 0.05;

        //glEnable(GL_LINE_SMOOTH);
        glLineWidth(2.0);

        glBegin(GL_LINES);
        for(int i = 0; i <= triangleAmount; i++)
        {
            glVertex2d(viewport_x, viewport_y);
            glVertex2d(viewport_x + (radius * cos(i * 2*M_PI / triangleAmount)),
                       viewport_y + (radius * sin(i * 2*M_PI / triangleAmount)));
        }
        glEnd();
    }

    std::pair<double, double> GetViewportFromInteger(int x, int y) {
        double viewport_x = GetViewportCoordOnGrid(
                this->num_lines_,
                this->viewport_start_x_,
                this->viewport_size_x_,
                x
        );

        double viewport_y = GetViewportCoordOnGrid(
                this->num_columns_,
                this->viewport_start_y_,
                this->viewport_size_y_,
                y
        );

        return std::make_pair(viewport_x, viewport_y);
    }

    Grid(int num_lines, int num_columns,
         double viewport_start_x, double viewport_start_y,
         double viewport_size_x, double viewport_size_y) {

        this->num_lines_ = num_lines;
        this->num_columns_ = num_columns;
        this->viewport_start_x_ = viewport_start_x;
        this->viewport_start_y_ = viewport_start_y;
        this->viewport_size_x_ = viewport_size_x;
        this->viewport_size_y_ = viewport_size_y;
    }

    void DrawLineOnSelf(int start_x, int start_y, int end_x, int end_y) {
        int dx = 2, dy = 3;

        int d = 2 * dy - dx;
        int dE = 2 * dy;
        int dNE = 2 * (dy - dx);
        int x = start_x, y = start_y;

        this->writePixel(x, y);
        while (x < end_x)
        {
            if (d <= 0) {
                d += dE;
                x++;
            }
            else
            {
                d += dNE; x++; y++;
            }
            this->writePixel(x, y);
        }
    }

    void DrawSelf() {
        // Viewport offsets
        double offset_x = -0.98;
        double offset_y = -0.98;

        double grid_size_x = 1.96;
        double grid_size_y = 1.96;

        glColor3d(0.5, 0.5, 0.5);

        // Draw lines
        for(int l = 0; l <= this->num_lines_; l++) {
            double viewport_x = GetViewportCoordOnGrid(
                    this->num_lines_,
                    this->viewport_start_x_,
                    this->viewport_size_x_,
                    l
            );

            glBegin(GL_LINES);
            glVertex2d(viewport_x, offset_y);
            glVertex2d(viewport_x, offset_y + grid_size_y);
            glEnd();
        }

        // Draw columns
        for(int c = 0; c <= this->num_columns_; c++) {
            double viewport_y = GetViewportCoordOnGrid(
                    this->num_columns_,
                    this->viewport_start_y_,
                    this->viewport_size_y_,
                    c
            );

            glBegin(GL_LINES);
            glVertex2d(offset_x,viewport_y);
            glVertex2d(offset_x + grid_size_x, viewport_y);
            glEnd();
        }
    }
};


void Init(void) {

    glClearColor(1.0,1.0,1.0,1.0);

    glLineWidth(1);

    //glPointSize(3);

    glPolygonMode(GL_FRONT, GL_LINE);
}


void DisplaySolutions(void){
    glClear(GL_COLOR_BUFFER_BIT);
    Grid *g = new Grid(20, 20, -0.98, -0.98, 1.96, 1.96);

    g->DrawSelf();
    g->DrawLineOnSelf(15, 15, 10, 10);
    g->writePixel(0, 0);
    glFlush();
}

void Reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void KeyboardFunc(unsigned char key, int x, int y) {
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

