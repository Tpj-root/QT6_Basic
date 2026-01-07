#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

#include "ImageViewer.hpp"
#include "ProcessingWidget.hpp"
#include "StencilGenerator.hpp"

/**
 * @brief Main application window
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
private:
    // Core components
    StencilGenerator *stencilGenerator_;
    ImageViewer *originalViewer_;
    ImageViewer *processedViewer_;
    ProcessingWidget *processingWidget_;
    
    // UI Components
    QToolBar *mainToolBar_;
    QStatusBar *statusBar_;
    QDockWidget *controlDock_;
    QSplitter *mainSplitter_;
    
    // Actions
    QAction *openAction_;
    QAction *saveAction_;
    QAction *saveAsAction_;
    QAction *exitAction_;
    QAction *zoomInAction_;
    QAction *zoomOutAction_;
    QAction *zoomFitAction_;
    QAction *zoomOriginalAction_;
    QAction *toggleDockAction_;
    QAction *toggleSideBySideAction_;
    QAction *aboutAction_;
    QAction *aboutQtAction_;
    
    // Live preview timer
    QTimer *livePreviewTimer_;
    bool livePreviewEnabled_;
    StencilParams lastPreviewParams_;
    
    // Application state
    QString currentFilePath_;
    QString saveDirectory_;
    QSettings *appSettings_;
    
    // Statistics display
    QLabel *statsLabel_;
    
    // Initialization
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupDockWidget();
    void setupStatusBar();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    
    // Helper functions
    void updateWindowTitle();
    void updateStatusBar();
    void updateStatistics(const StencilResult &result);
    bool confirmUnsavedChanges();
    void showImageInfo(const QImage &image);
    
    // Processing
    void processImage();
    void updateLivePreview();
    
private slots:
    // File operations
    void onOpenFile();
    void onSaveFile();
    void onSaveAsFile();
    void onExit();
    
    // View operations
    void onZoomIn();
    void onZoomOut();
    void onZoomFit();
    void onZoomOriginal();
    void onToggleDock();
    void onToggleSideBySide();
    
    // Processing slots
    void onProcessRequested();
    void onParamsChanged(const StencilParams &params);
    void onProcessingStarted();
    void onProcessingProgress(int percent);
    void onProcessingCompleted(const StencilResult &result);
    void onProcessingError(const QString &error);
    
    // Help
    void onAbout();
    void onAboutQt();
    
    // Live preview
    void onLivePreviewTimer();
    void onLivePreviewToggled(bool enabled);
};

#endif // MAINWINDOW_HPP