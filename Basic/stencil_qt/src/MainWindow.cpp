#include "MainWindow.hpp"
#include <QApplication>
#include <QFileInfo>
#include <QDateTime>

/**
 * @brief MainWindow constructor
 */
MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent)
    , stencilGenerator_(new StencilGenerator(this))
    , livePreviewEnabled_(false) {
    
    // Load application settings
    appSettings_ = new QSettings("StencilStudio", "QtStencilGenerator", this);
    
    setupUI();
    loadSettings();
    
    setWindowTitle("Qt Stencil Generator");
    resize(1400, 800);
}

/**
 * @brief Setup the main user interface
 */
void MainWindow::setupUI() {
    // Create central widget with splitter
    mainSplitter_ = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainSplitter_);
    
    // Create image viewers
    originalViewer_ = new ImageViewer(this);
    processedViewer_ = new ImageViewer(this);
    
    originalViewer_->setObjectName("originalViewer");
    processedViewer_->setObjectName("processedViewer");
    
    // Set viewer properties
    originalViewer_->setViewMode(ImageViewer::VIEW_FIT_WINDOW);
    processedViewer_->setViewMode(ImageViewer::VIEW_FIT_WINDOW);
    
    // Add viewers to splitter
    mainSplitter_->addWidget(originalViewer_);
    mainSplitter_->addWidget(processedViewer_);
    mainSplitter_->setStretchFactor(0, 1);
    mainSplitter_->setStretchFactor(1, 1);
    
    // Create processing widget
    processingWidget_ = new ProcessingWidget(this);
    
    // Create dock widget for controls
    controlDock_ = new QDockWidget(tr("Processing Controls"), this);
    controlDock_->setWidget(processingWidget_);
    controlDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, controlDock_);
    
    // Setup menu bar and toolbars
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    
    // Create live preview timer
    livePreviewTimer_ = new QTimer(this);
    livePreviewTimer_->setInterval(150); // Update every 150ms for smooth preview
    livePreviewTimer_->setSingleShot(true);
}

/**
 * @brief Setup menu bar with all actions
 */
void MainWindow::setupMenuBar() {
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    
    openAction_ = new QAction(tr("&Open..."), this);
    openAction_->setShortcut(QKeySequence::Open);
    openAction_->setIcon(QIcon::fromTheme("document-open"));
    
    saveAction_ = new QAction(tr("&Save"), this);
    saveAction_->setShortcut(QKeySequence::Save);
    saveAction_->setIcon(QIcon::fromTheme("document-save"));
    saveAction_->setEnabled(false);
    
    saveAsAction_ = new QAction(tr("Save &As..."), this);
    saveAsAction_->setShortcut(QKeySequence::SaveAs);
    saveAsAction_->setIcon(QIcon::fromTheme("document-save-as"));
    saveAsAction_->setEnabled(false);
    
    exitAction_ = new QAction(tr("E&xit"), this);
    exitAction_->setShortcut(QKeySequence::Quit);
    
    fileMenu->addAction(openAction_);
    fileMenu->addAction(saveAction_);
    fileMenu->addAction(saveAsAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction_);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    
    zoomInAction_ = new QAction(tr("Zoom &In"), this);
    zoomInAction_->setShortcut(QKeySequence::ZoomIn);
    zoomInAction_->setEnabled(false);
    
    zoomOutAction_ = new QAction(tr("Zoom &Out"), this);
    zoomOutAction_->setShortcut(QKeySequence::ZoomOut);
    zoomOutAction_->setEnabled(false);
    
    zoomFitAction_ = new QAction(tr("&Fit to Window"), this);
    zoomFitAction_->setShortcut(Qt::Key_F);
    zoomFitAction_->setEnabled(false);
    
    zoomOriginalAction_ = new QAction(tr("&Original Size"), this);
    zoomOriginalAction_->setShortcut(Qt::Key_O);
    zoomOriginalAction_->setEnabled(false);
    
    toggleDockAction_ = controlDock_->toggleViewAction();
    toggleDockAction_->setText(tr("Show &Controls"));
    
    toggleSideBySideAction_ = new QAction(tr("&Side by Side View"), this);
    toggleSideBySideAction_->setCheckable(true);
    toggleSideBySideAction_->setChecked(true);
    
    viewMenu->addAction(zoomInAction_);
    viewMenu->addAction(zoomOutAction_);
    viewMenu->addAction(zoomFitAction_);
    viewMenu->addAction(zoomOriginalAction_);
    viewMenu->addSeparator();
    viewMenu->addAction(toggleDockAction_);
    viewMenu->addAction(toggleSideBySideAction_);
    
    // Processing menu
    QMenu *processMenu = menuBar()->addMenu(tr("&Processing"));
    
    QAction *processAction = new QAction(tr("&Generate Stencil"), this);
    processAction->setShortcut(Qt::Key_F5);
    connect(processAction, &QAction::triggered, this, &MainWindow::onProcessRequested);
    
    QAction *livePreviewAction = new QAction(tr("&Live Preview"), this);
    livePreviewAction->setCheckable(true);
    livePreviewAction->setChecked(livePreviewEnabled_);
    connect(livePreviewAction, &QAction::toggled, this, &MainWindow::onLivePreviewToggled);
    
    processMenu->addAction(processAction);
    processMenu->addAction(livePreviewAction);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    
    aboutAction_ = new QAction(tr("&About"), this);
    aboutQtAction_ = new QAction(tr("About &Qt"), this);
    
    helpMenu->addAction(aboutAction_);
    helpMenu->addAction(aboutQtAction_);
}

