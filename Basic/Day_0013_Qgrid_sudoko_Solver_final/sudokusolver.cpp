#include "sudokusolver.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

SudokuSolver::SudokuSolver(QWidget *parent) : QWidget(parent), process(nullptr)
{
    // Initialize grid to nullptr
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            inputGrid[i][j] = nullptr;
        }
    }
    
    setWindowTitle("Sudoku Solver with Device Control");
    setFixedSize(450, 900);
    
    setupUI();
    connectSignals();
}

void SudokuSolver::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *titleLabel = new QLabel("🧩 Sudoku Solver with Device Control 📱", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: blue; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Device Control Section
    QLabel *deviceLabel = new QLabel("Device Control:", this);
    deviceLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin-top: 10px;");
    mainLayout->addWidget(deviceLabel);
    
    // Device control buttons
    QHBoxLayout *deviceLayout = new QHBoxLayout();
    
    androidConnectButton = new QPushButton("📱 Connect Android Device", this);
    gcodeSendButton = new QPushButton("⚙️ Send G-code", this);
    
    androidConnectButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9800;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 12px;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: #F57C00; }"
        "QPushButton:pressed { background-color: #EF6C00; }"
    );
    
    gcodeSendButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #9C27B0;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 12px;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: #7B1FA2; }"
        "QPushButton:pressed { background-color: #6A1B9A; }"
    );
    
    deviceLayout->addWidget(androidConnectButton);
    deviceLayout->addWidget(gcodeSendButton);
    mainLayout->addLayout(deviceLayout);
    
    // Status label for device operations
    statusLabel = new QLabel("Ready to connect devices...", this);
    statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #E3F2FD; border: 1px solid #2196F3; border-radius: 5px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);
    
    // Separator
    QLabel *separator = new QLabel("────────────────────────────────────────", this);
    separator->setStyleSheet("color: #ccc; margin: 10px 0;");
    separator->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(separator);
    
    // Sudoku Section
    QLabel *sudokuLabel = new QLabel("Sudoku Solver:", this);
    sudokuLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #D32F2F;");
    mainLayout->addWidget(sudokuLabel);
    
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
    
    // Sudoku buttons layout
    QHBoxLayout *sudokuButtonLayout = new QHBoxLayout();
    
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
    
    sudokuButtonLayout->addWidget(solveButton);
    sudokuButtonLayout->addWidget(clearButton);
    sudokuButtonLayout->addWidget(generateButton);
    mainLayout->addLayout(sudokuButtonLayout);
    
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
    outputTextEdit->setFixedHeight(150);
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
            
            // Store row and col as properties for easy access
            input->setProperty("row", row);
            input->setProperty("col", col);
            
            gridLayout->addWidget(input, row, col);
            inputGrid[row][col] = input;
            
            // Connect text changed signal for auto navigation
            connect(input, &QLineEdit::textChanged, this, &SudokuSolver::onTextChanged);
            
            // Connect key press events for arrow key navigation
            connect(input, &QLineEdit::returnPressed, this, &SudokuSolver::onReturnPressed);
        }
    }
    
    // Set focus to first cell
    if (inputGrid[0][0]) {
        inputGrid[0][0]->setFocus();
    }
}


void SudokuSolver::onTextChanged(const QString &text)
{
    QLineEdit *senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit || text.isEmpty()) return;
    
    int row = senderEdit->property("row").toInt();
    int col = senderEdit->property("col").toInt();
    
    // Move to next cell after input
    QTimer::singleShot(50, this, [this, row, col]() {
        moveToNextCell(row, col);
    });
}

void SudokuSolver::onReturnPressed()
{
    QLineEdit *senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) return;
    
    int row = senderEdit->property("row").toInt();
    int col = senderEdit->property("col").toInt();
    
    moveToNextCell(row, col);
}

void SudokuSolver::moveToNextCell(int currentRow, int currentCol)
{
    int nextRow = currentRow;
    int nextCol = currentCol + 1;
    
    if (nextCol >= 9) {
        nextCol = 0;
        nextRow++;
        
        if (nextRow >= 9) {
            nextRow = 0; // Wrap around to beginning
        }
    }
    
    // Find next empty cell or just move to next position
    if (inputGrid[nextRow][nextCol]) {
        inputGrid[nextRow][nextCol]->setFocus();
        inputGrid[nextRow][nextCol]->selectAll(); // Select text for easy replacement
    }
}

