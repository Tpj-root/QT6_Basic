#include "ImageViewer.hpp"
#include <QPainter>
#include <QScrollBar>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

/**
 * @brief Constructor for ImageViewer
 * @param parent Parent widget
 */
ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
    , imageLabel_(new QLabel(this))
    , scrollArea_(new QScrollArea(this))
    , zoomFactor_(1.0)
    , panOffset_(0, 0)
    , isPanning_(false)
    , viewMode_(VIEW_FIT_WINDOW)
    , showGrid_(false)
    , showCrosshair_(false)
    , backgroundColor_(Qt::darkGray)
    , gridColor_(QColor(80, 80, 80))
    , rubberBand_(nullptr)
    , isSelecting_(false) {
    
    setupUI();
    setupContextMenu();
    setupConnections();
}

/**
 * @brief Destructor for ImageViewer
 */
ImageViewer::~ImageViewer() {
    delete contextMenu_;
}

/**
 * @brief Setup the user interface
 */
void ImageViewer::setupUI() {
    // Configure scroll area
    scrollArea_->setBackgroundRole(QPalette::Dark);
    scrollArea_->setAlignment(Qt::AlignCenter);
    scrollArea_->setWidget(imageLabel_);
    scrollArea_->setWidgetResizable(true);
    
    // Configure image label
    imageLabel_->setBackgroundRole(QPalette::Base);
    imageLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel_->setScaledContents(false);
    imageLabel_->setAlignment(Qt::AlignCenter);
    
    // Set up rubber band for selection
    rubberBand_ = new QRubberBand(QRubberBand::Rectangle, imageLabel_);
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea_);
    
    setLayout(mainLayout);
    
    // Set initial background
    QPalette pal = scrollArea_->palette();
    pal.setColor(QPalette::Window, backgroundColor_);
    scrollArea_->setPalette(pal);
}

/**
 * @brief Setup context menu
 */
void ImageViewer::setupContextMenu() {
    contextMenu_ = new QMenu(this);
    
    zoomInAction_ = new QAction(tr("Zoom In"), this);
    zoomInAction_->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction_, &QAction::triggered, this, &ImageViewer::zoomIn);
    
    zoomOutAction_ = new QAction(tr("Zoom Out"), this);
    zoomOutAction_->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction_, &QAction::triggered, this, &ImageViewer::zoomOut);
    
    zoomFitAction_ = new QAction(tr("Fit to Window"), this);
    zoomFitAction_->setShortcut(Qt::Key_F);
    connect(zoomFitAction_, &QAction::triggered, this, &ImageViewer::zoomFit);
    
    zoomOriginalAction_ = new QAction(tr("Original Size"), this);
    zoomOriginalAction_->setShortcut(Qt::Key_O);
    connect(zoomOriginalAction_, &QAction::triggered, this, &ImageViewer::zoomOriginal);
    
    contextMenu_->addAction(zoomInAction_);
    contextMenu_->addAction(zoomOutAction_);
    contextMenu_->addAction(zoomFitAction_);
    contextMenu_->addAction(zoomOriginalAction_);
    contextMenu_->addSeparator();
    
    copyAction_ = new QAction(tr("Copy Image"), this);
    copyAction_->setShortcut(QKeySequence::Copy);
    connect(copyAction_, &QAction::triggered, this, &ImageViewer::onCopyImage);
    
    saveAsAction_ = new QAction(tr("Save Image As..."), this);
    saveAsAction_->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction_, &QAction::triggered, this, &ImageViewer::onSaveImageAs);
    
    contextMenu_->addAction(copyAction_);
    contextMenu_->addAction(saveAsAction_);
}

/**
 * @brief Setup signal/slot connections
 */
void ImageViewer::setupConnections() {
    // Connect scroll area signals
    connect(scrollArea_->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &ImageViewer::updateViewport);
    connect(scrollArea_->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &ImageViewer::updateViewport);
    
    // Connect zoom factor changes
    connect(this, &ImageViewer::zoomChanged, this, &ImageViewer::adjustScrollBars);
}

