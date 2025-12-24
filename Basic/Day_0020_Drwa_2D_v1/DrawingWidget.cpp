#include "DrawingWidget.h"
#include <QPainter>

DrawingWidget::DrawingWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 400);
    setStyleSheet("background:white;");
}

void DrawingWidget::addLine(const QLineF &line, const QColor &color) {
    lines.push_back({line, color});
    update();
}

void DrawingWidget::clear() {
    lines.clear();
    update();
}

void DrawingWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    for (const auto &l : lines) {
        p.setPen(QPen(l.color, 2));
        p.drawLine(l.line);
    }
}
