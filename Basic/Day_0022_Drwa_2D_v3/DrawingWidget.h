#pragma once
#include <QWidget>
#include <QVector>
#include <QLineF>
#include <QPointF>
#include <QColor>

struct ColoredLine {
    QLineF line;
    QColor color;
};

struct ColoredPoint {
    QPointF pt;
    QColor color;
};

class DrawingWidget : public QWidget {
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    void addLine(const QLineF &, const QColor &);
    void addPoint(const QPointF &, const QColor &);

    void clear();
    void setScale(double s);
    void showGrid(bool on);


protected:
    void paintEvent(QPaintEvent *) override;

private:
    QVector<ColoredLine> lines;
    QVector<ColoredPoint> points;

    // grid + zoom state (declare ONCE)
    double scale = 10.0;
    bool grid = true;
};
