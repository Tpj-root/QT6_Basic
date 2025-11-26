#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include "label_update.h"  // Include our custom library

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget window;
    window.setWindowTitle("My Custom Label Library");
    window.resize(400, 400);
    
    // Create a layout to organize our labels
    QVBoxLayout* layout = new QVBoxLayout(&window);
    
    // 🎨 Create different styled labels using our library
    
    // 1. Happy Label
    ColorfulLabel* happyLabel = new ColorfulLabel("I'm Happy! 😊", &window);
    happyLabel->setHappyStyle();
    layout->addWidget(happyLabel);
    
    // 2. Warning Label  
    ColorfulLabel* warningLabel = new ColorfulLabel("Warning! ⚠️", &window);
    warningLabel->setWarningStyle();
    layout->addWidget(warningLabel);
    
    // 3. Info Label
    ColorfulLabel* infoLabel = new ColorfulLabel("Information ℹ️", &window);
    infoLabel->setInfoStyle();
    layout->addWidget(infoLabel);
    
    // 4. Custom Label
    ColorfulLabel* customLabel = new ColorfulLabel("Custom Style! 🎨", &window);
    customLabel->setCustomStyle("pink", "purple", 18);
    customLabel->makeRounded(20);  // Extra rounded corners
    layout->addWidget(customLabel);
    
    window.show();
    return app.exec();
}
