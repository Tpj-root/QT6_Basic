#include "mainwindow.h"
#include <QPainter>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), xPos(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    resize(800, 600);

    connect(&timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer.start(16); // ~60 FPS
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor circleColor(255, 0, 0, 200); // semi-transparent red
    painter.setBrush(circleColor);
    painter.setPen(Qt::NoPen);

    painter.drawEllipse(xPos, height()/2 - 50, 100, 100);
}

void MainWindow::onTimeout()
{
    xPos += 5;
    if (xPos > width()) xPos = -100;
    update();
}
