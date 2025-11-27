#include "sudokusolver.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>

SudokuSolver::SudokuSolver(QWidget *parent) : QWidget(parent)
{
    // Initialize grid to nullptr
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            inputGrid[i][j] = nullptr;
        }
    }
    
    setWindowTitle("Sudoku Solver");
    setFixedSize(800, 700);
    
    setupUI();
    connectSignals();
}

void SudokuSolver::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *titleLabel = new QLabel("Sudoku Solver - 9×9 Grid", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: blue; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Instructions
    QLabel *instructionLabel = new QLabel(
        "Enter Sudoku puzzle (1-9), leave empty cells blank. Click 'Solve Sudoku' to solve!", this);
    instructionLabel->setWordWrap(true);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("padding: 5px; background-color: #f0f0f0;");
    mainLayout->addWidget(instructionLabel);
    
    // Create grid container with scroll area
    QWidget *gridContainer = new QWidget(this);
    gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(2);
    
    setupGrid();
    mainLayout->addWidget(gridContainer);
    
    // Buttons layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    solveButton = new QPushButton("🧩 Solve Sudoku", this);
    clearButton = new QPushButton("🗑️ Clear Grid", this);
    generateButton = new QPushButton("📋 Generate Output", this);
    
    solveButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
    );
    
    clearButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #d32f2f; }"
    );
    
    generateButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }"
    );
    
    buttonLayout->addWidget(solveButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(generateButton);
    mainLayout->addLayout(buttonLayout);
    
    // Output area
    QLabel *outputLabel = new QLabel("Solution Output:", this);
    outputLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(outputLabel);
    
    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setPlaceholderText("Sudoku solution will appear here...");
    outputTextEdit->setStyleSheet(
        "QTextEdit {"
        "   font-family: 'Courier New', monospace;"
        "   font-size: 16px;"
        "   padding: 10px;"
        "   border: 2px solid #ccc;"
        "   background-color: #f9f9f9;"
        "}"
    );
    outputTextEdit->setFixedHeight(200);
    mainLayout->addWidget(outputTextEdit);
    
    setLayout(mainLayout);
}

void SudokuSolver::setupGrid()
{
    // Create 9x9 grid with thicker borders for 3x3 boxes
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QLineEdit *input = new QLineEdit(this);
            input->setMaxLength(1); // Only one character
            input->setAlignment(Qt::AlignCenter);
            
            // Set validator to only allow numbers 1-9
            QRegularExpressionValidator *validator = 
                new QRegularExpressionValidator(QRegularExpression("[1-9]"), input);
            input->setValidator(validator);
            
            // Style based on 3x3 box position for better visual separation
            QString style = 
                "QLineEdit {"
                "   font-size: 16px;"
                "   font-weight: bold;"
                "   padding: 8px;"
                "   background-color: white;";
            
            // Add thicker borders for 3x3 boxes
            if (row % 3 == 0) style += "border-top: 3px solid #000;";
            if (row == 8) style += "border-bottom: 3px solid #000;";
            if (col % 3 == 0) style += "border-left: 3px solid #000;";
            if (col == 8) style += "border-right: 3px solid #000;";
            
            // Regular borders for other sides
            if (row % 3 != 0) style += "border-top: 1px solid #ccc;";
            if (row != 8 && (row + 1) % 3 != 0) style += "border-bottom: 1px solid #ccc;";
            if (col % 3 != 0) style += "border-left: 1px solid #ccc;";
            if (col != 8 && (col + 1) % 3 != 0) style += "border-right: 1px solid #ccc;";
            
            style += "}";
            
            input->setStyleSheet(style);
            input->setFixedSize(40, 40);
            
            gridLayout->addWidget(input, row, col);
            inputGrid[row][col] = input;
        }
    }
}

void SudokuSolver::connectSignals()
{
    connect(solveButton, &QPushButton::clicked, this, &SudokuSolver::solveSudoku);
    connect(clearButton, &QPushButton::clicked, this, &SudokuSolver::clearGrid);
    connect(generateButton, &QPushButton::clicked, this, &SudokuSolver::generateOutput);
}

// Your Sudoku solving algorithm implementation
constexpr std::size_t SudokuSolver::get_cell(std::size_t row, std::size_t col) noexcept
{
    return (row / 3) * 3 + col / 3;
}

constexpr std::size_t SudokuSolver::get_next_row(std::size_t row, std::size_t col) noexcept
{
    return row + (col + 1) / 9;
}

constexpr std::size_t SudokuSolver::get_next_col(std::size_t col) noexcept
{
    return (col + 1) % 9;
}

std::pair<std::size_t, std::size_t> SudokuSolver::next_empty_position(
    std::vector<std::vector<char>> &board, std::size_t row, std::size_t col) noexcept
{
    while (row != 9) {
        if (board[row][col] == '.') {
            return {row, col};
        }
        row = get_next_row(row, col);
        col = get_next_col(col);
    }
    return {9, 0};
}

