#include <cstdlib>

#include <GL/glut.h>
#include <utility>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>

// dimensiunea ferestrei in pixeli
#define dim 300

using std::set;
using std::pair;

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

    void WritePixel(int x, int y) {
        double viewport_x, viewport_y;
        std::tie(viewport_x, viewport_y) = this->GetViewportFromInteger(x, y);


        int triangleAmount = 50;
        double radius = 0.045;

        //glEnable(GL_LINE_SMOOTH);
        glLineWidth(2.0);
        glColor3f(0.5,0.5,0.5);
        glBegin(GL_LINES);
        for(int i = 0; i <= triangleAmount; i++)
        {
            glVertex2d(viewport_x, viewport_y);
            glVertex2d(viewport_x + (radius * cos(i * 2*M_PI / triangleAmount)),
                       viewport_y + (radius * sin(i * 2*M_PI / triangleAmount)));
        }
        glEnd();
    }

    void DrawLine(int start_x, int start_y, int end_x, int end_y) {
        double viewport_start_x, viewport_start_y, viewport_end_x, viewport_end_y;
        std::tie(viewport_start_x, viewport_start_y) = this->GetViewportFromInteger(start_x, start_y);
        std::tie(viewport_end_x, viewport_end_y) = this->GetViewportFromInteger(end_x, end_y);


        glLineWidth(3.0);
        glColor3f(1,0,0);

        glBegin(GL_LINES);
        glVertex2d(viewport_start_x, viewport_start_y);
        glVertex2d(viewport_end_x, viewport_end_y);
    }

public:

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

    void AddStamp(set<pair<int, int> > &container, int x, int y, int stamp_size) {
        for(int i = 0; i <= 0; i++) {
            for (int j = -stamp_size; j <= stamp_size; j++) {
                int x_stamp = x + i;
                int y_stamp = y + j;
                if (0 <= x_stamp && x_stamp <= this->num_lines_) {
                    if (0 <= y_stamp && y_stamp <= this->num_columns_) {
                        container.insert(std::make_pair(x_stamp, y_stamp));
                    }
                }
            }
        }
    }

    set<pair<int, int> > GetLinePointsOnSelfXBounded(int start_x, int start_y, int end_x, int end_y, int stamp_size) {
        set<pair<int, int> > result;
        if (start_x > end_x) {
            std::tie(start_x, end_x) = std::make_pair(end_x, start_x);
        }

        int direction_y = end_y >= start_y ? 1 : -1;

        int dx = end_x - start_x, dy = direction_y * (end_y - start_y);

        int d = 2 * dy - dx;
        int dE = 2 * dy;
        int dNE = 2 * (dy - dx);
        int x = start_x, y = start_y;

        this->AddStamp(result, x, y, stamp_size);
        while (x <= end_x && y * direction_y <= end_y * direction_y) {
            if (d <= 0) {
                d += dE;
                x += 1;
            }
            else
            {
                d += dNE;
                x += 1;
                y += direction_y;
            }

            this->AddStamp(result, x, y, stamp_size);
        }

        return result;
    }

    set<pair<int, int> > GetLinePointsOnSelf(int start_x, int start_y, int end_x, int end_y, int stamp_size) {
        this->DrawLine(start_x, start_y, end_x, end_y);

        int direction_y = end_y >= start_y ? 1 : -1;

        int dx = end_x - start_x, dy = direction_y * (end_y - start_y);

        if (dx >= dy) {
            return GetLinePointsOnSelfXBounded(start_x, start_y, end_x, end_y, stamp_size);
        }
        else {
            auto result = GetLinePointsOnSelfXBounded(start_y, start_x, end_y, end_x, stamp_size);
            set<pair<int, int> > real_coords;
            for (auto point : result) {
                real_coords.insert(std::make_pair(point.second, point.first));
            }
            return real_coords;
        }
    }

    void DrawLineOnSelf(int start_x, int start_y, int end_x, int end_y, int stamp_size) {
        auto result = GetLinePointsOnSelf(start_x, start_y, end_x, end_y, stamp_size);
        for(auto point : result) {
            this->WritePixel(point.first, point.second);
        }
    }

    void DrawSelf() {
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
            glVertex2d(viewport_x, this->viewport_start_y_);
            glVertex2d(viewport_x, this->viewport_start_y_ + this->viewport_size_y_);
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
            glVertex2d(this->viewport_start_x_, viewport_y);
            glVertex2d(this->viewport_start_x_ + this->viewport_size_x_, viewport_y);
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
    Grid *g = new Grid(15, 15, -0.92, -0.92, 1.84, 1.84);

    g->DrawSelf();
    g->DrawLineOnSelf(0, 0, 15, 7, 0);
    g->DrawLineOnSelf(0, 15, 15, 10, 1);

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

