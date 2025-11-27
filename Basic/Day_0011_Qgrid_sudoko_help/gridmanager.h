#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

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
    void generateOutput(); // Called when generate button is clicked

private:
    void setupUI();
    void setupGrids();
    void connectSignals();
    
    // 3x3 arrays for input fields and display labels
    QLineEdit *inputGrid[9][9];
    QLabel *displayGrid[9][9];
    
    // New UI elements
    QPushButton *generateButton;
    QTextEdit *outputTextEdit;
    
    QVBoxLayout *mainLayout;
    QHBoxLayout *gridsLayout;
    QGridLayout *inputLayout;
    QGridLayout *displayLayout;
};

#endif // GRIDMANAGER_H
