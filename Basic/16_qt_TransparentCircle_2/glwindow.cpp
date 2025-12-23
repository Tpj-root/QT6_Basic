#include "glwindow.h"
#include <QtMath>
#include <QGuiApplication>

GLWindow::GLWindow() : angle(0.0f)
{
    setFormat(QSurfaceFormat::defaultFormat()); // Ensure alpha support
    setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setGeometry(0, 0, 800, 600);

    // Make click-through
    setFlags(flags() | Qt::WindowTransparentForInput);

    connect(&timer, &QTimer::timeout, this, &GLWindow::onTimeout);
    timer.start(16); // ~60 FPS
}

void GLWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLWindow::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void GLWindow::paintGL()
{
    glClearColor(0,0,0,0); // fully transparent
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = float(width())/height();
    glOrtho(-1.5*aspect, 1.5*aspect, -1.5, 1.5, -10, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, 1.0f, 1.0f, 0.0f);

    // Draw wireframe cube
    glColor4f(1.0f, 0.0f, 0.0f, 0.8f); // semi-transparent red
    glBegin(GL_LINES);

    float s = 0.5f;
    float verts[8][3] = {
        {-s,-s,-s}, { s,-s,-s}, { s, s,-s}, {-s, s,-s},
        {-s,-s, s}, { s,-s, s}, { s, s, s}, {-s, s, s}
    };
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
    for(int i=0;i<12;i++){
        glVertex3fv(verts[edges[i][0]]);
        glVertex3fv(verts[edges[i][1]]);
    }
    glEnd();
}

void GLWindow::onTimeout()
{
    angle += 1.0f;
    if(angle >= 360.0f) angle -= 360.0f;
    update();
}
