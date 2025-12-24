#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QPushButton>


static QColor parseColor(QString s) {
    s = s.toLower();
    if (s=="red") return Qt::red;
    if (s=="green") return Qt::green;
    if (s=="blue") return Qt::blue;
    if (s=="yellow") return Qt::yellow;
    return Qt::black;
}

MainWindow::MainWindow() {
    QWidget *c = new QWidget;
    setCentralWidget(c);

    input = new QTextEdit;
    input->setPlaceholderText(
        "GRID ON\n"
        "SCALE 10\n"
        "POINT 0 0 red\n"
        "CHAIN 0 0  5 5  10 0"
    );

    canvas = new DrawingWidget;

    auto drawBtn = new QPushButton("Run");
    auto clearBtn = new QPushButton("Clear");

    connect(drawBtn, &QPushButton::clicked, this, &MainWindow::drawFromText);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearScreen);

    auto btns = new QHBoxLayout;
    btns->addWidget(drawBtn);
    btns->addWidget(clearBtn);

    auto lay = new QVBoxLayout;
    lay->addWidget(input);
    lay->addLayout(btns);
    lay->addWidget(canvas);

    c->setLayout(lay);
}

void MainWindow::drawFromText() {
    QStringList cmds = input->toPlainText().split('\n');
    QColor currentColor = Qt::black;

    for (QString c : cmds) {
        auto t = c.split(' ', Qt::SkipEmptyParts);
        if (t.isEmpty()) continue;

        if (t[0]=="COLOR") currentColor = parseColor(t[1]);
        else if (t[0]=="GRID") canvas->showGrid(t[1]=="ON");
        else if (t[0]=="SCALE") canvas->setScale(t[1].toDouble());

        else if (t[0]=="POINT")
            canvas->addPoint({t[1].toDouble(), t[2].toDouble()},
                             t.size()==4?parseColor(t[3]):currentColor);

        else if (t[0]=="LINE")
            canvas->addLine({t[1].toDouble(),t[2].toDouble(),
                             t[3].toDouble(),t[4].toDouble()},
                             t.size()==6?parseColor(t[5]):currentColor);

        else if (t[0]=="CHAIN")
            for (int i=1;i+3<t.size();i+=2)
                canvas->addLine({t[i].toDouble(),t[i+1].toDouble(),
                                 t[i+2].toDouble(),t[i+3].toDouble()},
                                 currentColor);
        else if (t[0] == "GRID" && t.size() == 2) {
            canvas->showGrid(t[1].toUpper() == "ON");
        }

        else if (t[0] == "SCALE" && t.size() == 2) {
            canvas->setScale(t[1].toDouble());
        }
        
        else if (t[0]=="SAVE") {
            QFile f(t[1]);
            if (f.open(QFile::WriteOnly))
                QTextStream(&f) << input->toPlainText();
        }
        else if (t[0]=="LOAD") {
            QFile f(t[1]);
            if (f.open(QFile::ReadOnly))
                input->setPlainText(f.readAll());
        }
    }
}

void MainWindow::clearScreen() {
    canvas->clear();
}