/**
 * @brief Set image to display
 * @param image QImage to display
 */
void ImageViewer::setImage(const QImage &image) {
    if (image.isNull()) {
        clear();
        return;
    }
    
    currentImage_ = image;
    currentPixmap_ = QPixmap::fromImage(image);
    
    updatePixmap();
    updateView();
}

/**
 * @brief Set pixmap to display
 * @param pixmap QPixmap to display
 */
void ImageViewer::setPixmap(const QPixmap &pixmap) {
    if (pixmap.isNull()) {
        clear();
        return;
    }
    
    currentPixmap_ = pixmap;
    currentImage_ = pixmap.toImage();
    
    updatePixmap();
    updateView();
}

/**
 * @brief Clear the viewer
 */
void ImageViewer::clear() {
    currentImage_ = QImage();
    currentPixmap_ = QPixmap();
    imageLabel_->clear();
    zoomFactor_ = 1.0;
    panOffset_ = QPoint(0, 0);
    
    update();
}

/**
 * @brief Zoom in by 25%
 */
void ImageViewer::zoomIn() {
    setZoomFactor(zoomFactor_ * 1.25);
}

/**
 * @brief Zoom out by 25%
 */
void ImageViewer::zoomOut() {
    setZoomFactor(zoomFactor_ * 0.8);
}

/**
 * @brief Fit image to window
 */
void ImageViewer::zoomFit() {
    if (currentImage_.isNull()) {
        return;
    }
    
    int viewerWidth = scrollArea_->viewport()->width();
    int viewerHeight = scrollArea_->viewport()->height();
    int imageWidth = currentImage_.width();
    int imageHeight = currentImage_.height();
    
    if (viewerWidth <= 0 || viewerHeight <= 0 || imageWidth <= 0 || imageHeight <= 0) {
        return;
    }
    
    double widthRatio = static_cast<double>(viewerWidth) / imageWidth;
    double heightRatio = static_cast<double>(viewerHeight) / imageHeight;
    double scale = qMin(widthRatio, heightRatio);
    
    setZoomFactor(scale);
    viewMode_ = VIEW_FIT_WINDOW;
    
    adjustScrollBars();
}

/**
 * @brief Reset to original size (100% zoom)
 */
void ImageViewer::zoomOriginal() {
    setZoomFactor(1.0);
    viewMode_ = VIEW_NORMAL;
}

/**
 * @brief Set zoom factor
 * @param factor Zoom factor (1.0 = 100%)
 */
void ImageViewer::setZoomFactor(double factor) {
    factor = qMax(0.1, qMin(10.0, factor)); // Clamp between 10% and 1000%
    
    if (qFuzzyCompare(zoomFactor_, factor)) {
        return;
    }
    
    zoomFactor_ = factor;
    updatePixmap();
    emit zoomChanged(zoomFactor_);
}

/**
 * @brief Pan the view
 * @param delta Amount to pan
 */
void ImageViewer::pan(const QPoint &delta) {
    QScrollBar *hBar = scrollArea_->horizontalScrollBar();
    QScrollBar *vBar = scrollArea_->verticalScrollBar();
    
    hBar->setValue(hBar->value() + delta.x());
    vBar->setValue(vBar->value() + delta.y());
}

/**
 * @brief Center view on a point
 * @param pos Position to center on (image coordinates)
 */
void ImageViewer::centerOn(const QPoint &pos) {
    QPoint widgetPos = imageToWidget(pos);
    QScrollBar *hBar = scrollArea_->horizontalScrollBar();
    QScrollBar *vBar = scrollArea_->verticalScrollBar();
    
    int hValue = widgetPos.x() - scrollArea_->viewport()->width() / 2;
    int vValue = widgetPos.y() - scrollArea_->viewport()->height() / 2;
    
    hBar->setValue(hValue);
    vBar->setValue(vValue);
}

