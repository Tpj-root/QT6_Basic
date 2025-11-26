#include "mainwindow.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt6 Toolbar Demo");
    setMinimumSize(800, 600);
    
    // Create central widget
    textEdit = new QTextEdit(this);
    textEdit->setPlaceholderText("Type something here...");
    setCentralWidget(textEdit);
    
    // Create UI elements
    createToolbar();
    createMenus();
    createStatusBar();
}

void MainWindow::createToolbar()
{
    // Create main toolbar
    mainToolbar = addToolBar("Main Toolbar");
    
    // File actions
    QAction *newAct = new QAction("📄 New", this);
    QAction *openAct = new QAction("📂 Open", this);
    QAction *saveAct = new QAction("💾 Save", this);
    
    // Edit actions
    QAction *cutAct = new QAction("✂️ Cut", this);
    QAction *copyAct = new QAction("📋 Copy", this);
    QAction *pasteAct = new QAction("📝 Paste", this);
    
    // View actions
    QAction *toolbarToggle = new QAction("🛠️ Toggle Toolbar", this);
    QAction *statusbarToggle = new QAction("📊 Toggle Statusbar", this);
    
    // Help actions
    QAction *aboutAct = new QAction("ℹ️ About", this);
    
    // Add actions to toolbar
    mainToolbar->addAction(newAct);
    mainToolbar->addAction(openAct);
    mainToolbar->addAction(saveAct);
    mainToolbar->addSeparator();
    mainToolbar->addAction(cutAct);
    mainToolbar->addAction(copyAct);
    mainToolbar->addAction(pasteAct);
    mainToolbar->addSeparator();
    mainToolbar->addAction(toolbarToggle);
    mainToolbar->addAction(statusbarToggle);
    mainToolbar->addSeparator();
    mainToolbar->addAction(aboutAct);
    
    // Connect signals to slots
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    connect(toolbarToggle, &QAction::triggered, this, &MainWindow::toggleToolbar);
    connect(statusbarToggle, &QAction::triggered, this, &MainWindow::toggleStatusbar);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("📁 File");
    fileMenu->addAction("📄 New", this, &MainWindow::newFile);
    fileMenu->addAction("📂 Open", this, &MainWindow::openFile);
    fileMenu->addAction("💾 Save", this, &MainWindow::saveFile);
    fileMenu->addSeparator();
    fileMenu->addAction("🚪 Exit", this, &QWidget::close);
    
    // Edit menu
    QMenu *editMenu = menuBar()->addMenu("✏️ Edit");
    editMenu->addAction("✂️ Cut", this, &MainWindow::cut);
    editMenu->addAction("📋 Copy", this, &MainWindow::copy);
    editMenu->addAction("📝 Paste", this, &MainWindow::paste);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu("👀 View");
    viewMenu->addAction("🛠️ Toggle Toolbar", this, &MainWindow::toggleToolbar);
    viewMenu->addAction("📊 Toggle Statusbar", this, &MainWindow::toggleStatusbar);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("❓ Help");
    helpMenu->addAction("ℹ️ About", this, &MainWindow::about);
    helpMenu->addAction("🌈 About Qt", this, &MainWindow::aboutQt);
}

void MainWindow::createStatusBar()
{
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage("Ready ✅");
}

// 📁 File actions
void MainWindow::newFile()
{
    textEdit->clear();
    statusBar->showMessage("New file created", 3000);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            textEdit->setPlainText(stream.readAll());
            file.close();
            statusBar->showMessage("Opened: " + fileName, 3000);
        }
    }
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << textEdit->toPlainText();
            file.close();
            statusBar->showMessage("Saved: " + fileName, 3000);
        }
    }
}

// ✏️ Edit actions
void MainWindow::cut()
{
    textEdit->cut();
    statusBar->showMessage("Text cut to clipboard", 2000);
}

void MainWindow::copy()
{
    textEdit->copy();
    statusBar->showMessage("Text copied to clipboard", 2000);
}

void MainWindow::paste()
{
    textEdit->paste();
    statusBar->showMessage("Text pasted from clipboard", 2000);
}

// 👀 View actions
void MainWindow::toggleToolbar()
{
    mainToolbar->setVisible(!mainToolbar->isVisible());
    statusBar->showMessage(mainToolbar->isVisible() ? "Toolbar shown" : "Toolbar hidden", 2000);
}

void MainWindow::toggleStatusbar()
{
    statusBar->setVisible(!statusBar->isVisible());
    statusBar->showMessage(statusBar->isVisible() ? "Statusbar shown" : "Statusbar hidden", 2000);
}

// ❓ Help actions
void MainWindow::about()
{
    QMessageBox::about(this, "About This App", 
        "<h2>Qt6 Toolbar Demo</h2>"
        "<p>This is a demo application showing how to create:</p>"
        "<ul>"
        "<li>Toolbars with actions</li>"
        "<li>Menu bars</li>"
        "<li>Status bars</li>"
        "<li>Custom functions</li>"
        "</ul>"
        "<p>Built with Qt6 and ❤️</p>");
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}
