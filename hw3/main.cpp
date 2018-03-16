#include <cstdlib>

#include <GL/glut.h>

// dimensiunea ferestrei in pixeli
#define dim 300


void Init(void) {

    glClearColor(1.0,1.0,1.0,1.0);

    glLineWidth(1);

    //glPointSize(3);

    glPolygonMode(GL_FRONT, GL_LINE);
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
        double line_quotent = (double)l / lines;
        glBegin(GL_LINES);
        glVertex2d(offset_x + line_quotent * grid_size_x, offset_y);
        glVertex2d(offset_x + line_quotent * grid_size_x, offset_y + grid_size_y);
        glEnd();
    }

    // Draw columns
    for(int c = 0; c <= columns; c++) {
        double column_quotent = (double)c / columns;
        glBegin(GL_LINES);
        glVertex2d(offset_x, offset_y + column_quotent * grid_size_y);
        glVertex2d(offset_x + grid_size_x, offset_y + column_quotent * grid_size_y);
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

