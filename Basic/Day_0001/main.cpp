#include <QApplication>
#include <QWidget>
#include <QLabel>

int main(int argc, char *argv[])
{
    // Step 1: Create the application
    QApplication app(argc, argv);
    
    // Step 2: Create a window
    QWidget window;
    window.setWindowTitle("My First Qt6 App");
    window.resize(300, 200); // width: 300, height: 200
    
    // Step 3: Create a label with text
    QLabel label("Hello World! 👋", &window);
    label.setAlignment(Qt::AlignCenter); // Center the text
    
    // Step 4: Show the window
    window.show();
    
    // Step 5: Start the application loop
    return app.exec();
}
