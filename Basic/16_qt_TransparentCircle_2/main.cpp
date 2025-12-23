#include "glwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GLWindow window;
    window.show();

    return a.exec();
}