/**
 * @brief Set view mode
 * @param mode View mode
 */
void ImageViewer::setViewMode(ViewMode mode) {
    viewMode_ = mode;
    updateView();
}

/**
 * @brief Save image to file
 * @param filePath Path to save to
 * @param format Image format (nullptr for auto-detection)
 * @return true if saved successfully
 */
bool ImageViewer::saveImage(const QString &filePath, const char *format) {
    if (currentImage_.isNull()) {
        return false;
    }
    
    return currentImage_.save(filePath, format);
}

/**
 * @brief Paint event handler
 * @param event Paint event
 */
void ImageViewer::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    
    if (showGrid_ || showCrosshair_) {
        QPainter painter(imageLabel_);
        painter.setRenderHint(QPainter::Antialiasing);
        
        if (showGrid_) {
            drawGrid(painter);
        }
        
        if (showCrosshair_) {
            drawCrosshair(painter);
        }
    }
}

/**
 * @brief Resize event handler
 * @param event Resize event
 */
void ImageViewer::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    
    if (viewMode_ == VIEW_FIT_WINDOW && !currentImage_.isNull()) {
        zoomFit();
    } else if (viewMode_ == VIEW_FIT_WIDTH && !currentImage_.isNull()) {
        // Fit width
        int viewerWidth = scrollArea_->viewport()->width();
        double scale = static_cast<double>(viewerWidth) / currentImage_.width();
        setZoomFactor(scale);
    } else if (viewMode_ == VIEW_FIT_HEIGHT && !currentImage_.isNull()) {
        // Fit height
        int viewerHeight = scrollArea_->viewport()->height();
        double scale = static_cast<double>(viewerHeight) / currentImage_.height();
        setZoomFactor(scale);
    }
    
    adjustScrollBars();
}

/**
 * @brief Mouse press event handler
 * @param event Mouse event
 */
void ImageViewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)) {
        // Start panning
        isPanning_ = true;
        lastPanPos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton && event->modifiers() & Qt::ControlModifier) {
        // Start selection
        isSelecting_ = true;
        rubberBandOrigin_ = imageLabel_->mapFromParent(event->pos());
        rubberBand_->setGeometry(QRect(rubberBandOrigin_, QSize()));
        rubberBand_->show();
        event->accept();
    } else if (event->button() == Qt::LeftButton) {
        // Emit click signal
        QPoint imagePos = widgetToImage(event->pos());
        emit imageClicked(imagePos);
        event->accept();
    }
    
    QWidget::mousePressEvent(event);
}

/**
 * @brief Mouse move event handler
 * @param event Mouse event
 */
void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
    if (isPanning_) {
        QPoint delta = event->pos() - lastPanPos_;
        pan(delta);
        lastPanPos_ = event->pos();
        event->accept();
    } else if (isSelecting_) {
        QPoint currentPos = imageLabel_->mapFromParent(event->pos());
        QRect rect = QRect(rubberBandOrigin_, currentPos).normalized();
        rubberBand_->setGeometry(rect);
        event->accept();
    } else {
        // Update cursor
        if (event->buttons() == Qt::NoButton) {
            if (event->modifiers() & Qt::ShiftModifier) {
                setCursor(Qt::OpenHandCursor);
            } else if (event->modifiers() & Qt::ControlModifier) {
                setCursor(Qt::CrossCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
        
        // Emit hover position if needed
        QPoint imagePos = widgetToImage(event->pos());
        // You could emit a signal here for hover coordinates
    }
    
    QWidget::mouseMoveEvent(event);
}

/**
 * @brief Mouse release event handler
 * @param event Mouse event
 */
void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && isPanning_)) {
        // Stop panning
        isPanning_ = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton && isSelecting_) {
        // Finish selection
        isSelecting_ = false;
        rubberBand_->hide();
        
        QRect selection = rubberBand_->geometry();
        if (selection.width() > 5 && selection.height() > 5) {
            // Convert selection to image coordinates
            QPoint topLeft = widgetToImage(rubberBand_->geometry().topLeft());
            QPoint bottomRight = widgetToImage(rubberBand_->geometry().bottomRight());
            QRect imageSelection = QRect(topLeft, bottomRight);
            
            // You could emit a selection signal here
            // emit selectionChanged(imageSelection);
        }
        event->accept();
    } else if (event->button() == Qt::LeftButton) {
        // Regular click handled in press event
        event->accept();
    }
    
    QWidget::mouseReleaseEvent(event);
}