void SudokuSolver::connectSignals()
{
    // Sudoku buttons
    connect(solveButton, &QPushButton::clicked, this, &SudokuSolver::solveSudoku);
    connect(clearButton, &QPushButton::clicked, this, &SudokuSolver::clearGrid);
    connect(generateButton, &QPushButton::clicked, this, &SudokuSolver::generateOutput);
    
    // Device control buttons
    connect(androidConnectButton, &QPushButton::clicked, this, &SudokuSolver::connectAndroidDevice);
    connect(gcodeSendButton, &QPushButton::clicked, this, &SudokuSolver::sendGcode);
    
    // Install event filter for arrow key navigation
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (inputGrid[row][col]) {
                inputGrid[row][col]->installEventFilter(this);
            }
        }
    }
}

bool SudokuSolver::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QLineEdit *edit = qobject_cast<QLineEdit*>(obj);
        
        if (edit) {
            int row = edit->property("row").toInt();
            int col = edit->property("col").toInt();
            
            switch (keyEvent->key()) {
                case Qt::Key_Up:
                    if (row > 0) {
                        inputGrid[row-1][col]->setFocus();
                        inputGrid[row-1][col]->selectAll();
                    }
                    return true;
                    
                case Qt::Key_Down:
                    if (row < 8) {
                        inputGrid[row+1][col]->setFocus();
                        inputGrid[row+1][col]->selectAll();
                    }
                    return true;
                    
                case Qt::Key_Left:
                    if (col > 0) {
                        inputGrid[row][col-1]->setFocus();
                        inputGrid[row][col-1]->selectAll();
                    }
                    return true;
                    
                case Qt::Key_Right:
                    if (col < 8) {
                        inputGrid[row][col+1]->setFocus();
                        inputGrid[row][col+1]->selectAll();
                    }
                    return true;
                    
                case Qt::Key_Backspace:
                    if (edit->text().isEmpty() && col > 0) {
                        // Move to previous cell on backspace when empty
                        inputGrid[row][col-1]->setFocus();
                        inputGrid[row][col-1]->selectAll();
                        return false; // Let default backspace handling occur
                    }
                    break;
                    
                case Qt::Key_Delete:
                    edit->clear();
                    return true;
                    
                default:
                    break;
            }
        }
    }
    
    // Standard event processing
    return QWidget::eventFilter(obj, event);
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
        qDebug() << "Sudoku solved successfully! 🎉";
        //QMessageBox::information(this, "Success", "Sudoku solved successfully! 🎉");

       // QMessageBox *msg = new QMessageBox(this);
       // msg->setWindowTitle("Success");
       // msg->setText("Sudoku solved successfully! 🎉");
       // msg->setIcon(QMessageBox::Information);
       // msg->setStandardButtons(QMessageBox::NoButton);  // no OK button
       // msg->show();
       // 
       // // auto close after 2 seconds
       // QTimer::singleShot(500, msg, &QMessageBox::close);

    } else {

        qDebug() << "No solution found for this Sudoku puzzle! ❌";
        //QMessageBox::warning(this, "Error", "No solution found for this Sudoku puzzle! ❌");
        
       // QMessageBox *msg = new QMessageBox(this);
       // msg->setWindowTitle("Error");
       // msg->setText("No solution found for this Sudoku puzzle! ❌");
       // msg->setIcon(QMessageBox::Information);
       // msg->setStandardButtons(QMessageBox::NoButton);  // no OK button
       // msg->show();
       // 
       // // auto close after 2 seconds
       // QTimer::singleShot(500, msg, &QMessageBox::close);
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

    // --- delete file.txt ---
    QFile::remove("file.txt");
    //QFile::remove("file.txt"); automatically deletes the file if it exists.
}

