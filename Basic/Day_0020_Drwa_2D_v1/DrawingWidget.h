#pragma once
#include <QWidget>
#include <QVector>
#include <QLineF>
#include <QColor>

/*
  Stores each line with its own color.
*/
struct ColoredLine {
    QLineF line;
    QColor color;
};

class DrawingWidget : public QWidget {
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    // Add a colored line
    void addLine(const QLineF &line, const QColor &color);

    // Clear screen
    void clear();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QVector<ColoredLine> lines;
};