/**
 * @brief Mouse wheel event handler for zooming
 * @param event Wheel event
 */
void ImageViewer::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom with Ctrl+Wheel
        QPoint numDegrees = event->angleDelta() / 8;
        
        if (!numDegrees.isNull()) {
            if (numDegrees.y() > 0) {
                zoomIn();
            } else {
                zoomOut();
            }
        }
        event->accept();
    } else if (event->modifiers() & Qt::ShiftModifier) {
        // Horizontal scrolling with Shift+Wheel
        QScrollBar *hBar = scrollArea_->horizontalScrollBar();
        int delta = event->angleDelta().y();
        hBar->setValue(hBar->value() - delta);
        event->accept();
    } else {
        // Vertical scrolling (default)
        QWidget::wheelEvent(event);
    }
}

/**
 * @brief Context menu event handler
 * @param event Context menu event
 */
void ImageViewer::contextMenuEvent(QContextMenuEvent *event) {
    if (hasImage()) {
        contextMenu_->popup(event->globalPos());
        emit contextMenuRequested(event->pos());
    }
    
    event->accept();
}

/**
 * @brief Update the displayed pixmap with current zoom
 */
void ImageViewer::updatePixmap() {
    if (currentPixmap_.isNull()) {
        imageLabel_->clear();
        return;
    }
    
    QPixmap scaledPixmap;
    
    if (qFuzzyCompare(zoomFactor_, 1.0)) {
        scaledPixmap = currentPixmap_;
    } else {
        QSize scaledSize = currentPixmap_.size() * zoomFactor_;
        scaledPixmap = currentPixmap_.scaled(scaledSize, 
                                            Qt::KeepAspectRatio, 
                                            Qt::SmoothTransformation);
    }
    
    imageLabel_->setPixmap(scaledPixmap);
    imageLabel_->adjustSize();
    
    // Update scroll bars
    adjustScrollBars();
}

/**
 * @brief Convert widget coordinates to image coordinates
 * @param widgetPos Position in widget coordinates
 * @return Position in image coordinates
 */
QPoint ImageViewer::widgetToImage(const QPoint &widgetPos) const {
    if (currentImage_.isNull()) {
        return QPoint();
    }
    
    // Get position relative to image label
    QPoint labelPos = imageLabel_->mapFromParent(widgetPos);
    
    // Account for label alignment (centered)
    QRect labelRect = imageLabel_->rect();
    QSize pixmapSize = imageLabel_->pixmap(Qt::ReturnByValue).size();
    
    // Calculate offset if pixmap is smaller than label
    int xOffset = (labelRect.width() - pixmapSize.width()) / 2;
    int yOffset = (labelRect.height() - pixmapSize.height()) / 2;
    
    // Adjust position relative to pixmap
    QPoint pixmapPos = labelPos - QPoint(xOffset, yOffset);
    
    // Convert to image coordinates using zoom factor
    int imageX = qRound(pixmapPos.x() / zoomFactor_);
    int imageY = qRound(pixmapPos.y() / zoomFactor_);
    
    // Clamp to image bounds
    imageX = qBound(0, imageX, currentImage_.width() - 1);
    imageY = qBound(0, imageY, currentImage_.height() - 1);
    
    return QPoint(imageX, imageY);
}

/**
 * @brief Convert image coordinates to widget coordinates
 * @param imagePos Position in image coordinates
 * @return Position in widget coordinates
 */
