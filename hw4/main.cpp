#include <cstdlib>

#include <GL/glut.h>
#include <utility>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>
#include <fstream>
#include <algorithm>
#include <cstdlib>

// dimensiunea ferestrei in pixeli
#define dim 300

using std::set;
using std::pair;
using std::string;

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

    void AddStamp(set<pair<int, int> > &container, int x, int y, int stamp_size, int cadran) {
        int size_x = 0;
        int size_y = 0;
        switch(cadran) {
            case 0:
            case 3:
            case 4:
            case 7:
                size_x = stamp_size;
                break;
            case 1:
            case 2:
            case 5:
            case 6:
                size_y = stamp_size;
        }

        for(int i = -size_x; i <= size_x; i++) {
            for (int j = -size_y; j <= size_y; j++) {
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

    /*   # 7 # 0 #
     *    #  #  #
     *   6 # # # 1
     *      ###
     *   #########
     *      ###
     *   5 # # # 2
     *    #  #  #
     *   # 4 # 3 #
     */
    static int GetCadran(int start_x, int start_y, int end_x, int end_y) {
        if(start_y <= end_y) {
            // 0, 1, 6, 7
            if(start_x <= end_x) {
                // 0, 1
                if(end_y - start_y > end_x - start_x) {
                    return 0;
                }
                else {
                    return 1;
                }
            }
            else {
                // 6, 7
                if(end_y - start_y > start_x - end_x) {
                    return 7;
                }
                else {
                    return 6;
                }
            }
        }
        else {
            // 2, 3, 4, 5
            if(start_x <= end_x) {
                // 2, 3
                if(start_y - end_y > end_x - start_x) {
                    return 3;
                }
                else {
                    return 2;
                }
            }
            else {
                // 4, 5
                if(start_y - end_y > start_x - end_x) {
                    return 4;
                }
                else {
                    return 5;
                }
            }
        }
    }

    static pair<pair<int, int>, pair<int, int>> GetDirections(int cadran) {
        switch(cadran) {
            case 0:
                return std::make_pair(std::make_pair(1, 1), std::make_pair(0, 1));
            case 1:
                return std::make_pair(std::make_pair(1, 0), std::make_pair(1, 1));
            case 2:
                return std::make_pair(std::make_pair(1, -1), std::make_pair(1, 0));
            case 3:
                return std::make_pair(std::make_pair(0, -1), std::make_pair(1, -1));
            case 4:
                return std::make_pair(std::make_pair(-1, -1), std::make_pair(0, -1));
            case 5:
                return std::make_pair(std::make_pair(-1, 0), std::make_pair(-1, -1));
            case 6:
                return std::make_pair(std::make_pair(-1, 1), std::make_pair(-1, 0));
            case 7:
                return std::make_pair(std::make_pair(0, 1), std::make_pair(-1, 1));
            default:
                exit(-1);
        }
    };

    set<pair<int, int> > GetLinePointsOnSelf(int start_x, int start_y, int end_x, int end_y, int stamp_size) {
        this->DrawLine(start_x, start_y, end_x, end_y);
        set<pair<int, int> > result;
        int dx = end_x - start_x, dy = end_y - start_y;

        pair<int, int> direction_under, direction_up;
        int cadran = GetCadran(start_x, start_y, end_x, end_y);
        std::tie(direction_under, direction_up) = GetDirections(cadran);

        int d = (direction_under.first + direction_up.first) * dy -
                (direction_under.second + direction_up.second) * dx;

        int x = start_x, y = start_y;

        this->AddStamp(result, x, y, stamp_size, cadran);
        while (x != end_x || y != end_y) {
            if (d <= 0) {
                d += 2 * direction_under.first * dy - 2 * direction_under.second * dx;
                x += direction_under.first;
                y += direction_under.second;
            }
            else
            {
                d += 2 * direction_up.first * dy - 2 * direction_up.second * dx;
                x += direction_up.first;
                y += direction_up.second;
            }

            this->AddStamp(result, x, y, stamp_size, cadran);
        }

        return result;
    }

    void DrawLineOnSelf(int start_x, int start_y, int end_x, int end_y, int stamp_size) {
        auto result = GetLinePointsOnSelf(start_x, start_y, end_x, end_y, stamp_size);
        for(auto point : result) {
            this->WritePixel(point.first, point.second);
        }
    }

    void AddPointsToCircle(set<pair<int, int> > &container, int x, int y) {
        container.emplace(x, y);

        // ingrosare
        container.emplace(x - 1, y);
        container.emplace(x + 1, y);
    }

    set<pair<int, int> > GetCirclePointsOnSelf(int radius) {
        int x = radius, y = 0;
        double d = 5.0 / 4 - radius;
        set<pair<int, int> > result;
        AddPointsToCircle(result, x, y);
        while (x > y) {
            if (d < 0) {
                d += 2 * y + 3;
                y++;
            } else {
                d += 2 * (y - x) + 5;
                x--;
                y++;
            }
            AddPointsToCircle(result, x, y);
        }

        return result;
    }

    void DrawCircleOnSelf(int radius) {
        auto result = GetCirclePointsOnSelf(radius);
        for(auto point : result) {
            this->WritePixel(point.first, point.second);
        }
    }

    void AddXSegment(set<pair<int, int> > &container, int y, int start_x, int end_x) {
        for(int idx = start_x; idx <= end_x; idx++) {
            container.emplace(idx, y);
        }
        for(int idx = end_x; idx <= start_x; idx++) {
            container.emplace(idx, y);
        }
    }

    set<pair<int, int> > GetElipsePointsOnSelf(int origin_x, int origin_y, int radius_x, int radius_y) {
        set<pair<int, int> > result;
        int xi = 0, x = 0, y = -radius_y;
        double fxpyp = 0.0;
        double deltaV, deltaNV, deltaN;

        AddXSegment(result, origin_y + y, origin_x + xi, origin_x + x);
        while (radius_x * radius_x * (y + 0.5) < radius_y * radius_y * (x - 1))
        {
            deltaV = radius_y * radius_y * ( 2 * x - 1 );
            deltaNV = radius_y * radius_y * ( 2 * x - 1 ) + radius_x * radius_x * (-2 * y - 1);
            if (fxpyp - deltaV <= 0.0) {
                // V este in interior
                fxpyp -= deltaV;
                x--;
                AddXSegment(result, origin_y + y, origin_x + xi, origin_x + x);
            }
            else if (fxpyp - deltaNV <= 0.0) {
                // NV este in interior
                fxpyp -= deltaNV;
                x--;y++;
                AddXSegment(result, origin_y + y, origin_x + xi, origin_x + x);
            }
        }

        while (y < 0)
        {
            deltaNV = radius_y * radius_y * ( 2 * x - 1 ) + radius_x * radius_x * (-2 * y - 1);
            deltaN = radius_x * radius_x * (-2 * y - 1);
            if (fxpyp - deltaNV <= 0.0) {
                // NV este in interior
                fxpyp -= deltaNV;
                x--;y++;
            }
            else {
                // N este in interior
                fxpyp -= deltaN;
                y++;
            }
            AddXSegment(result, origin_y + y, origin_x + xi, origin_x + x);
        }

        return result;
    }

    void DrawElipseOnSelf(int origin_x, int origin_y, int radius_x, int radius_y) {
        auto result = GetElipsePointsOnSelf(origin_x, origin_y, radius_x, radius_y);
        for(auto point : result) {
            this->WritePixel(point.first, point.second);
        }
    }

    double intersectPolyLine(int start_x, int start_y, int end_x, int end_y, int y) {
        if(start_x == end_x) {
            return start_x;
        }

        return start_x + (double)(y - start_y) / (double)(end_y - start_y) * (double)(end_x - start_x);
    }

    bool isBetween(int val, int a, int b) {
        return (a <= val && val <= b) || (b <= val && val <= a);
    }

    set<pair<int, int> > GetPolyPointsOnSelf(std::vector<pair<int, int> > &poly, int ymin, int ymax) {
        set<pair<int, int> > result;

        poly.push_back(poly[0]);
        for(int y = ymin; y <= ymax; y++) {
            std::vector<double> polyIntersectPoints;
            for(int idx = 1; idx < poly.size(); idx++) {
                pair<int, int> start = poly[idx - 1];
                pair<int, int> end = poly[idx];

                if(isBetween(y, start.second, end.second) && start.second != end.second) {
                    double intersection = intersectPolyLine(start.first, start.second, end.first, end.second, y);

                    if((y == end.second && intersection == end.first) ||
                            (y == start.second && intersection == start.first)) {
                        int ymin = std::min(start.second, end.second);
                        if(y == ymin) {
                            polyIntersectPoints.emplace_back(intersection);
                        }
                    }
                    else {
                        polyIntersectPoints.emplace_back(intersection);
                    }
                }
            }

            std::sort(polyIntersectPoints.begin(), polyIntersectPoints.end());
            for(int idx = 0; idx < (int)polyIntersectPoints.size() - 1; idx+=2) {
                auto start_x = (int)(ceil(polyIntersectPoints[idx]));
                auto end_x = (int)(floor(polyIntersectPoints[idx + 1]));
                if(end_x == polyIntersectPoints[idx + 1]) {
                    end_x--;
                }
                AddXSegment(result, y, start_x, end_x);
            }
        }
        poly.pop_back();
        return result;
    }

    void DrawPolyOnSelf(const std::string &filename) {
        int n, x, y;
        std::ifstream fin(filename);
        fin >> n;
        std::vector<pair<int, int> > poly;
        int ymax = -1, ymin = this->num_columns_ + 1;
        for(int i = 0; i < n; i++) {
            fin >> x >> y;
            poly.emplace_back(x, y);
            if(y < ymin) {
                ymin = y;
            }

            if(y > ymax) {
                ymax = y;
            }
        }

        auto result = GetPolyPointsOnSelf(poly, ymin, ymax);
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
    //g->DrawLineOnSelf(0, 0, 15, 7, 0);
    //g->DrawLineOnSelf(0, 15, 15, 10, 1);
    //g->DrawCircleOnSelf(13);
    //g->DrawElipseOnSelf(4, 4, 4, 4);
    g->DrawPolyOnSelf("poly.txt");

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

