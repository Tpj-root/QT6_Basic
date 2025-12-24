#pragma once
#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include "DrawingWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void drawFromText();
    void clearScreen();

private:
    QTextEdit *input;
    DrawingWidget *canvas;
};
