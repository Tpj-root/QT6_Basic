#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>

class GridCalculator : public QWidget
{
    Q_OBJECT

public:
    GridCalculator(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("9x9 Grid Calculator");
        setMinimumSize(600, 500);
        
        setupUI();
        setupGrid();
    }

private slots:
    void calculateSums()
    {
        // Clear previous results
        rowSumsLabel->clear();
        columnSumsLabel->clear();
        
        // Parse grid data
        QString text = gridTextEdit->toPlainText();
        QStringList lines = text.split('\n');
        
        QVector<QVector<int>> grid(9, QVector<int>(9, 0));
        QVector<int> rowSums(9, 0);
        QVector<int> colSums(9, 0);
        
        // Parse numbers from text
        for (int row = 0; row < 9 && row < lines.size(); ++row) {
            QStringList numbers = lines[row].split(' ', Qt::SkipEmptyParts);
            for (int col = 0; col < 9 && col < numbers.size(); ++col) {
                bool ok;
                int value = numbers[col].toInt(&ok);
                if (ok) {
                    grid[row][col] = value;
                    rowSums[row] += value;
                    colSums[col] += value;
                }
            }
        }
        
        // Display row sums
        QString rowText = "Row Sums:\n";
        for (int i = 0; i < 9; ++i) {
            rowText += QString("Row %1: %2\n").arg(i + 1).arg(rowSums[i]);
        }
        rowSumsLabel->setText(rowText);
        
        // Display column sums
        QString colText = "Column Sums:\n";
        for (int i = 0; i < 9; ++i) {
            colText += QString("Col %1: %2\n").arg(i + 1).arg(colSums[i]);
        }
        columnSumsLabel->setText(colText);
    }
    
    void clearGrid()
    {
        gridTextEdit->clear();
        rowSumsLabel->clear();
        columnSumsLabel->clear();
        setupGrid(); // Reset to empty grid
    }

private:
    void setupUI()
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        // Title
        QLabel *titleLabel = new QLabel("9×9 Grid Calculator", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: blue;");
        mainLayout->addWidget(titleLabel);
        
        // Instructions
        QLabel *instructionLabel = new QLabel(
            "Enter numbers in the 9×9 grid below. Separate numbers with spaces.\n"
            "Example: 1 2 3 4 5 6 7 8 9", this);
        instructionLabel->setWordWrap(true);
        mainLayout->addWidget(instructionLabel);
        
        // Grid input
        gridTextEdit = new QTextEdit(this);
        gridTextEdit->setFixedHeight(200);
        gridTextEdit->setPlaceholderText("Enter 9 rows of numbers here...\nExample:\n1 2 3 4 5 6 7 8 9\n2 3 4 5 6 7 8 9 1\n3 4 5 6 7 8 9 1 2\n...");
        mainLayout->addWidget(gridTextEdit);
        
        // Buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *calculateBtn = new QPushButton("🧮 Calculate Sums", this);
        QPushButton *clearBtn = new QPushButton("🗑️ Clear", this);
        
        buttonLayout->addWidget(calculateBtn);
        buttonLayout->addWidget(clearBtn);
        mainLayout->addLayout(buttonLayout);
        
        // Results area
        QHBoxLayout *resultsLayout = new QHBoxLayout();
        
        rowSumsLabel = new QLabel(this);
        rowSumsLabel->setStyleSheet("background-color: #e8f5e8; padding: 10px; border: 1px solid #ccc;");
        rowSumsLabel->setFixedWidth(150);
        
        columnSumsLabel = new QLabel(this);
        columnSumsLabel->setStyleSheet("background-color: #e8f5e8; padding: 10px; border: 1px solid #ccc;");
        columnSumsLabel->setFixedWidth(150);
        
        resultsLayout->addWidget(rowSumsLabel);
        resultsLayout->addWidget(columnSumsLabel);
        mainLayout->addLayout(resultsLayout);
        
        // Connect buttons
        connect(calculateBtn, &QPushButton::clicked, this, &GridCalculator::calculateSums);
        connect(clearBtn, &QPushButton::clicked, this, &GridCalculator::clearGrid);
    }
    
    void setupGrid()
    {
        // Create empty 9x9 grid template
        QString gridTemplate;
        for (int i = 0; i < 9; ++i) {
            gridTemplate += "0 0 0 0 0 0 0 0 0\n";
        }
        gridTextEdit->setPlainText(gridTemplate);
    }

private:
    QTextEdit *gridTextEdit;
    QLabel *rowSumsLabel;
    QLabel *columnSumsLabel;
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