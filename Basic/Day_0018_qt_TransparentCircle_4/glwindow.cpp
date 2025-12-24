#include "glwindow.h"
#include <QtMath>
#include <QMatrix4x4>


GLWindow::GLWindow() : angle(0.0f)
{
    // Transparent, double-buffered, vsync-enabled surface
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(1); // vsync
    setFormat(format);

    // Window setup
    setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setGeometry(0, 0, 800, 600);
    setFlags(flags() | Qt::WindowTransparentForInput);

    connect(&timer, &QTimer::timeout, this, &GLWindow::onTimeout);
    timer.start(16);

    elapsed.start();
}

void GLWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST); // depth test for proper cube edges
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
	
	QMatrix4x4 proj;
	proj.perspective(45.0f, float(width())/height(), 0.1f, 100.0f);
	glLoadMatrixf(proj.constData());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(angle, 1.0f, 1.0f, 0.0f);
	
	

    // Draw semi-transparent wireframe cube
    glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
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
    float delta = elapsed.restart() / 16.0f; // smooth rotation
    angle += 1.0f * delta;
    if(angle >= 360.0f) angle -= 360.0f;
    update();
}
