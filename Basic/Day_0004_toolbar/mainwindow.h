#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QStatusBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    // File menu actions
    void newFile();
    void openFile();
    void saveFile();
    
    // Edit menu actions  
    void cut();
    void copy();
    void paste();
    
    // View menu actions
    void toggleToolbar();
    void toggleStatusbar();
    
    // Help menu actions
    void about();
    void aboutQt();

private:
    void createToolbar();
    void createMenus();
    void createStatusBar();
    
    QTextEdit *textEdit;
    QToolBar *mainToolbar;
    QStatusBar *statusBar;
};

#endif // MAINWINDOW_H
