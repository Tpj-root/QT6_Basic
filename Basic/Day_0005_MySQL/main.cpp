#include <QApplication>
#include "productmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ProductManager window;
    window.show();
    
    return app.exec();
}