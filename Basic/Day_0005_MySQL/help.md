Let me guide you through creating a MySQL database and then create a Qt6 application to manage products! 🗄️

## Step 1: Create MySQL Database and Table

First, let's create the database and table:

```sql
-- Connect to MySQL as root
sudo mysql -u root

-- Create database
CREATE DATABASE product_manager;
USE product_manager;

-- Create products table
CREATE TABLE products (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL,
    quantity INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert sample data
INSERT INTO products (name, description, price, quantity) VALUES
('Laptop', 'High-performance laptop with 16GB RAM', 999.99, 10),
('Mouse', 'Wireless optical mouse', 25.50, 50),
('Keyboard', 'Mechanical gaming keyboard', 79.99, 25),
('Monitor', '27-inch 4K display monitor', 299.99, 15),
('Headphones', 'Noise-cancelling headphones', 149.99, 30);

-- Create a user for the application (optional but recommended)
CREATE USER 'app_user'@'localhost' IDENTIFIED BY 'password123';
GRANT ALL PRIVILEGES ON product_manager.* TO 'app_user'@'localhost';
FLUSH PRIVILEGES;

-- Exit
EXIT;
```

## Step 2: Qt6 MySQL Product Manager

### Header File: `productmanager.h`

```cpp
#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

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
    void setupDatabase();

private:
    void createToolbar();
    void createModel();
    void createView();
    
    QSqlDatabase database;
    QSqlTableModel *model;
    QTableView *view;
    QToolBar *toolbar;
    QStatusBar *statusBar;
};

#endif // PRODUCTMANAGER_H
```

### Implementation File: `productmanager.cpp`

```cpp
#include "productmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QInputDialog>
#include <QHeaderView>

ProductManager::ProductManager(QWidget *parent)
    : QMainWindow(parent), model(nullptr), view(nullptr)
{
    setWindowTitle("Product Manager - MySQL Qt6");
    setMinimumSize(800, 600);
    
    setupDatabase();
    createModel();
    createView();
    createToolbar();
    
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage("Connected to MySQL Database ✅");
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
    database.setUserName("root");      // Change to 'app_user' if using the created user
    database.setPassword("");          // Enter your MySQL root password here
    
    if (!database.open()) {
        QMessageBox::critical(this, "Database Error", 
            "Could not connect to MySQL database!\nError: " + database.lastError().text());
        return;
    }
    
    statusBar->showMessage("Connected to MySQL Database ✅");
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
    
    setCentralWidget(view);
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
    bool ok;
    QString name = QInputDialog::getText(this, "Add Product", "Product Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    
    QString description = QInputDialog::getText(this, "Add Product", "Description:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    
    double price = QInputDialog::getDouble(this, "Add Product", "Price:", 0, 0, 10000, 2, &ok);
    if (!ok) return;
    
    int quantity = QInputDialog::getInt(this, "Add Product", "Quantity:", 0, 0, 10000, 1, &ok);
    if (!ok) return;
    
    QSqlQuery query;
    query.prepare("INSERT INTO products (name, description, price, quantity) VALUES (?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(price);
    query.addBindValue(quantity);
    
    if (query.exec()) {
        statusBar->showMessage("Product added successfully! ✅", 3000);
        refreshProducts();
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
    QString currentName = model->data(model->index(row, 1)).toString();
    QString currentDesc = model->data(model->index(row, 2)).toString();
    double currentPrice = model->data(model->index(row, 3)).toDouble();
    int currentQty = model->data(model->index(row, 4)).toInt();
    
    bool ok;
    QString name = QInputDialog::getText(this, "Edit Product", "Product Name:", QLineEdit::Normal, currentName, &ok);
    if (!ok) return;
    
    QString description = QInputDialog::getText(this, "Edit Product", "Description:", QLineEdit::Normal, currentDesc, &ok);
    if (!ok) return;
    
    double price = QInputDialog::getDouble(this, "Edit Product", "Price:", currentPrice, 0, 10000, 2, &ok);
    if (!ok) return;
    
    int quantity = QInputDialog::getInt(this, "Edit Product", "Quantity:", currentQty, 0, 10000, 1, &ok);
    if (!ok) return;
    
    QSqlQuery query;
    query.prepare("UPDATE products SET name=?, description=?, price=?, quantity=? WHERE id=?");
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(price);
    query.addBindValue(quantity);
    query.addBindValue(id);
    
    if (query.exec()) {
        statusBar->showMessage("Product updated successfully! ✅", 3000);
        refreshProducts();
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
        QSqlQuery query;
        query.prepare("DELETE FROM products WHERE id = ?");
        query.addBindValue(id);
        
        if (query.exec()) {
            statusBar->showMessage("Product deleted successfully! ✅", 3000);
            refreshProducts();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete product: " + query.lastError().text());
        }
    }
}

void ProductManager::refreshProducts()
{
    model->select();
    statusBar->showMessage("Data refreshed ✅", 2000);
}

void ProductManager::about()
{
    QMessageBox::about(this, "About Product Manager",
        "<h2>MySQL Product Manager</h2>"
        "<p>This application demonstrates:</p>"
        "<ul>"
        "<li>MySQL database connection with Qt6</li>"
        "<li>CRUD operations (Create, Read, Update, Delete)</li>"
        "<li>QSqlTableModel for data display</li>"
        "<li>Toolbar with actions</li>"
        "</ul>"
        "<p>Built with Qt6 and MySQL ❤️</p>");
}
```

### Main File: `main.cpp`

```cpp
#include <QApplication>
#include "productmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ProductManager window;
    window.show();
    
    return app.exec();
}
```

## Step 3: CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(ProductManager)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt6_standard_project_setup()

qt6_add_executable(ProductManager
    main.cpp
    productmanager.cpp
)

target_link_libraries(ProductManager Qt6::Core Qt6::Widgets Qt6::Sql)
```

## 🗄️ MySQL Setup Commands Summary:

```bash
# Connect to MySQL
sudo mysql -u root

# Then run these SQL commands:
CREATE DATABASE product_manager;
USE product_manager;

CREATE TABLE products (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL,
    quantity INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO products (name, description, price, quantity) VALUES
('Laptop', 'High-performance laptop', 999.99, 10),
('Mouse', 'Wireless optical mouse', 25.50, 50);
```

## 🔧 Important Notes:

1. **Install MySQL Connector**: Make sure you have MySQL development libraries:
   ```bash
   sudo apt-get install libmysqlclient-dev
   ```

2. **MySQL Password**: Change the password in `setupDatabase()` if your MySQL root has a password.

3. **Qt SQL Driver**: Qt should automatically find the MySQL driver. If not, you may need to build it separately.

## 🎯 Features:
- **View all products** in a nice table
- **Add new products** with input dialogs
- **Edit existing products** 
- **Delete products** with confirmation
- **Real-time refresh** of data
- **MySQL database integration**

Now you have a complete product management system! 🚀


sudo apt install libqt6sql6-mysql








# Stop MySQL service
sudo systemctl stop mysql

# Start MySQL without password checking
sudo mysqld_safe --skip-grant-tables &

# Connect to MySQL without password
mysql -u root

# In MySQL, run these commands:
FLUSH PRIVILEGES;
ALTER USER 'root'@'localhost' IDENTIFIED BY '';
FLUSH PRIVILEGES;
EXIT;

# Restart MySQL normally
sudo systemctl restart mysql

# Now try connecting without password
sudo mysql -u root
