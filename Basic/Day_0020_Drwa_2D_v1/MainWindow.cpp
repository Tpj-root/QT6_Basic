#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

/*
  Convert text color to QColor
*/
static QColor parseColor(QString s) {
    s = s.toLower();
    if (s == "red") return Qt::red;
    if (s == "green") return Qt::green;
    if (s == "blue") return Qt::blue;
    if (s == "yellow") return Qt::yellow;
    if (s == "cyan") return Qt::cyan;
    if (s == "magenta") return Qt::magenta;
    return Qt::black;
}

MainWindow::MainWindow() {
    QWidget *central = new QWidget;
    setCentralWidget(central);

    input = new QTextEdit;
    input->setPlaceholderText(
        "COLOR red\n"
        "CHAIN 0 0  0 10  10 10\n"
        "LINE 10 10  20 5 blue"
    );

    canvas = new DrawingWidget;

    QPushButton *drawBtn  = new QPushButton("Draw");
    QPushButton *clearBtn = new QPushButton("Clear");

    connect(drawBtn,  &QPushButton::clicked, this, &MainWindow::drawFromText);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearScreen);

    QHBoxLayout *btns = new QHBoxLayout;
    btns->addWidget(drawBtn);
    btns->addWidget(clearBtn);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(input);
    layout->addLayout(btns);
    layout->addWidget(canvas);

    central->setLayout(layout);
}

/*
  Supported commands:
  COLOR name
  LINE x1 y1 x2 y2 [color]
  CHAIN x1 y1 x2 y2 x3 y3 ...
*/
void MainWindow::drawFromText() {
    QStringList cmds = input->toPlainText().split('\n');
    QColor currentColor = Qt::black;

    for (QString c : cmds) {
        auto t = c.split(' ', Qt::SkipEmptyParts);
        if (t.isEmpty()) continue;

        if (t[0] == "COLOR" && t.size() == 2) {
            currentColor = parseColor(t[1]);
        }
        else if (t[0] == "LINE" && t.size() >= 5) {
            QColor col = (t.size() == 6) ? parseColor(t[5]) : currentColor;

            canvas->addLine(
                QLineF(t[1].toDouble(), t[2].toDouble(),
                       t[3].toDouble(), t[4].toDouble()),
                col
            );
        }
        else if (t[0] == "CHAIN" && t.size() >= 5 && t.size() % 2 == 1) {
            for (int i = 1; i + 3 < t.size(); i += 2) {
                canvas->addLine(
                    QLineF(t[i].toDouble(), t[i+1].toDouble(),
                           t[i+2].toDouble(), t[i+3].toDouble()),
                    currentColor
                );
            }
        }
    }
}

void MainWindow::clearScreen() {
    canvas->clear();
}
