#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <vector>
#include <array>
#include <bitset>

class SudokuSolver : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuSolver(QWidget *parent = nullptr);
    
private slots:
    void solveSudoku();
    void clearGrid();
    void generateOutput();

private:
    void setupUI();
    void setupGrid();
    void connectSignals();
    
    // Sudoku solving functions (your algorithm)
    bool solveSudokuInternal(std::vector<std::vector<char>> &board);
    static constexpr std::size_t get_cell(std::size_t row, std::size_t col) noexcept;
    static constexpr std::size_t get_next_row(std::size_t row, std::size_t col) noexcept;
    static constexpr std::size_t get_next_col(std::size_t col) noexcept;
    static std::pair<std::size_t, std::size_t> next_empty_position(std::vector<std::vector<char>> &board, std::size_t row, std::size_t col) noexcept;
    bool solve(std::vector<std::vector<char>> &board, std::size_t row_start, std::size_t col_start,
               std::array<std::bitset<9>, 9> &row_contains,
               std::array<std::bitset<9>, 9> &col_contains,
               std::array<std::bitset<9>, 9> &cell_contains) noexcept;
    
    // UI Elements
    QLineEdit *inputGrid[9][9];
    QPushButton *solveButton;
    QPushButton *clearButton;
    QPushButton *generateButton;
    QTextEdit *outputTextEdit;
    QGridLayout *gridLayout;
};

#endif // SUDOKUSOLVER_H
