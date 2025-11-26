#include <QApplication>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QVBoxLayout>
#include <QWidget>
#include <QDir>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Sudoku Solver - Qt6");
    window.resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QWebEngineView *view = new QWebEngineView(&window);

    // Enable file access (REQUIRED)
    view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    // Path to project folder
    QString basePath = QDir::currentPath();   // e.g. /home/.../webengine

    // Load index.html with base path so JS + CSS loads
    QUrl url = QUrl::fromLocalFile(basePath + "/index.html");
    view->setUrl(url);

    layout->addWidget(view);
    window.show();

    return app.exec();
}

