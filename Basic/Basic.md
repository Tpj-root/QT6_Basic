# QT6_Basic




What You'll See:

A small window with "Hello World! 👋" centered in the middle!

    QApplication: The "boss" that runs everything 👑

    QWidget: A basic "thing" you can see (window, button, etc.) 🪟

    QLabel: A piece of text 📝

    &window: Means "put me inside this window" 📦


 Our Own Library Structure:

    label_update.h - The "blueprint" 📋

    label_update.cpp - The "instructions" 📖

    main.cpp - Where we "use" our creation 🎯

🎯 Key Concepts:

    Class: Like a cookie cutter 🍪 that makes many cookies

    Inheritance: Our ColorfulLabel is a special type of QLabel

    Methods: Different "actions" our label can do

    Constructor: Special method that runs when label is born




✨ New Features:

    Hover effects - Changes when mouse hovers 🖱️

    Click effects - Visual feedback when pressed 👇

    Different sizes - Small, medium, large 📏

    Icons support - Add images to buttons 🖼️

    Interactive states - Normal, hover, pressed

🎨 Professional Colors:

    Primary: Blue for main actions 🔵

    Success: Green for positive actions ✅

    Danger: Red for delete/cancel ❌

    Warning: Yellow for warnings ⚠️
    
    


# QT6 Basic Practice

This project contains my daily Qt6 learning exercises.  
Each day has its own folder with source code and a CMake build file.

## Project Structure

| Day | Folder Name | Description |
|-----|-------------|-------------|
| 1   | `Day_0001`  | HelloWorld |
| 2   | `Day_0002`  | *(add later)* |
| 3   | `Day_0003`  | *(add later)* |

Update this table each day when new code is added.

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
./HelloWorld

