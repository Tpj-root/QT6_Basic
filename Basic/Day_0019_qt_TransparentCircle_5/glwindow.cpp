#include "glwindow.h"
#include <QMatrix4x4>

void GLWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0); // transparent
}

void GLWindow::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection (perspective)
    QMatrix4x4 proj;
    proj.perspective(45.0f, float(width())/height(), 0.1f, 100.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj.constData());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-3);

    // Wireframe cube
    glColor3f(0,1,0);
    glBegin(GL_LINES);

    float s=0.5f;
    float v[8][3] = {
        {-s,-s,-s},{s,-s,-s},{s,s,-s},{-s,s,-s},
        {-s,-s,s},{s,-s,s},{s,s,s},{-s,s,s}
    };

    int e[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    for(auto &i:e){
        glVertex3fv(v[i[0]]);
        glVertex3fv(v[i[1]]);
    }

    glEnd();
}
