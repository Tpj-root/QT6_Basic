## 🎯 Key Features:

### 🛠️ **Toolbar Contains:**
- **File operations**: New, Open, Save
- **Edit operations**: Cut, Copy, Paste  
- **View toggles**: Show/hide toolbar and statusbar
- **Help**: About dialog

### 📋 **Menu Bar Contains:**
- **File menu** with all file operations
- **Edit menu** with text operations
- **View menu** with visibility toggles
- **Help menu** with about dialogs

### 🔧 **How Toolbar Actions Work:**
```cpp
// 1. Create action
QAction *myAction = new QAction("Button Text", this);

// 2. Add to toolbar
toolbar->addAction(myAction);

// 3. Connect to function
connect(myAction, &QAction::triggered, this, &MyClass::myFunction);

// 4. Define the function
void MyClass::myFunction() {
    // This runs when toolbar button is clicked!
}
```

### 💡 **Custom Functions:**
- Each toolbar button runs a specific function
- Status bar shows feedback messages
- File operations actually work!
- Toggle buttons show/hide UI elements

