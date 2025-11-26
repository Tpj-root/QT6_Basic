#include "productmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QInputDialog>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QGroupBox>

ProductManager::ProductManager(QWidget *parent)
    : QMainWindow(parent), model(nullptr), view(nullptr)
{
    setWindowTitle("Product Manager - MySQL Qt6");
    setMinimumSize(1000, 700);
    
    setupDatabase();
    createModel();
    createView();
    createToolbar();
    createInputForm();
    
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    if (database.isOpen()) {
        statusBar->showMessage("Connected to MySQL Database ✅");
    } else {
        statusBar->showMessage("Database not connected ❌");
    }
}

ProductManager::~ProductManager()
{
    if (database.isOpen()) {
        database.close();
    }
}

void ProductManager::setupDatabase()
{
    // Configure MySQL database connection
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setDatabaseName("product_manager");
    database.setUserName("root");      // Try with root first
    database.setPassword("");          // Empty password for local development
    
    if (!database.open()) {
        QMessageBox::critical(this, "Database Error", 
            "Could not connect to MySQL database!\nError: " + database.lastError().text() +
            "\n\nTroubleshooting:\n"
            "1. Make sure MySQL server is running: sudo systemctl start mysql\n"
            "2. Create database: CREATE DATABASE product_manager;\n"
            "3. Check if user has permissions");
        
        // Try alternative connection (no database selected)
        database = QSqlDatabase::addDatabase("QMYSQL", "alternative");
        database.setHostName("localhost");
        database.setUserName("root");
        database.setPassword("");
        
        if (database.open()) {
            QMessageBox::information(this, "Connection Test", 
                "MySQL server is accessible! But cannot connect to 'product_manager' database.\n"
                "Please create the database first.");
        }
        return;
    }
    
    // Check if table exists, if not create it
    QSqlQuery query(database);
    if (!database.tables().contains("products")) {
        query.exec("CREATE TABLE products ("
                  "id INT AUTO_INCREMENT PRIMARY KEY,"
                  "name VARCHAR(100) NOT NULL,"
                  "description TEXT,"
                  "price DECIMAL(10,2) NOT NULL,"
                  "quantity INT NOT NULL,"
                  "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
        
        // Insert sample data
        query.exec("INSERT INTO products (name, description, price, quantity) VALUES "
                  "('Laptop', 'High-performance laptop with 16GB RAM', 999.99, 10),"
                  "('Mouse', 'Wireless optical mouse', 25.50, 50),"
                  "('Keyboard', 'Mechanical gaming keyboard', 79.99, 25)");
    }
}

void ProductManager::createModel()
{
    model = new QSqlTableModel(this, database);
    model->setTable("products");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    
    // Set friendly header names
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Product Name");
    model->setHeaderData(2, Qt::Horizontal, "Description");
    model->setHeaderData(3, Qt::Horizontal, "Price ($)");
    model->setHeaderData(4, Qt::Horizontal, "Quantity");
    model->setHeaderData(5, Qt::Horizontal, "Created Date");
}

void ProductManager::createView()
{
    view = new QTableView(this);
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setAlternatingRowColors(true);
    
    // Adjust column widths
    view->setColumnWidth(0, 50);   // ID
    view->setColumnWidth(1, 150);  // Name
    view->setColumnWidth(2, 250);  // Description
    view->setColumnWidth(3, 100);  // Price
    view->setColumnWidth(4, 80);   // Quantity
    view->setColumnWidth(5, 150);  // Created Date
}

void ProductManager::createInputForm()
{
    // Create input widgets
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Input form group
    QGroupBox *inputGroup = new QGroupBox("Add/Edit Product", this);
    QGridLayout *formLayout = new QGridLayout(inputGroup);
    
    QLabel *nameLabel = new QLabel("Name:", this);
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Enter product name");
    
    QLabel *descLabel = new QLabel("Description:", this);
    descEdit = new QTextEdit(this);
    descEdit->setMaximumHeight(80);
    descEdit->setPlaceholderText("Enter product description");
    
    QLabel *priceLabel = new QLabel("Price:", this);
    priceSpin = new QDoubleSpinBox(this);
    priceSpin->setRange(0, 10000);
    priceSpin->setDecimals(2);
    priceSpin->setPrefix("$ ");
    
    QLabel *qtyLabel = new QLabel("Quantity:", this);
    qtySpin = new QSpinBox(this);
    qtySpin->setRange(0, 10000);
    
    // Form buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("➕ Add Product", this);
    QPushButton *updateButton = new QPushButton("✏️ Update Product", this);
    QPushButton *clearButton = new QPushButton("🗑️ Clear Form", this);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(clearButton);
    
    // Add widgets to form layout
    formLayout->addWidget(nameLabel, 0, 0);
    formLayout->addWidget(nameEdit, 0, 1);
    formLayout->addWidget(priceLabel, 0, 2);
    formLayout->addWidget(priceSpin, 0, 3);
    formLayout->addWidget(descLabel, 1, 0);
    formLayout->addWidget(descEdit, 1, 1, 1, 3);
    formLayout->addWidget(qtyLabel, 2, 0);
    formLayout->addWidget(qtySpin, 2, 1);
    formLayout->addLayout(buttonLayout, 3, 0, 1, 4);
    
    // Add to main layout
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(view);
    
    setCentralWidget(centralWidget);
    
    // Connect buttons
    connect(addButton, &QPushButton::clicked, this, &ProductManager::addProduct);
    connect(updateButton, &QPushButton::clicked, this, &ProductManager::editProduct);
    connect(clearButton, &QPushButton::clicked, this, &ProductManager::clearForm);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged, 
            this, &ProductManager::onSelectionChanged);
}