/**
 * @brief Setup main toolbar
 */
void MainWindow::setupToolBar() {
    mainToolBar_ = addToolBar(tr("Main Toolbar"));
    mainToolBar_->setMovable(false);
    
    mainToolBar_->addAction(openAction_);
    mainToolBar_->addAction(saveAction_);
    mainToolBar_->addSeparator();
    mainToolBar_->addAction(zoomInAction_);
    mainToolBar_->addAction(zoomOutAction_);
    mainToolBar_->addAction(zoomFitAction_);
    mainToolBar_->addAction(zoomOriginalAction_);
    mainToolBar_->addSeparator();
    
    // Add mode selector
    QLabel *modeLabel = new QLabel(tr("Mode:"), this);
    mainToolBar_->addWidget(modeLabel);
    
    QComboBox *modeCombo = new QComboBox(this);
    modeCombo->addItem(tr("Simple Threshold"), QVariant::fromValue(ProcessingMode::SIMPLE_THRESHOLD));
    modeCombo->addItem(tr("Edge Detection"), QVariant::fromValue(ProcessingMode::EDGE_DETECTION));
    modeCombo->addItem(tr("Adaptive Threshold"), QVariant::fromValue(ProcessingMode::ADAPTIVE_THRESHOLD));
    modeCombo->addItem(tr("Multi-layer"), QVariant::fromValue(ProcessingMode::MULTI_LAYER));
    modeCombo->addItem(tr("Polygon"), QVariant::fromValue(ProcessingMode::CONTOUR_POLYGON));
    modeCombo->addItem(tr("Detail Preserving"), QVariant::fromValue(ProcessingMode::DETAIL_PRESERVING));
    
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            processingWidget_, &ProcessingWidget::onModeChanged);
    
    mainToolBar_->addWidget(modeCombo);
}

/**
 * @brief Setup status bar with statistics
 */
void MainWindow::setupStatusBar() {
    statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);
    
    // Add permanent widgets
    statsLabel_ = new QLabel(this);
    statusBar_->addPermanentWidget(statsLabel_);
    
    updateStatusBar();
}

/**
 * @brief Setup signal/slot connections
 */