QPoint ImageViewer::imageToWidget(const QPoint &imagePos) const {
    if (currentImage_.isNull()) {
        return QPoint();
    }
    
    // Convert from image to pixmap coordinates
    QPoint pixmapPos = QPoint(imagePos.x() * zoomFactor_, 
                             imagePos.y() * zoomFactor_);
    
    // Get label position
    QRect labelRect = imageLabel_->rect();
    QSize pixmapSize = imageLabel_->pixmap(Qt::ReturnByValue).size();
    
    // Calculate offset if pixmap is smaller than label
    int xOffset = (labelRect.width() - pixmapSize.width()) / 2;
    int yOffset = (labelRect.height() - pixmapSize.height()) / 2;
    
    // Convert to label coordinates
    QPoint labelPos = pixmapPos + QPoint(xOffset, yOffset);
    
    // Convert to widget coordinates
    return imageLabel_->mapToParent(labelPos);
}

/**
 * @brief Get image rectangle in widget coordinates
 * @return Rectangle of image in widget coordinates
 */
QRect ImageViewer::imageRect() const {
    if (currentImage_.isNull()) {
        return QRect();
    }
    
    QPoint topLeft = imageToWidget(QPoint(0, 0));
    QPoint bottomRight = imageToWidget(QPoint(currentImage_.width(), 
                                             currentImage_.height()));
    
    return QRect(topLeft, bottomRight);
}

/**
 * @brief Adjust scroll bars based on zoom and image size
 */
void ImageViewer::adjustScrollBars() {
    if (currentImage_.isNull()) {
        return;
    }
    
    QScrollBar *hBar = scrollArea_->horizontalScrollBar();
    QScrollBar *vBar = scrollArea_->verticalScrollBar();
    
    // Calculate maximum scroll values
    int hMax = qMax(0, imageLabel_->width() - scrollArea_->viewport()->width());
    int vMax = qMax(0, imageLabel_->height() - scrollArea_->viewport()->height());
    
    hBar->setRange(0, hMax);
    vBar->setRange(0, vMax);
    
    // Set page step to viewport size
    hBar->setPageStep(scrollArea_->viewport()->width());
    vBar->setPageStep(scrollArea_->viewport()->height());
    
    // Set single step to 10% of page step
    hBar->setSingleStep(scrollArea_->viewport()->width() / 10);
    vBar->setSingleStep(scrollArea_->viewport()->height() / 10);
}

/**
 * @brief Update view based on current settings
 */
void ImageViewer::updateView() {
    if (currentImage_.isNull()) {
        return;
    }
    
    switch (viewMode_) {
        case VIEW_FIT_WINDOW:
            zoomFit();
            break;
        case VIEW_FIT_WIDTH: {
            int viewerWidth = scrollArea_->viewport()->width();
            double scale = static_cast<double>(viewerWidth) / currentImage_.width();
            setZoomFactor(scale);
            break;
        }
        case VIEW_FIT_HEIGHT: {
            int viewerHeight = scrollArea_->viewport()->height();
            double scale = static_cast<double>(viewerHeight) / currentImage_.height();
            setZoomFactor(scale);
            break;
        }
        case VIEW_NORMAL:
            // Keep current zoom
            break;
    }
    
    updatePixmap();
}

/**
 * @brief Draw grid on image
 * @param painter QPainter to use
 */
