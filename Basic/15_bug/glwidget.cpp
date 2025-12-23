#include "glwidget.h"
#include <QOpenGLFunctions>
#include <QPainter>
#include <QtMath>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent), angle(0.0f)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);

    connect(&timer, &QTimer::timeout, this, &GLWidget::onTimeout);
    timer.start(16); // ~60 FPS
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
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
    // 12 edges of cube
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

void GLWidget::onTimeout()
{
    angle += 1.0f;
    if(angle >= 360.0f) angle -= 360.0f;
    update();
}