void MainWindow::setupConnections() {
    // File actions
    connect(openAction_, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(saveAction_, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(saveAsAction_, &QAction::triggered, this, &MainWindow::onSaveAsFile);
    connect(exitAction_, &QAction::triggered, this, &MainWindow::onExit);
    
    // View actions
    connect(zoomInAction_, &QAction::triggered, originalViewer_, &ImageViewer::zoomIn);
    connect(zoomInAction_, &QAction::triggered, processedViewer_, &ImageViewer::zoomIn);
    connect(zoomOutAction_, &QAction::triggered, originalViewer_, &ImageViewer::zoomOut);
    connect(zoomOutAction_, &QAction::triggered, processedViewer_, &ImageViewer::zoomOut);
    connect(zoomFitAction_, &QAction::triggered, originalViewer_, &ImageViewer::zoomFit);
    connect(zoomFitAction_, &QAction::triggered, processedViewer_, &ImageViewer::zoomFit);
    connect(zoomOriginalAction_, &QAction::triggered, originalViewer_, &ImageViewer::zoomOriginal);
    connect(zoomOriginalAction_, &QAction::triggered, processedViewer_, &ImageViewer::zoomOriginal);
    
    connect(toggleSideBySideAction_, &QAction::toggled, [this](bool checked) {
        if (checked) {
            mainSplitter_->setOrientation(Qt::Horizontal);
        } else {
            mainSplitter_->setOrientation(Qt::Vertical);
        }
    });
    
    // Processing connections
    connect(processingWidget_, &ProcessingWidget::paramsChanged,
            this, &MainWindow::onParamsChanged);
    connect(processingWidget_, &ProcessingWidget::processRequested,
            this, &MainWindow::onProcessRequested);
    
    // Stencil generator connections
    connect(stencilGenerator_, &StencilGenerator::processingStarted,
            this, &MainWindow::onProcessingStarted);
    connect(stencilGenerator_, &StencilGenerator::processingProgress,
            this, &MainWindow::onProcessingProgress);
    connect(stencilGenerator_, &StencilGenerator::processingCompleted,
            this, &MainWindow::onProcessingCompleted);
    connect(stencilGenerator_, &StencilGenerator::processingError,
            this, &MainWindow::onProcessingError);
    
    // Live preview timer
    connect(livePreviewTimer_, &QTimer::timeout,
            this, &MainWindow::onLivePreviewTimer);
    
    // Help actions
    connect(aboutAction_, &QAction::triggered, this, &MainWindow::onAbout);
    connect(aboutQtAction_, &QAction::triggered, qApp, &QApplication::aboutQt);
}

/**
 * @brief Open image file dialog
 */
void MainWindow::onOpenFile() {
    if (!confirmUnsavedChanges()) {
        return;
    }
    
    QStringList filters;
    filters << tr("Image files (*.png *.jpg *.jpeg *.bmp *.tif *.tiff *.webp)")
            << tr("All files (*)");
    
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Image"),
        saveDirectory_.isEmpty() ? QDir::homePath() : saveDirectory_,
        filters.join(";;"));
    
    if (filePath.isEmpty()) {
        return;
    }
    
    // Load the image
    if (stencilGenerator_->loadImage(filePath)) {
        currentFilePath_ = filePath;
        saveDirectory_ = QFileInfo(filePath).absolutePath();
        
        // Display original image
        originalViewer_->setImage(stencilGenerator_->getOriginalQImage());
        
        // Enable actions
        saveAction_->setEnabled(true);
        saveAsAction_->setEnabled(true);
        zoomInAction_->setEnabled(true);
        zoomOutAction_->setEnabled(true);
        zoomFitAction_->setEnabled(true);
        zoomOriginalAction_->setEnabled(true);
        
        // Clear processed viewer
        processedViewer_->clear();
        
        // Enable processing
        processingWidget_->setControlsEnabled(true);
        
        // Update window title
        updateWindowTitle();
        
        // Show image info
        showImageInfo(stencilGenerator_->getOriginalQImage());
        
        statusBar_->showMessage(tr("Image loaded: %1").arg(filePath), 3000);
    } else {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to load image:\n%1").arg(filePath));
    }
}

/**
 * @brief Process image with current parameters
 */
void MainWindow::processImage() {
    if (!stencilGenerator_->hasImage()) {
        QMessageBox::warning(this, tr("No Image"),
            tr("Please load an image first."));
        return;
    }
    
    StencilParams params = processingWidget_->getCurrentParams();
    StencilResult result = stencilGenerator_->generateStencil(params);
    
    if (result.success) {
        processedViewer_->setImage(result.fullResolutionImage);
        updateStatistics(result);
        statusBar_->showMessage(tr("Stencil generated successfully"), 3000);
    }
}

/**
 * @brief Handle parameter changes (live preview)
 */
void MainWindow::onParamsChanged(const StencilParams &params) {
    lastPreviewParams_ = params;
    
    if (livePreviewEnabled_ && stencilGenerator_->hasImage()) {
        // Restart the timer for live preview
        livePreviewTimer_->start();
    }
}

/**
 * @brief Update live preview (throttled)
 */
void MainWindow::onLivePreviewTimer() {
    if (!stencilGenerator_->hasImage()) {
        return;
    }
    
    StencilResult preview = stencilGenerator_->generateLivePreview(lastPreviewParams_);
    if (preview.success) {
        processedViewer_->setImage(preview.previewImage);
    }
}

/**
 * @brief Toggle live preview mode
 */
void MainWindow::onLivePreviewToggled(bool enabled) {
    livePreviewEnabled_ = enabled;
    
    if (enabled) {
        livePreviewTimer_->start();
        statusBar_->showMessage(tr("Live preview enabled"), 2000);
    } else {
        livePreviewTimer_->stop();
        statusBar_->showMessage(tr("Live preview disabled"), 2000);
    }
}

/**
 * @brief Save processed image
 */
void MainWindow::onSaveFile() {
    if (!processedViewer_->hasImage()) {
        QMessageBox::warning(this, tr("No Image"),
            tr("No processed image to save."));
        return;
    }
    
    QString defaultPath;
    if (!currentFilePath_.isEmpty()) {
        QFileInfo info(currentFilePath_);
        defaultPath = info.path() + "/" + info.baseName() + "_stencil.png";
    } else {
        defaultPath = saveDirectory_ + "/stencil.png";
    }
    
    QString filePath = QFileDialog::getSaveFileName(this,
        tr("Save Stencil"),
        defaultPath,
        tr("PNG files (*.png);;JPEG files (*.jpg *.jpeg);;BMP files (*.bmp)"));
    
    if (filePath.isEmpty()) {
        return;
    }
    
    if (processedViewer_->saveImage(filePath)) {
        statusBar_->showMessage(tr("Image saved: %1").arg(filePath), 3000);
        saveDirectory_ = QFileInfo(filePath).absolutePath();
    } else {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to save image:\n%1").arg(filePath));
    }
}

/**
 * @brief Update statistics display
 */
void MainWindow::updateStatistics(const StencilResult &result) {
    QString stats = QString("Black: %1 px | White: %2 px | Islands: %3 | Time: %4 ms")
        .arg(result.blackPixels)
        .arg(result.whitePixels)
        .arg(result.islandCount)
        .arg(result.processingTimeMs, 0, 'f', 1);
    
    statsLabel_->setText(stats);
}

/**
 * @brief Show image information dialog
 */
void MainWindow::showImageInfo(const QImage &image) {
    QString info = QString(
        "Dimensions: %1 x %2 pixels\n"
        "Format: %3\n"
        "Depth: %4 bits\n"
        "Size: %5 KB")
        .arg(image.width())
        .arg(image.height())
        .arg(image.format())
        .arg(image.depth())
        .arg(image.sizeInBytes() / 1024);
    
    QMessageBox::information(this, tr("Image Information"), info);
}