#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

// Forward declarations
class QLineEdit;
class QTextEdit;
class QDoubleSpinBox;
class QSpinBox;

class ProductManager : public QMainWindow
{
    Q_OBJECT

public:
    ProductManager(QWidget *parent = nullptr);
    ~ProductManager();

private slots:
    void addProduct();
    void editProduct();
    void deleteProduct();
    void refreshProducts();
    void about();
    void clearForm();
    void onSelectionChanged();

private:
    void setupDatabase();
    void createToolbar();
    void createModel();
    void createView();
    void createInputForm();
    
    QSqlDatabase database;
    QSqlTableModel *model;
    QTableView *view;
    QToolBar *toolbar;
    QStatusBar *statusBar;
    
    // Input widgets
    QLineEdit *nameEdit;
    QTextEdit *descEdit;
    QDoubleSpinBox *priceSpin;
    QSpinBox *qtySpin;
};

#endif // PRODUCTMANAGER_H
