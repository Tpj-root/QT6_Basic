#include "gridmanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

GridManager::GridManager(QWidget *parent) : QWidget(parent)
{
    // Initialize arrays to nullptr
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            inputGrid[i][j] = nullptr;
            displayGrid[i][j] = nullptr;
        }
    }
    
    setWindowTitle("3x3 Grid Manager with Output");
    setFixedSize(800, 500);
    
    setupUI();
    connectSignals();
}

void GridManager::setupUI()
{
    // Create main vertical layout
    mainLayout = new QVBoxLayout(this);
    
    // Create horizontal layout for grids
    gridsLayout = new QHBoxLayout();
    
    // Create left side - Input Grid (3x3 QLineEdit)
    QWidget *inputWidget = new QWidget(this);
    inputLayout = new QGridLayout(inputWidget);
    
    // Create right side - Display Grid (3x3 QLabel)
    QWidget *displayWidget = new QWidget(this);
    displayLayout = new QGridLayout(displayWidget);
    
    setupGrids();
    
    // Add both grids to horizontal layout
    gridsLayout->addWidget(inputWidget);
    gridsLayout->addWidget(displayWidget);
    
    // Create generate button
    generateButton = new QPushButton("Generate Output", this);
    generateButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
    );
    generateButton->setFixedHeight(40);
    
    // Create output text edit
    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setPlaceholderText("Generated output will appear here...");
    outputTextEdit->setStyleSheet(
        "QTextEdit {"
        "   font-family: 'Courier New', monospace;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "   border: 2px solid #ccc;"
        "   background-color: #f9f9f9;"
        "}"
    );
    outputTextEdit->setFixedHeight(120);
    
    // Add all widgets to main layout
    mainLayout->addLayout(gridsLayout);
    mainLayout->addWidget(generateButton);
    mainLayout->addWidget(outputTextEdit);
    
    setLayout(mainLayout);
}

void GridManager::setupGrids()
{
    // Create 3x3 input grid (QLineEdit)
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            // Create input field
            QLineEdit *input = new QLineEdit(this);
            input->setPlaceholderText(QString("R%1C%2").arg(row+1).arg(col+1));
            input->setAlignment(Qt::AlignCenter);
            input->setStyleSheet(
                "QLineEdit {"
                "   font-size: 14px;"
                "   padding: 8px;"
                "   border: 2px solid #ccc;"
                "   background-color: white;"
                "}"
                "QLineEdit:focus {"
                "   border: 2px solid #2196F3;"
                "}"
            );
            input->setFixedSize(80, 40);
            
            inputLayout->addWidget(input, row, col);
            inputGrid[row][col] = input;
            
            // Create display label
            QLabel *display = new QLabel("0", this);
            display->setAlignment(Qt::AlignCenter);
            display->setStyleSheet(
                "QLabel {"
                "   font-size: 16px;"
                "   font-weight: bold;"
                "   padding: 8px;"
                "   border: 2px solid #4CAF50;"
                "   background-color: #e8f5e8;"
                "   color: #2E7D32;"
                "}"
            );
            display->setFixedSize(80, 40);
            
            displayLayout->addWidget(display, row, col);
            displayGrid[row][col] = display;
        }
    }
}

void GridManager::connectSignals()
{
    // Connect all input fields to the text changed slot
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            connect(inputGrid[row][col], &QLineEdit::textChanged, this, &GridManager::onTextChanged);
        }
    }
    
    // Connect generate button
    connect(generateButton, &QPushButton::clicked, this, &GridManager::generateOutput);
}

void GridManager::onTextChanged()
{
    // Find which input field triggered the signal
    QLineEdit *senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) return;
    
    // Find the position of the changed input
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (inputGrid[row][col] == senderEdit) {
                // Update corresponding label
                QString text = senderEdit->text();
                displayGrid[row][col]->setText(text.isEmpty() ? "0" : text);
                return;
            }
        }
    }
}

void GridManager::generateOutput()
{
    QString output;
    
    // Generate 3x3 grid output
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QString value = inputGrid[row][col]->text().trimmed();
            
            // If empty, use #, otherwise use the value
            if (value.isEmpty()) {
                output += "#";
            } else {
                output += value;
            }
            
            // Add space between columns (except last column)
            //if (col < 2) {
            //    output += " ";
            //}
        }
        
        // Add newline between rows (except last row)
        if (row < 9) {
            output += "\n";
        }
    }
    
    // Update output text edit
    outputTextEdit->setPlainText(output);
}

// Public method to get cell value
QString GridManager::getCellValue(int row, int col) const
{
    if (row >= 0 && row < 9 && col >= 0 && col < 9 && inputGrid[row][col]) {
        return inputGrid[row][col]->text();
    }
    return QString();
}

// Public method to set label value
void GridManager::setLabelValue(int row, int col, const QString &value)
{
    if (row >= 0 && row < 9 && col >= 0 && col < 9 && displayGrid[row][col]) {
        displayGrid[row][col]->setText(value);
    }
}