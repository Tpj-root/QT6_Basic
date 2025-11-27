#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>

class GridExample : public QWidget 
{
    Q_OBJECT

public:
    GridExample(QWidget *parent = nullptr) : QWidget(parent) 
    {
        setWindowTitle("3x3 QLineEdit Grid");
        setFixedSize(300, 300);
        
        // Create a grid layout (3 rows, 3 columns)
        QGridLayout *gridLayout = new QGridLayout(this);
        
        // Create 3x3 QLineEdit boxes and add them to the grid
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                // Create a new QLineEdit for each cell
                QLineEdit *lineEdit = new QLineEdit(this);
                lineEdit->setPlaceholderText(QString("R%1 C%2").arg(row + 1).arg(col + 1));
                lineEdit->setAlignment(Qt::AlignCenter);  // Center the text
                lineEdit->setStyleSheet("font-size: 14px; padding: 5px;");
                
                // Store in 2D array if you need to access them later
                gridLineEdits[row][col] = lineEdit;
                
                // Add to grid layout at specified row and column position
                gridLayout->addWidget(lineEdit, row, col);
            }
        }
        
        setLayout(gridLayout);
    }

private:
    // 2D array to store references to all QLineEdit boxes
    QLineEdit *gridLineEdits[3][3];
};

int main(int argc, char *argv[]) 
{
    QApplication app(argc, argv);
    
    GridExample window;
    window.show();
    
    return app.exec();
}

#include "main.moc"