bool SudokuSolver::solve(std::vector<std::vector<char>> &board, std::size_t row_start, std::size_t col_start,
                         std::array<std::bitset<9>, 9> &row_contains,
                         std::array<std::bitset<9>, 9> &col_contains,
                         std::array<std::bitset<9>, 9> &cell_contains) noexcept
{
    auto [row, col] = next_empty_position(board, row_start, col_start);

    if (row == 9) {
        return true;
    }

    std::size_t const cell = get_cell(row, col);
    std::bitset<9> const contains = row_contains[row] | col_contains[col] | cell_contains[cell];
    if (contains.all()) {
        return false;
    }

    for (std::size_t digit_idx = 0; digit_idx < 9; ++digit_idx) {
        if (!contains[digit_idx]) {
            board[row][col] = static_cast<char>(digit_idx + '1');
            row_contains[row].set(digit_idx);
            col_contains[col].set(digit_idx);
            cell_contains[cell].set(digit_idx);
            if (solve(board, row, col, row_contains, col_contains, cell_contains)) {
                return true;
            }

            row_contains[row].reset(digit_idx);
            col_contains[col].reset(digit_idx);
            cell_contains[cell].reset(digit_idx);
        }
    }
    board[row][col] = '.';
    return false;
}

bool SudokuSolver::solveSudokuInternal(std::vector<std::vector<char>> &board)
{
    std::array<std::bitset<9>, 9> row_contains = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<std::bitset<9>, 9> col_contains = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<std::bitset<9>, 9> cell_contains = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (std::size_t row = 0; row < 9; ++row) {
        for (std::size_t col = 0; col < 9; ++col) {
            char digit;
            if ((digit = board[row][col]) != '.') {
                std::size_t digit_idx = digit - '1';
                row_contains[row].set(digit_idx);
                col_contains[col].set(digit_idx);
                std::size_t cell = get_cell(row, col);
                cell_contains[cell].set(digit_idx);
            }
        }
    }
    return solve(board, 0, 0, row_contains, col_contains, cell_contains);
}

void SudokuSolver::solveSudoku()
{
    // Read input from grid
    std::vector<std::vector<char>> board(9, std::vector<char>(9, '.'));
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QString text = inputGrid[row][col]->text().trimmed();
            if (!text.isEmpty() && text[0].isDigit()) {
                board[row][col] = text[0].toLatin1();
            } else {
                board[row][col] = '.';
            }
        }
    }
    
    // Solve the Sudoku
    if (solveSudokuInternal(board)) {
        // Update the grid with solution
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                inputGrid[row][col]->setText(QString(board[row][col]));
                // Style solved cells differently
                inputGrid[row][col]->setStyleSheet(
                    inputGrid[row][col]->styleSheet() + "color: #2E7D32; background-color: #e8f5e8;"
                );
            }
        }
        QMessageBox::information(this, "Success", "Sudoku solved successfully! 🎉");
    } else {
        QMessageBox::warning(this, "Error", "No solution found for this Sudoku puzzle! ❌");
    }
}

void SudokuSolver::clearGrid()
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            inputGrid[row][col]->clear();
            // Reset to original style
            QString originalStyle = 
                "QLineEdit {"
                "   font-size: 16px;"
                "   font-weight: bold;"
                "   padding: 8px;"
                "   background-color: white;"
                "   color: black;";
            
            if (row % 3 == 0) originalStyle += "border-top: 3px solid #000;";
            if (row == 8) originalStyle += "border-bottom: 3px solid #000;";
            if (col % 3 == 0) originalStyle += "border-left: 3px solid #000;";
            if (col == 8) originalStyle += "border-right: 3px solid #000;";
            if (row % 3 != 0) originalStyle += "border-top: 1px solid #ccc;";
            if (row != 8 && (row + 1) % 3 != 0) originalStyle += "border-bottom: 1px solid #ccc;";
            if (col % 3 != 0) originalStyle += "border-left: 1px solid #ccc;";
            if (col != 8 && (col + 1) % 3 != 0) originalStyle += "border-right: 1px solid #ccc;";
            
            originalStyle += "}";
            inputGrid[row][col]->setStyleSheet(originalStyle);
        }
    }
    outputTextEdit->clear();
}

void SudokuSolver::generateOutput()
{
    QString output;
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QString value = inputGrid[row][col]->text().trimmed();
            
            if (value.isEmpty()) {
                output += "#";
            } else {
                output += value;
            }
            
            if (col < 8) {
                output += " ";
            }
        }
        
        if (row < 8) {
            output += "\n";
        }
    }
    
    outputTextEdit->setPlainText(output);
}