#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QFile>
#include <QFontDatabase>

#include "MainWindow.hpp"

/**
 * @brief Application entry point
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Qt Stencil Generator");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("StencilStudio");
    app.setWindowIcon(QIcon(":/icons/app-icon.png"));
    
    // Set dark theme (optional)
    app.setStyle(QStyleFactory::create("Fusion"));
    
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    // Load stylesheet
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
    }
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    // Handle command line arguments
    if (argc > 1) {
        QString imagePath = QString::fromLocal8Bit(argv[1]);
        // You could add auto-loading of image from command line here
    }
    
    return app.exec();
}