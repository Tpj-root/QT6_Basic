#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "awesome_button.h"
#include "label_update.h"  // Our previous label library

// Function to show message when button is clicked
void showMessage(const QString& message)
{
    QMessageBox::information(nullptr, "Button Clicked!", message);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QWidget window;
    window.setWindowTitle("Awesome Button Library Demo");
    window.resize(500, 500);
    
    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(&window);
    
    // Add a title label using our previous library
    ColorfulLabel* titleLabel = new ColorfulLabel("Awesome Buttons Gallery 🎨", &window);
    titleLabel->setCustomStyle("purple", "white", 20);
    titleLabel->setFixedHeight(60);
    layout->addWidget(titleLabel);
    
    // 🔘 Create different styled buttons
    
    // 1. Primary Button
    AwesomeButton* primaryBtn = new AwesomeButton("Primary Button 🔵", &window);
    primaryBtn->setPrimaryStyle();
    QObject::connect(primaryBtn, &QPushButton::clicked, []() {
        showMessage("Primary button clicked!");
    });
    layout->addWidget(primaryBtn);
    
    // 2. Success Button
    AwesomeButton* successBtn = new AwesomeButton("Success Button ✅", &window);
    successBtn->setSuccessStyle();
    QObject::connect(successBtn, &QPushButton::clicked, []() {
        showMessage("Success! Operation completed!");
    });
    layout->addWidget(successBtn);
    
    // 3. Danger Button
    AwesomeButton* dangerBtn = new AwesomeButton("Danger Button ❌", &window);
    dangerBtn->setDangerStyle();
    QObject::connect(dangerBtn, &QPushButton::clicked, []() {
        showMessage("Danger! Are you sure?");
    });
    layout->addWidget(dangerBtn);
    
    // 4. Warning Button
    AwesomeButton* warningBtn = new AwesomeButton("Warning Button ⚠️", &window);
    warningBtn->setWarningStyle();
    QObject::connect(warningBtn, &QPushButton::clicked, []() {
        showMessage("Warning! Proceed with caution!");
    });
    layout->addWidget(warningBtn);
    
    // 5. Custom Button
    //AwesomeButton* customBtn = new AwesomeButton("Custom Button 🌈", &window);
    //customBtn->setCustomStyle("linear-gradient(to right, #ff6b6b, #ffa726)", "white", 16);
    //customBtn->makeRounded(20);
    //customBtn->setLargeSize();
    //QObject::connect(customBtn, &QPushButton::clicked, []() {
    //    showMessage("Custom button clicked! So colorful! 🌈");
    //});
    //layout->addWidget(customBtn);
    //
    
    //// 5. Custom Button
    //AwesomeButton* customBtn = new AwesomeButton("Custom Button 🌈", &window);
    //customBtn->setCustomStyle("#ff6b6b", "white", 16);  // Fixed: Use solid color instead of gradient
    //customBtn->makeRounded(20);
    //customBtn->setLargeSize();
    //QObject::connect(customBtn, &QPushButton::clicked, []() {
    //    showMessage("Custom button clicked! So colorful! 🌈");
    //});
    //layout->addWidget(customBtn);
        
    
    // // 5. Custom Button
    // AwesomeButton* customBtn = new AwesomeButton("Custom Button 🌈", &window);
    // customBtn->setCustomStyle("#ff6b6b", "white", 16);
    // customBtn->makeRounded(20);
    // customBtn->setLargeSize();
    // QObject::connect(customBtn, &QPushButton::clicked, []() {
    //     QMessageBox::critical(nullptr, "Custom Button", "Custom button clicked! So colorful! 🌈", QMessageBox::Ok);
    // });
    // layout->addWidget(customBtn);
    

    // 5. Custom Button
    AwesomeButton* customBtn = new AwesomeButton("Custom Button 🌈", &window);
    customBtn->setCustomStyle("#ff6b6b", "white", 16);
    customBtn->makeRounded(20);
    customBtn->setLargeSize();
    QObject::connect(customBtn, &QPushButton::clicked, []() {
        // Create a text file
        QFile file("my_custom_file.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "Custom button was clicked! 🌈\n";
            stream << "Timestamp: " << QDateTime::currentDateTime().toString() << "\n";
            stream << "This file was created by Qt6 App!\n";
            file.close();
            QMessageBox::information(nullptr, "Success", "File 'my_custom_file.txt' created successfully! ✅");
        } else {
            QMessageBox::warning(nullptr, "Error", "Could not create file! ❌");
        }
    });
    layout->addWidget(customBtn);







    
    // 6. Different sizes
    AwesomeButton* smallBtn = new AwesomeButton("Small", &window);
    smallBtn->setPrimaryStyle();
    smallBtn->setSmallSize();
    layout->addWidget(smallBtn);
    
    AwesomeButton* mediumBtn = new AwesomeButton("Medium", &window);
    mediumBtn->setSuccessStyle();
    mediumBtn->setMediumSize();
    layout->addWidget(mediumBtn);
    
    AwesomeButton* largeBtn = new AwesomeButton("Large", &window);
    largeBtn->setDangerStyle();
    largeBtn->setLargeSize();
    layout->addWidget(largeBtn);
    
    // Add some spacing
    layout->addStretch();
    
    window.show();
    return app.exec();
}