void ProductManager::createToolbar()
{
    toolbar = addToolBar("Main Toolbar");
    
    QAction *addAct = new QAction("➕ Add Product", this);
    QAction *editAct = new QAction("✏️ Edit Product", this);
    QAction *deleteAct = new QAction("🗑️ Delete Product", this);
    QAction *refreshAct = new QAction("🔄 Refresh", this);
    QAction *aboutAct = new QAction("ℹ️ About", this);
    
    toolbar->addAction(addAct);
    toolbar->addAction(editAct);
    toolbar->addAction(deleteAct);
    toolbar->addSeparator();
    toolbar->addAction(refreshAct);
    toolbar->addSeparator();
    toolbar->addAction(aboutAct);
    
    connect(addAct, &QAction::triggered, this, &ProductManager::addProduct);
    connect(editAct, &QAction::triggered, this, &ProductManager::editProduct);
    connect(deleteAct, &QAction::triggered, this, &ProductManager::deleteProduct);
    connect(refreshAct, &QAction::triggered, this, &ProductManager::refreshProducts);
    connect(aboutAct, &QAction::triggered, this, &ProductManager::about);
}

void ProductManager::addProduct()
{
    if (!database.isOpen()) {
        QMessageBox::warning(this, "Error", "Database not connected!");
        return;
    }
    
    QString name = nameEdit->text().trimmed();
    QString description = descEdit->toPlainText().trimmed();
    double price = priceSpin->value();
    int quantity = qtySpin->value();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Product name cannot be empty!");
        return;
    }
    
    QSqlQuery query(database);
    query.prepare("INSERT INTO products (name, description, price, quantity) VALUES (?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(price);
    query.addBindValue(quantity);
    
    if (query.exec()) {
        statusBar->showMessage("Product added successfully! ✅", 3000);
        refreshProducts();
        clearForm();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add product: " + query.lastError().text());
    }
}

void ProductManager::editProduct()
{
    QModelIndexList selected = view->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Edit Product", "Please select a product to edit.");
        return;
    }
    
    int row = selected.first().row();
    int id = model->data(model->index(row, 0)).toInt();
    
    QString name = nameEdit->text().trimmed();
    QString description = descEdit->toPlainText().trimmed();
    double price = priceSpin->value();
    int quantity = qtySpin->value();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Product name cannot be empty!");
        return;
    }
    
    QSqlQuery query(database);
    query.prepare("UPDATE products SET name=?, description=?, price=?, quantity=? WHERE id=?");
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(price);
    query.addBindValue(quantity);
    query.addBindValue(id);
    
    if (query.exec()) {
        statusBar->showMessage("Product updated successfully! ✅", 3000);
        refreshProducts();
        clearForm();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update product: " + query.lastError().text());
    }
}

void ProductManager::deleteProduct()
{
    QModelIndexList selected = view->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Delete Product", "Please select a product to delete.");
        return;
    }
    
    int row = selected.first().row();
    int id = model->data(model->index(row, 0)).toInt();
    QString name = model->data(model->index(row, 1)).toString();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Product", 
                                 "Are you sure you want to delete '" + name + "'?",
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        QSqlQuery query(database);
        query.prepare("DELETE FROM products WHERE id = ?");
        query.addBindValue(id);
        
        if (query.exec()) {
            statusBar->showMessage("Product deleted successfully! ✅", 3000);
            refreshProducts();
            clearForm();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete product: " + query.lastError().text());
        }
    }
}

void ProductManager::refreshProducts()
{
    if (database.isOpen()) {
        model->select();
        statusBar->showMessage("Data refreshed ✅", 2000);
    }
}

void ProductManager::clearForm()
{
    nameEdit->clear();
    descEdit->clear();
    priceSpin->setValue(0);
    qtySpin->setValue(0);
    view->clearSelection();
}

void ProductManager::onSelectionChanged()
{
    QModelIndexList selected = view->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }
    
    int row = selected.first().row();
    nameEdit->setText(model->data(model->index(row, 1)).toString());
    descEdit->setText(model->data(model->index(row, 2)).toString());
    priceSpin->setValue(model->data(model->index(row, 3)).toDouble());
    qtySpin->setValue(model->data(model->index(row, 4)).toInt());
}

void ProductManager::about()
{
    QMessageBox::about(this, "About Product Manager",
        "<h2>MySQL Product Manager</h2>"
        "<p>This application demonstrates MySQL database integration with Qt6.</p>"
        "<p><b>Features:</b></p>"
        "<ul>"
        "<li>Add, Edit, Delete products</li>"
        "<li>Real-time database operations</li>"
        "<li>Form-based input</li>"
        "<li>Table view with selection</li>"
        "</ul>"
        "<p>Built with Qt6 and MySQL ❤️</p>");
}