void ImageViewer::drawGrid(QPainter &painter) {
    if (currentImage_.isNull() || zoomFactor_ < 0.5) {
        return; // Don't draw grid at low zoom levels
    }
    
    QRect viewport = scrollArea_->viewport()->rect();
    QPoint topLeft = widgetToImage(viewport.topLeft());
    QPoint bottomRight = widgetToImage(viewport.bottomRight());
    
    // Calculate grid spacing in image coordinates
    int gridSpacing = 50; // 50 pixels at 100% zoom
    
    // Adjust spacing for zoom level
    int effectiveSpacing = qMax(1, static_cast<int>(gridSpacing * zoomFactor_));
    
    if (effectiveSpacing < 10) {
        return; // Don't draw if grid would be too dense
    }
    
    // Convert back to widget coordinates for drawing
    int startX = qMax(0, topLeft.x() / gridSpacing) * gridSpacing;
    int startY = qMax(0, topLeft.y() / gridSpacing) * gridSpacing;
    int endX = bottomRight.x();
    int endY = bottomRight.y();
    
    painter.save();
    painter.setPen(QPen(gridColor_, 1, Qt::DotLine));
    
    // Draw vertical lines
    for (int x = startX; x <= endX; x += gridSpacing) {
        QPoint p1 = imageToWidget(QPoint(x, topLeft.y()));
        QPoint p2 = imageToWidget(QPoint(x, bottomRight.y()));
        painter.drawLine(p1, p2);
    }
    
    // Draw horizontal lines
    for (int y = startY; y <= endY; y += gridSpacing) {
        QPoint p1 = imageToWidget(QPoint(topLeft.x(), y));
        QPoint p2 = imageToWidget(QPoint(bottomRight.x(), y));
        painter.drawLine(p1, p2);
    }
    
    painter.restore();
}

/**
 * @brief Draw crosshair on image
 * @param painter QPainter to use
 */
void ImageViewer::drawCrosshair(QPainter &painter) {
    if (currentImage_.isNull()) {
        return;
    }
    
    QPoint center = widgetToImage(rect().center());
    QPoint widgetCenter = imageToWidget(center);
    
    painter.save();
    painter.setPen(QPen(Qt::red, 2));
    
    // Draw crosshair
    int size = 20;
    painter.drawLine(widgetCenter.x() - size, widgetCenter.y(), 
                     widgetCenter.x() + size, widgetCenter.y());
    painter.drawLine(widgetCenter.x(), widgetCenter.y() - size, 
                     widgetCenter.x(), widgetCenter.y() + size);
    
    // Draw circle around center
    painter.drawEllipse(widgetCenter, size / 2, size / 2);
    
    painter.restore();
}

/**
 * @brief Slot for copying image to clipboard
 */
void ImageViewer::onCopyImage() {
    if (hasImage()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setImage(currentImage_);
    }
}

/**
 * @brief Slot for saving image as file
 */
void ImageViewer::onSaveImageAs() {
    if (!hasImage()) {
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Image As"),
        QDir::homePath(),
        tr("PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp);;All Files (*)")
    );
    
    if (!filePath.isEmpty()) {
        QString format;
        if (filePath.endsWith(".png", Qt::CaseInsensitive)) {
            format = "PNG";
        } else if (filePath.endsWith(".jpg", Qt::CaseInsensitive) || 
                   filePath.endsWith(".jpeg", Qt::CaseInsensitive)) {
            format = "JPEG";
        } else if (filePath.endsWith(".bmp", Qt::CaseInsensitive)) {
            format = "BMP";
        }
        
        if (saveImage(filePath, format.toLatin1().constData())) {
            QMessageBox::information(this, tr("Success"), 
                                   tr("Image saved successfully."));
        } else {
            QMessageBox::critical(this, tr("Error"), 
                                tr("Failed to save image."));
        }
    }
}

/**
 * @brief Update viewport and emit signal
 */
void ImageViewer::updateViewport() {
    if (hasImage()) {
        QRect visibleRect = scrollArea_->viewport()->rect();
        visibleRect.translate(scrollArea_->horizontalScrollBar()->value(),
                             scrollArea_->verticalScrollBar()->value());
        
        // Convert to image coordinates
        QPoint topLeft = widgetToImage(visibleRect.topLeft());
        QPoint bottomRight = widgetToImage(visibleRect.bottomRight());
        QRect imageVisibleRect = QRect(topLeft, bottomRight);
        
        emit viewportChanged(imageVisibleRect);
    }
}