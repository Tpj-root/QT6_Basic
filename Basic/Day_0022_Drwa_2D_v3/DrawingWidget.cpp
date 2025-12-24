#include "DrawingWidget.h"
#include <QPainter>

DrawingWidget::DrawingWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(500, 500);
    setStyleSheet("background:white;");
}

void DrawingWidget::addLine(const QLineF &l, const QColor &c) {
    lines.push_back({l, c});
    update();
}

void DrawingWidget::addPoint(const QPointF &p, const QColor &c) {
    points.push_back({p, c});
    update();
}

void DrawingWidget::clear() {
    lines.clear();
    points.clear();
    update();
}

void DrawingWidget::setScale(double s) {
    if (s > 0) scale = s;
    update();
}

void DrawingWidget::showGrid(bool on) {
    grid = on;
    update();
}

void DrawingWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // move origin to bottom-left
    p.translate(50, height() - 50);
    p.scale(scale, -scale);

    // draw grid
    if (grid) {
        p.setPen(QPen(Qt::lightGray, 0));
        for (int i = -100; i <= 100; ++i) {
            p.drawLine(i, -100, i, 100);
            p.drawLine(-100, i, 100, i);
        }
    }

    // draw lines
    for (auto &l : lines) {
        p.setPen(QPen(l.color, 0));
        p.drawLine(l.line);
    }
}
