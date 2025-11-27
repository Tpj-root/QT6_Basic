#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

class GridCalculator : public QWidget
{
    Q_OBJECT

public:
    GridCalculator(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Initialize the grid array to nullptr
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                gridTextEdits[row][col] = nullptr;
            }
        }
        
        setWindowTitle("9×9 Grid Calculator - Individual Boxes");
        setMinimumSize(800, 700);
        
        setupUI();
    }

private slots:
    void calculateSums()
    {
        // Clear previous results
        rowSumsLabel->clear();
        columnSumsLabel->clear();
        totalSumLabel->clear();
        
        QVector<int> rowSums(9, 0);
        QVector<int> colSums(9, 0);
        int grandTotal = 0;
        
        // Calculate sums from each text edit
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (gridTextEdits[row][col]) { // Check if pointer is valid
                    QString text = gridTextEdits[row][col]->toPlainText().trimmed();
                    bool ok;
                    int value = text.toInt(&ok);
                    
                    if (ok) {
                        rowSums[row] += value;
                        colSums[col] += value;
                        grandTotal += value;
                        
                        // Visual feedback - change background color
                        gridTextEdits[row][col]->setStyleSheet("background-color: #e8f5e8;");
                    } else {
                        // Invalid input - red background
                        if (!text.isEmpty()) {
                            gridTextEdits[row][col]->setStyleSheet("background-color: #ffcccc;");
                        } else {
                            gridTextEdits[row][col]->setStyleSheet("background-color: white;");
                        }
                    }
                }
            }
        }
        
        // Display row sums
        QString rowText = "<b>Row Sums:</b><br>";
        for (int i = 0; i < 9; ++i) {
            rowText += QString("Row %1: <b>%2</b><br>").arg(i + 1).arg(rowSums[i]);
        }
        rowSumsLabel->setText(rowText);
        
        // Display column sums
        QString colText = "<b>Column Sums:</b><br>";
        for (int i = 0; i < 9; ++i) {
            colText += QString("Col %1: <b>%2</b><br>").arg(i + 1).arg(colSums[i]);
        }
        columnSumsLabel->setText(colText);
        
        // Display grand total
        totalSumLabel->setText(QString("<b>Grand Total: %1</b>").arg(grandTotal));
    }
    
    void clearGrid()
    {
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (gridTextEdits[row][col]) {
                    gridTextEdits[row][col]->clear();
                    gridTextEdits[row][col]->setStyleSheet("background-color: white;");
                }
            }
        }
        rowSumsLabel->clear();
        columnSumsLabel->clear();
        totalSumLabel->clear();
    }
    
    void fillSampleData()
    {
        // Fill with sample data for testing
        int counter = 1;
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (gridTextEdits[row][col]) {
                    gridTextEdits[row][col]->setPlainText(QString::number(counter));
                    counter++;
                }
            }
        }
    }

private:
    void setupUI()
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        // Title
        QLabel *titleLabel = new QLabel("9×9 Grid Calculator - Individual Boxes", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: blue; padding: 10px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // Instructions
        QLabel *instructionLabel = new QLabel(
            "Enter numbers in each box. Each cell has its own QTextEdit.\n"
            "Click 'Calculate' to see row and column sums.", this);
        instructionLabel->setWordWrap(true);
        instructionLabel->setAlignment(Qt::AlignCenter);
        instructionLabel->setStyleSheet("padding: 5px; background-color: #f0f0f0;");
        mainLayout->addWidget(instructionLabel);
        
        // Create scroll area for the grid
        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        
        QWidget *gridContainer = new QWidget;
        QGridLayout *gridLayout = new QGridLayout(gridContainer);
        
        // Create grid labels and text edits
        createGridLayout(gridLayout);
        
        scrollArea->setWidget(gridContainer);
        mainLayout->addWidget(scrollArea);
        
        // Buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *calculateBtn = new QPushButton("🧮 Calculate Sums", this);
        QPushButton *clearBtn = new QPushButton("🗑️ Clear All", this);
        QPushButton *sampleBtn = new QPushButton("📊 Fill Sample Data", this);
        
        calculateBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
        clearBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
        sampleBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px; }");
        
        buttonLayout->addWidget(calculateBtn);
        buttonLayout->addWidget(clearBtn);
        buttonLayout->addWidget(sampleBtn);
        mainLayout->addLayout(buttonLayout);
        
        // Results area
        QHBoxLayout *resultsLayout = new QHBoxLayout();
        
        rowSumsLabel = new QLabel(this);
        rowSumsLabel->setStyleSheet("background-color: #e8f5e8; padding: 10px; border: 2px solid #4CAF50; border-radius: 5px;");
        rowSumsLabel->setFixedWidth(180);
        
        columnSumsLabel = new QLabel(this);
        columnSumsLabel->setStyleSheet("background-color: #e3f2fd; padding: 10px; border: 2px solid #2196F3; border-radius: 5px;");
        columnSumsLabel->setFixedWidth(180);
        
        totalSumLabel = new QLabel(this);
        totalSumLabel->setStyleSheet("background-color: #fff3e0; padding: 10px; border: 2px solid #FF9800; border-radius: 5px; font-size: 16px;");
        totalSumLabel->setFixedWidth(200);
        totalSumLabel->setAlignment(Qt::AlignCenter);
        
        resultsLayout->addWidget(rowSumsLabel);
        resultsLayout->addWidget(columnSumsLabel);
        resultsLayout->addWidget(totalSumLabel);
        resultsLayout->addStretch();
        
        mainLayout->addLayout(resultsLayout);
        
        // Connect buttons
        connect(calculateBtn, &QPushButton::clicked, this, &GridCalculator::calculateSums);
        connect(clearBtn, &QPushButton::clicked, this, &GridCalculator::clearGrid);
        connect(sampleBtn, &QPushButton::clicked, this, &GridCalculator::fillSampleData);
    }
    
    void createGridLayout(QGridLayout *gridLayout)
    {
        // Create column headers
        for (int col = 0; col < 9; ++col) {
            QLabel *colLabel = new QLabel(QString("Col %1").arg(col + 1));
            colLabel->setAlignment(Qt::AlignCenter);
            colLabel->setStyleSheet("font-weight: bold; background-color: #e3f2fd; padding: 5px; border: 1px solid #ccc;");
            gridLayout->addWidget(colLabel, 0, col + 1);
        }
        
        // Create row headers and text edits
        for (int row = 0; row < 9; ++row) {
            // Row header
            QLabel *rowLabel = new QLabel(QString("Row %1").arg(row + 1));
            rowLabel->setAlignment(Qt::AlignCenter);
            rowLabel->setStyleSheet("font-weight: bold; background-color: #e8f5e8; padding: 5px; border: 1px solid #ccc;");
            gridLayout->addWidget(rowLabel, row + 1, 0);
            
            for (int col = 0; col < 9; ++col) {
                QTextEdit *textEdit = new QTextEdit();
                textEdit->setMaximumSize(60, 40);
                textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                textEdit->setPlaceholderText(QString("R%1C%2").arg(row + 1).arg(col + 1));
                textEdit->setStyleSheet("QTextEdit { border: 1px solid #ccc; padding: 2px; }");
                
                gridLayout->addWidget(textEdit, row + 1, col + 1);
                gridTextEdits[row][col] = textEdit;
            }
        }
    }

private:
    QTextEdit *gridTextEdits[9][9];
    QLabel *rowSumsLabel;
    QLabel *columnSumsLabel;
    QLabel *totalSumLabel;
};

// Main function
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    GridCalculator calculator;
    calculator.show();
    
    return app.exec();
}

#include "main.moc"