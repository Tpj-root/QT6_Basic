#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QRubberBand>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QSlider>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

/**
 * @brief Image viewer widget with zoom and pan capabilities
 */
class ImageViewer : public QWidget {
    Q_OBJECT
    
public:
    explicit ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();
    
    // Image display
    void setImage(const QImage &image);
    void setPixmap(const QPixmap &pixmap);
    void clear();
    
    // Zoom control
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomOriginal();
    void setZoomFactor(double factor);
    double getZoomFactor() const { return zoomFactor_; }
    
    // Navigation
    void pan(const QPoint &delta);
    void centerOn(const QPoint &pos);
    
    // View modes
    enum ViewMode {
        VIEW_NORMAL,
        VIEW_FIT_WIDTH,
        VIEW_FIT_HEIGHT,
        VIEW_FIT_WINDOW
    };
    
    void setViewMode(ViewMode mode);
    
    // Getters
    QImage getImage() const { return currentImage_; }
    QPixmap getPixmap() const { return currentPixmap_; }
    bool hasImage() const { return !currentImage_.isNull(); }
    
    // Export
    bool saveImage(const QString &filePath, const char *format = nullptr);
    
signals:
    void zoomChanged(double factor);
    void imageClicked(const QPoint &imagePos);
    void imageDoubleClicked(const QPoint &imagePos);
    void viewportChanged(const QRect &visibleRect);
    void contextMenuRequested(const QPoint &pos);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    
private:
    // UI Components
    QLabel *imageLabel_;
    QScrollArea *scrollArea_;
    
    // Image data
    QImage currentImage_;
    QPixmap currentPixmap_;
    
    // Zoom and pan
    double zoomFactor_;
    QPoint panOffset_;
    QPoint lastPanPos_;
    bool isPanning_;
    
    // View settings
    ViewMode viewMode_;
    bool showGrid_;
    bool showCrosshair_;
    QColor backgroundColor_;
    QColor gridColor_;
    
    // Tools
    QRubberBand *rubberBand_;
    QPoint rubberBandOrigin_;
    bool isSelecting_;
    
    // Context menu
    QMenu *contextMenu_;
    QAction *zoomInAction_;
    QAction *zoomOutAction_;
    QAction *zoomFitAction_;
    QAction *zoomOriginalAction_;
    QAction *copyAction_;
    QAction *saveAsAction_;
    
    // Initialization
    void setupUI();
    void setupContextMenu();
    void setupConnections();
    
    // Helper functions
    void updatePixmap();
    QPoint widgetToImage(const QPoint &widgetPos) const;
    QPoint imageToWidget(const QPoint &imagePos) const;
    QRect imageRect() const;
    void adjustScrollBars();
    void updateView();
    
    // Drawing helpers
    void drawGrid(QPainter &painter);
    void drawCrosshair(QPainter &painter);
    
private slots:
    void onCopyImage();
    void onSaveImageAs();
};

#endif // IMAGEVIEWER_HPP