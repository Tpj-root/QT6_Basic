#include <QApplication>
#include "sudokusolver.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SudokuSolver solver;
    solver.show();
    
    return app.exec();
}