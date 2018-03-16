#include <cstdlib>

#include <GL/glut.h>
#include <utility>
#include <vector>

// dimensiunea ferestrei in pixeli
#define dim 300

#define GRID_NUM_LINES 20
#define GRID_NUM_COLUMNS 20
#define GRID_VIEWPORT_START_X (-0.98)
#define GRID_VIEWPORT_START_Y (-0.98)
#define GRID_VIEWPORT_SIZE_X 1.96
#define GRID_VIEWPORT_SIZE_Y 1.96


void Init(void) {

    glClearColor(1.0,1.0,1.0,1.0);

    glLineWidth(1);

    //glPointSize(3);

    glPolygonMode(GL_FRONT, GL_LINE);
}

double getViewportCoordOnGrid(int axis_size,
                              double viewport_offset,
                              double viewport_grid_size,
                              int point_coord) {
    double quotent = (double)point_coord / axis_size;
    return viewport_offset + quotent * viewport_grid_size;
}

void drawGlVertexAtGridCoords(int x, int y) {
    double viewport_x = getViewportCoordOnGrid(
            GRID_NUM_LINES,
            GRID_VIEWPORT_START_X,
            GRID_VIEWPORT_SIZE_X,
            x
    );

    double viewport_y = getViewportCoordOnGrid(
            GRID_NUM_COLUMNS,
            GRID_VIEWPORT_START_Y,
            GRID_VIEWPORT_SIZE_Y,
            y
    );

    glVertex2d(viewport_x, viewport_y);
}

void DrawLine(int start_x, int start_y, int end_x, int end_y, int lines, int columns) {
    int dx = 2, dy = 3;

    int d = 2 * dy - dx;
    int dE = 2 * dy;
    int dNE = 2 * (dy - dx);
    int x = start_x, y = start_y;

    glBegin(GL_POINTS);
    drawGlVertexAtGridCoords(x, y);
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
        drawGlVertexAtGridCoords(x, y);;
    }
    glEnd();
}


void DrawGrid(int lines, int columns) {
    // Viewport offsets
    double offset_x = -0.98;
    double offset_y = -0.98;

    double grid_size_x = 1.96;
    double grid_size_y = 1.96;

    glColor3d(0.5, 0.5, 0.5);

    // Draw lines
    for(int l = 0; l <= lines; l++) {
        double viewport_x = getViewportCoordOnGrid(
            GRID_NUM_LINES,
            GRID_VIEWPORT_START_X,
            GRID_VIEWPORT_SIZE_X,
            l
        );

        glBegin(GL_LINES);
        glVertex2d(viewport_x, offset_y);
        glVertex2d(viewport_x, offset_y + grid_size_y);
        glEnd();
    }

    // Draw columns
    for(int c = 0; c <= columns; c++) {
        double viewport_y = getViewportCoordOnGrid(
            GRID_NUM_COLUMNS,
            GRID_VIEWPORT_START_Y,
            GRID_VIEWPORT_SIZE_Y,
            c
        );

        glBegin(GL_LINES);
        glVertex2d(offset_x,viewport_y);
        glVertex2d(offset_x + grid_size_x, viewport_y);
        glEnd();
    }
}

void DisplaySolutions(void){
    glClear(GL_COLOR_BUFFER_BIT);
    DrawGrid(20, 20);
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

