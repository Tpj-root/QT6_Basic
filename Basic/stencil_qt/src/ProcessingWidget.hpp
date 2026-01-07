#ifndef PROCESSINGWIDGET_HPP
#define PROCESSINGWIDGET_HPP

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTabWidget>
#include <QStackedWidget>

#include "StencilGenerator.hpp"

/**
 * @brief Widget for controlling stencil processing parameters
 */
class ProcessingWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit ProcessingWidget(QWidget *parent = nullptr);
    ~ProcessingWidget();
    
    // Get current parameters
    StencilParams getCurrentParams() const;
    
    // Set parameters
    void setParams(const StencilParams &params);
    
    // Reset to defaults
    void resetToDefaults();
    
    // Preset management
    void savePreset(const QString &name);
    bool loadPreset(const QString &name);
    void deletePreset(const QString &name);
    QStringList getPresetNames() const;
    
    // Enable/disable controls
    void setControlsEnabled(bool enabled);
    
signals:
    void paramsChanged(const StencilParams &params);
    void processRequested();
    void saveRequested();
    void loadPresetRequested(const QString &name);
    void savePresetRequested(const QString &name);
    
public slots:
    void onProcessingStarted();
    void onProcessingProgress(int percent);
    void onProcessingCompleted();
    void onProcessingError(const QString &error);
    
private:
    // UI Components organized by processing mode
    QTabWidget *modeTabs_;
    
    // Common controls
    QGroupBox *basicGroup_;
    QSlider *thresholdSlider_;
    QSpinBox *thresholdSpin_;
    QSlider *contrastSlider_;
    QDoubleSpinBox *contrastSpin_;
    QSlider *brightnessSlider_;
    QDoubleSpinBox *brightnessSpin_;
    QSlider *blurSlider_;
    QSpinBox *blurSpin_;
    QCheckBox *invertCheck_;
    QCheckBox *preserveEdgesCheck_;
    
    // Edge detection controls
    QGroupBox *edgeGroup_;
    QSlider *edgeLowSlider_;
    QSpinBox *edgeLowSpin_;
    QSlider *edgeHighSlider_;
    QSpinBox *edgeHighSpin_;
    QSlider *edgeKernelSlider_;
    QSpinBox *edgeKernelSpin_;
    
    // Adaptive threshold controls
    QGroupBox *adaptiveGroup_;
    QSlider *adaptiveBlockSlider_;
    QSpinBox *adaptiveBlockSpin_;
    QSlider *adaptiveCSlider_;
    QSpinBox *adaptiveCSpin_;
    
    // Polygon controls
    QGroupBox *polygonGroup_;
    QDoubleSpinBox *polygonEpsilonSpin_;
    QSpinBox *minContourAreaSpin_;
    
    // Multi-layer controls
    QGroupBox *multiLayerGroup_;
    QSlider *layerCountSlider_;
    QSpinBox *layerCountSpin_;
    
    // Output controls
    QGroupBox *outputGroup_;
    QSpinBox *widthSpin_;
    QSpinBox *heightSpin_;
    QCheckBox *maintainAspectCheck_;
    QComboBox *formatCombo_;
    
    // Action buttons
    QPushButton *processButton_;
    QPushButton *saveButton_;
    QPushButton *resetButton_;
    QPushButton *savePresetButton_;
    QPushButton *loadPresetButton_;
    
    // Progress
    QProgressBar *progressBar_;
    
    // Preset management
    QComboBox *presetCombo_;
    
    // Current parameters
    StencilParams currentParams_;
    
    // Initialization
    void setupUI();
    void setupConnections();
    void createBasicTab();
    void createEdgeTab();
    void createAdaptiveTab();
    void createPolygonTab();
    void createMultiLayerTab();
    void createOutputTab();
    
    // Helper functions
    void updateParamsFromUI();
    void updateUIFromParams();
    void showModeSpecificControls(ProcessingMode mode);
    
private slots:
    void onModeChanged(int index);
    void onThresholdChanged(int value);
    void onContrastChanged(double value);
    void onBrightnessChanged(double value);
    void onBlurChanged(int value);
    void onInvertToggled(bool checked);
    void onProcessClicked();
    void onSaveClicked();
    void onResetClicked();
    void onSavePresetClicked();
    void onLoadPresetClicked();
};

#endif // PROCESSINGWIDGET_HPP