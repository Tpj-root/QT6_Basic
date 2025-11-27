#include <QApplication>
#include "gridmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create our grid manager
    GridManager gridManager;
    gridManager.show();
    
    return app.exec();
}