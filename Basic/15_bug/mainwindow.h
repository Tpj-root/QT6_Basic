#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "glwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    GLWidget *gl;
};

#endif // MAINWINDOW_H
