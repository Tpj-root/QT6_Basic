#pragma once
#include <QOpenGLWindow>
#include <QOpenGLFunctions>

class GLWindow : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
