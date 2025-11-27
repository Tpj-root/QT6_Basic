#include "gridmanager.h"
#include <QHBoxLayout>

GridManager::GridManager(QWidget *parent) : QWidget(parent)
{
    // Initialize arrays to nullptr
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            inputGrid[i][j] = nullptr;
            displayGrid[i][j] = nullptr;
        }
    }
    
    setWindowTitle("3x3 Grid Manager");
    setFixedSize(600, 300);
    
    setupGrids();
    connectSignals();
}

void GridManager::setupGrids()
{
    // Create main horizontal layout
//    mainLayout = new QHBoxLayout(this);
    mainLayout = new QGridLayout(this);

    
    // Create left side - Input Grid (3x3 QLineEdit)
    QWidget *inputWidget = new QWidget(this);
    inputLayout = new QGridLayout(inputWidget);
    
    // Create right side - Display Grid (3x3 QLabel)
    QWidget *displayWidget = new QWidget(this);
    displayLayout = new QGridLayout(displayWidget);
    
    // Create 3x3 input grid (QLineEdit)
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            // Create input field
            QLineEdit *input = new QLineEdit(this);
            input->setPlaceholderText(QString("R%1C%2").arg(row+1).arg(col+1));
            input->setAlignment(Qt::AlignCenter);
            input->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #ccc; }");
            input->setFixedSize(80, 40);
            
            inputLayout->addWidget(input, row, col);
            inputGrid[row][col] = input;
            
            // Create display label
            QLabel *display = new QLabel("0", this);
            display->setAlignment(Qt::AlignCenter);
            display->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; padding: 8px; border: 2px solid #4CAF50; background-color: #e8f5e8; }");
            display->setFixedSize(80, 40);
            
            displayLayout->addWidget(display, row, col);
            displayGrid[row][col] = display;
        }
    }
    
    // Add both grids to main layout
    mainLayout->addWidget(inputWidget);
    mainLayout->addWidget(displayWidget);
    
    setLayout(mainLayout);
}

void GridManager::connectSignals()
{
    // Connect all input fields to the same slot
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            connect(inputGrid[row][col], &QLineEdit::textChanged, this, &GridManager::onTextChanged);
        }
    }
}

void GridManager::onTextChanged()
{
    // Find which input field triggered the signal
    QLineEdit *senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) return;
    
    // Find the position of the changed input
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (inputGrid[row][col] == senderEdit) {
                // Update corresponding label
                QString text = senderEdit->text();
                displayGrid[row][col]->setText(text.isEmpty() ? "0" : text);
                return;
            }
        }
    }
}

// Public method to get cell value
QString GridManager::getCellValue(int row, int col) const
{
    if (row >= 0 && row < 3 && col >= 0 && col < 3 && inputGrid[row][col]) {
        return inputGrid[row][col]->text();
    }
    return QString();
}

// Public method to set label value
void GridManager::setLabelValue(int row, int col, const QString &value)
{
    if (row >= 0 && row < 3 && col >= 0 && col < 3 && displayGrid[row][col]) {
        displayGrid[row][col]->setText(value);
    }
}