void SudokuSolver::generateOutput()
{
    QString output;
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {

            QString style = inputGrid[row][col]->styleSheet();
            QString currentValue = inputGrid[row][col]->text().trimmed();

            if (style.contains("background-color: #e8f5e8") && !currentValue.isEmpty()) {
                output += currentValue;
            } else {
                output += "#";
            }
        }

        if (row < 8)
            output += "\n";
    }

    outputTextEdit->setPlainText(output);

    QFile file("file.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream stream(&file);
        stream << output;
        file.close();

        qDebug() << "Solution saved to 'file_solution.txt";
        //// --- Auto close info box ---
        //QMessageBox *msg = new QMessageBox(this);
        //msg->setWindowTitle("Success");
        //msg->setText("Solution saved to 'file_solution.txt' ✅");
        //msg->setIcon(QMessageBox::Information);
        //msg->setStandardButtons(QMessageBox::NoButton);
        //msg->show();
        //QTimer::singleShot(500, msg, &QMessageBox::close);

    } else {

        qDebug() << "Could not save to file! ❌";

        //// --- Auto close warning box ---
        //QMessageBox *msg = new QMessageBox(this);
        //msg->setWindowTitle("Error");
        //msg->setText("Could not save to file! ❌");
        //msg->setIcon(QMessageBox::Warning);
        //msg->setStandardButtons(QMessageBox::NoButton);
        //msg->show();

        //QTimer::singleShot(500, msg, &QMessageBox::close);
    }
}

void SudokuSolver::connectAndroidDevice()
{
    statusLabel->setText("🔄 Connecting to Android device...");
    statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #FFF3E0; border: 1px solid #FF9800; border-radius: 5px;");
    
    // Run ADB connect command
    QProcess *adbProcess = new QProcess(this);
    connect(adbProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, adbProcess](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0) {
            statusLabel->setText("✅ Android device connected successfully!");
            statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #E8F5E8; border: 1px solid #4CAF50; border-radius: 5px;");
        } else {
            statusLabel->setText("❌ Failed to connect to Android device");
            statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #FFEBEE; border: 1px solid #F44336; border-radius: 5px;");
        }
        adbProcess->deleteLater();
    });
    
    connect(adbProcess, &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
        statusLabel->setText("❌ Error connecting to Android device");
        statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #FFEBEE; border: 1px solid #F44336; border-radius: 5px;");
    });
    
    adbProcess->start("adb", QStringList() << "connect" << "192.168.1.3:5555");
}

void SudokuSolver::sendGcode()
{
    statusLabel->setText("🔄 Sending G-code commands...");
    statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #F3E5F5; border: 1px solid #9C27B0; border-radius: 5px;");
    
    // Check if gcode_gen.out exists
    QFile gcodeFile("./gcode_gen.out");
    if (!gcodeFile.exists()) {
        statusLabel->setText("❌ gcode_gen.out not found in current directory!");
        statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #FFEBEE; border: 1px solid #F44336; border-radius: 5px;");
        return;
    }
    
    // Make sure the file is executable
    QFile::setPermissions("./gcode_gen.out", 
        QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
        QFile::ReadGroup | QFile::ExeGroup |
        QFile::ReadOther | QFile::ExeOther);
    
    // Run gcode_gen.out
    process = new QProcess(this);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SudokuSolver::onProcessFinished);
    connect(process, &QProcess::errorOccurred, this, &SudokuSolver::onProcessError);
    
    // Connect to readyRead signal to capture output
    connect(process, &QProcess::readyReadStandardOutput, [this]() {
        QByteArray output = process->readAllStandardOutput();
        QString outputText = QString::fromLocal8Bit(output);
        statusLabel->setText("📤 G-code output: " + outputText.trimmed());
    });
    
    connect(process, &QProcess::readyReadStandardError, [this]() {
        QByteArray error = process->readAllStandardError();
        QString errorText = QString::fromLocal8Bit(error);
        statusLabel->setText("❌ G-code error: " + errorText.trimmed());
    });
    
    process->start("./gcode_gen.out");
}

void SudokuSolver::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        statusLabel->setText("✅ G-code commands sent successfully!");
        statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #E8F5E8; border: 1px solid #4CAF50; border-radius: 5px;");
    } else {
        statusLabel->setText("❌ G-code execution failed with exit code: " + QString::number(exitCode));
        statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #FFEBEE; border: 1px solid #F44336; border-radius: 5px;");
    }
    process->deleteLater();
    process = nullptr;
}

void SudokuSolver::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start gcode_gen.out";
            break;
        case QProcess::Crashed:
            errorMsg = "gcode_gen.out crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "gcode_gen.out timed out";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error to gcode_gen.out";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error from gcode_gen.out";
            break;
        default:
            errorMsg = "Unknown error";
    }
    
    statusLabel->setText("❌ " + errorMsg);
    statusLabel->setStyleSheet("font-size: 14px; padding: 8px; background-color: #FFEBEE; border: 1px solid #F44336; border-radius: 5px;");
}

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