#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>

class GridManager : public QWidget
{
    Q_OBJECT

public:
    explicit GridManager(QWidget *parent = nullptr);
    
    // Public methods to access grid data
    QString getCellValue(int row, int col) const;
    void setLabelValue(int row, int col, const QString &value);

private slots:
    void onTextChanged();  // Called when any QLineEdit text changes

private:
    void setupGrids();
    void connectSignals();
    
    // 3x3 arrays for input fields and display labels
    QLineEdit *inputGrid[3][3];
    QLabel *displayGrid[3][3];
    
    QGridLayout *mainLayout;
    QGridLayout *inputLayout;
    QGridLayout *displayLayout;
};

#endif // GRIDMANAGER_H
