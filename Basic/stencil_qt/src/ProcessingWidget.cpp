#include "ProcessingWidget.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QApplication>
#include <QStyle>

/**
 * @brief Constructor for ProcessingWidget
 * @param parent Parent widget
 */
ProcessingWidget::ProcessingWidget(QWidget *parent)
    : QWidget(parent) {
    setupUI();
    setupConnections();
    
    // Initialize with default parameters
    resetToDefaults();
}

/**
 * @brief Destructor for ProcessingWidget
 */
ProcessingWidget::~ProcessingWidget() {
    // Save current settings
    saveCurrentPreset("LastSession");
}

/**
 * @brief Setup the user interface
 */
void ProcessingWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Create mode tabs
    modeTabs_ = new QTabWidget(this);
    modeTabs_->setTabPosition(QTabWidget::North);
    modeTabs_->setDocumentMode(true);
    
    // Create tabs for different processing modes
    createBasicTab();      // Simple threshold controls
    createEdgeTab();       // Edge detection controls
    createAdaptiveTab();   // Adaptive threshold controls
    createPolygonTab();    // Polygon simplification controls
    createMultiLayerTab(); // Multi-layer controls
    createOutputTab();     // Output settings
    
    mainLayout->addWidget(modeTabs_);
    
    // Create action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    processButton_ = new QPushButton(tr("Generate Stencil"), this);
    processButton_->setIcon(QIcon::fromTheme("media-playback-start"));
    processButton_->setMinimumHeight(35);
    
    saveButton_ = new QPushButton(tr("Save"), this);
    saveButton_->setIcon(QIcon::fromTheme("document-save"));
    saveButton_->setEnabled(false);
    
    resetButton_ = new QPushButton(tr("Reset"), this);
    resetButton_->setIcon(QIcon::fromTheme("edit-undo"));
    
    buttonLayout->addWidget(processButton_);
    buttonLayout->addWidget(saveButton_);
    buttonLayout->addWidget(resetButton_);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // Create preset controls
    QGroupBox *presetGroup = new QGroupBox(tr("Presets"), this);
    QHBoxLayout *presetLayout = new QHBoxLayout(presetGroup);
    
    presetCombo_ = new QComboBox(this);
    presetCombo_->setEditable(true);
    presetCombo_->addItem("Default");
    presetCombo_->addItem("High Contrast");
    presetCombo_->addItem("Soft Edges");
    presetCombo_->addItem("Wireframe");
    presetCombo_->addItem("Print Ready");
    
    savePresetButton_ = new QPushButton(tr("Save"), this);
    savePresetButton_->setIcon(QIcon::fromTheme("document-save-as"));
    
    loadPresetButton_ = new QPushButton(tr("Load"), this);
    loadPresetButton_->setIcon(QIcon::fromTheme("document-open"));
    
    presetLayout->addWidget(new QLabel(tr("Preset:"), this));
    presetLayout->addWidget(presetCombo_, 1);
    presetLayout->addWidget(savePresetButton_);
    presetLayout->addWidget(loadPresetButton_);
    
    mainLayout->addWidget(presetGroup);
    
    // Create progress bar
    progressBar_ = new QProgressBar(this);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setTextVisible(true);
    progressBar_->setFormat("%p%");
    progressBar_->setVisible(false);
    
    mainLayout->addWidget(progressBar_);
    
    // Add stretch to push everything up
    mainLayout->addStretch();
    
    setLayout(mainLayout);
}

/**
 * @brief Create basic threshold controls tab
 */
void ProcessingWidget::createBasicTab() {
    QWidget *basicTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(basicTab);
    
    // Threshold controls
    basicGroup_ = new QGroupBox(tr("Threshold Settings"), basicTab);
    QGridLayout *gridLayout = new QGridLayout(basicGroup_);
    
    // Threshold slider
    QLabel *threshLabel = new QLabel(tr("Threshold:"), basicGroup_);
    thresholdSlider_ = new QSlider(Qt::Horizontal, basicGroup_);
    thresholdSlider_->setRange(0, 255);
    thresholdSlider_->setValue(128);
    thresholdSlider_->setTickPosition(QSlider::TicksBelow);
    thresholdSlider_->setTickInterval(32);
    
    thresholdSpin_ = new QSpinBox(basicGroup_);
    thresholdSpin_->setRange(0, 255);
    thresholdSpin_->setValue(128);
    thresholdSpin_->setSuffix(" (0-255)");
    
    gridLayout->addWidget(threshLabel, 0, 0);
    gridLayout->addWidget(thresholdSlider_, 0, 1);
    gridLayout->addWidget(thresholdSpin_, 0, 2);
    
    // Contrast controls
    QLabel *contrastLabel = new QLabel(tr("Contrast:"), basicGroup_);
    contrastSlider_ = new QSlider(Qt::Horizontal, basicGroup_);
    contrastSlider_->setRange(50, 300);
    contrastSlider_->setValue(150);
    contrastSlider_->setTickPosition(QSlider::TicksBelow);
    contrastSlider_->setTickInterval(50);
    
    contrastSpin_ = new QDoubleSpinBox(basicGroup_);
    contrastSpin_->setRange(0.5, 3.0);
    contrastSpin_->setValue(1.5);
    contrastSpin_->setSingleStep(0.1);
    contrastSpin_->setDecimals(1);
    
    gridLayout->addWidget(contrastLabel, 1, 0);
    gridLayout->addWidget(contrastSlider_, 1, 1);
    gridLayout->addWidget(contrastSpin_, 1, 2);
    
    // Brightness controls
    QLabel *brightnessLabel = new QLabel(tr("Brightness:"), basicGroup_);
    brightnessSlider_ = new QSlider(Qt::Horizontal, basicGroup_);
    brightnessSlider_->setRange(-100, 100);
    brightnessSlider_->setValue(0);
    brightnessSlider_->setTickPosition(QSlider::TicksBelow);
    brightnessSlider_->setTickInterval(25);
    
    brightnessSpin_ = new QDoubleSpinBox(basicGroup_);
    brightnessSpin_->setRange(-1.0, 1.0);
    brightnessSpin_->setValue(0.0);
    brightnessSpin_->setSingleStep(0.1);
    brightnessSpin_->setDecimals(1);
    
    gridLayout->addWidget(brightnessLabel, 2, 0);
    gridLayout->addWidget(brightnessSlider_, 2, 1);
    gridLayout->addWidget(brightnessSpin_, 2, 2);
    
    // Blur controls
    QLabel *blurLabel = new QLabel(tr("Blur Radius:"), basicGroup_);
    blurSlider_ = new QSlider(Qt::Horizontal, basicGroup_);
    blurSlider_->setRange(0, 20);
    blurSlider_->setValue(2);
    blurSlider_->setTickPosition(QSlider::TicksBelow);
    blurSlider_->setTickInterval(5);
    
    blurSpin_ = new QSpinBox(basicGroup_);
    blurSpin_->setRange(0, 20);
    blurSpin_->setValue(2);
    blurSpin_->setSuffix(" px");
    
    gridLayout->addWidget(blurLabel, 3, 0);
    gridLayout->addWidget(blurSlider_, 3, 1);
    gridLayout->addWidget(blurSpin_, 3, 2);
    
    // Checkboxes
    invertCheck_ = new QCheckBox(tr("Invert Colors"), basicGroup_);
    preserveEdgesCheck_ = new QCheckBox(tr("Preserve Edges"), basicGroup_);
    preserveEdgesCheck_->setChecked(true);
    
    gridLayout->addWidget(invertCheck_, 4, 0, 1, 2);
    gridLayout->addWidget(preserveEdgesCheck_, 5, 0, 1, 2);
    
    basicGroup_->setLayout(gridLayout);
    layout->addWidget(basicGroup_);
    
    // Add information label
    QLabel *infoLabel = new QLabel(
        tr("<b>Simple Threshold:</b> Converts grayscale image to black and white "
           "using a single threshold value. Adjust threshold to control the amount "
           "of black vs white areas."),
        basicTab);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #2A2A2A; padding: 8px; border-radius: 4px; }");
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    modeTabs_->addTab(basicTab, tr("Basic"));
}

/**
 * @brief Create edge detection controls tab
 */
void ProcessingWidget::createEdgeTab() {
    QWidget *edgeTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(edgeTab);
    
    // Edge detection group
    edgeGroup_ = new QGroupBox(tr("Edge Detection Settings"), edgeTab);
    QGridLayout *gridLayout = new QGridLayout(edgeGroup_);
    
    // Low threshold
    QLabel *lowLabel = new QLabel(tr("Low Threshold:"), edgeGroup_);
    edgeLowSlider_ = new QSlider(Qt::Horizontal, edgeGroup_);
    edgeLowSlider_->setRange(0, 255);
    edgeLowSlider_->setValue(50);
    edgeLowSlider_->setTickPosition(QSlider::TicksBelow);
    edgeLowSlider_->setTickInterval(25);
    
    edgeLowSpin_ = new QSpinBox(edgeGroup_);
    edgeLowSpin_->setRange(0, 255);
    edgeLowSpin_->setValue(50);
    
    gridLayout->addWidget(lowLabel, 0, 0);
    gridLayout->addWidget(edgeLowSlider_, 0, 1);
    gridLayout->addWidget(edgeLowSpin_, 0, 2);
    
    // High threshold
    QLabel *highLabel = new QLabel(tr("High Threshold:"), edgeGroup_);
    edgeHighSlider_ = new QSlider(Qt::Horizontal, edgeGroup_);
    edgeHighSlider_->setRange(0, 255);
    edgeHighSlider_->setValue(150);
    edgeHighSlider_->setTickPosition(QSlider::TicksBelow);
    edgeHighSlider_->setTickInterval(25);
    
    edgeHighSpin_ = new QSpinBox(edgeGroup_);
    edgeHighSpin_->setRange(0, 255);
    edgeHighSpin_->setValue(150);
    
    gridLayout->addWidget(highLabel, 1, 0);
    gridLayout->addWidget(edgeHighSlider_, 1, 1);
    gridLayout->addWidget(edgeHighSpin_, 1, 2);
    
    // Kernel size
    QLabel *kernelLabel = new QLabel(tr("Kernel Size:"), edgeGroup_);
    edgeKernelSlider_ = new QSlider(Qt::Horizontal, edgeGroup_);
    edgeKernelSlider_->setRange(3, 7);
    edgeKernelSlider_->setValue(3);
    edgeKernelSlider_->setTickPosition(QSlider::TicksBelow);
    edgeKernelSlider_->setTickInterval(2);
    
    edgeKernelSpin_ = new QSpinBox(edgeGroup_);
    edgeKernelSpin_->setRange(3, 7);
    edgeKernelSpin_->setValue(3);
    edgeKernelSpin_->setSingleStep(2);
    
    gridLayout->addWidget(kernelLabel, 2, 0);
    gridLayout->addWidget(edgeKernelSlider_, 2, 1);
    gridLayout->addWidget(edgeKernelSpin_, 2, 2);
    
    edgeGroup_->setLayout(gridLayout);
    layout->addWidget(edgeGroup_);
    
    // Add information label
    QLabel *infoLabel = new QLabel(
        tr("<b>Edge Detection:</b> Detects edges in the image using the Canny "
           "edge detector. Creates a wireframe/stencil outline. Low threshold "
           "controls weak edges, high threshold controls strong edges."),
        edgeTab);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #2A2A2A; padding: 8px; border-radius: 4px; }");
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    modeTabs_->addTab(edgeTab, tr("Edge Detection"));
}

/**
 * @brief Create adaptive threshold controls tab
 */
void ProcessingWidget::createAdaptiveTab() {
    QWidget *adaptiveTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(adaptiveTab);
    
    // Adaptive threshold group
    adaptiveGroup_ = new QGroupBox(tr("Adaptive Threshold Settings"), adaptiveTab);
    QGridLayout *gridLayout = new QGridLayout(adaptiveGroup_);
    
    // Block size (must be odd)
    QLabel *blockLabel = new QLabel(tr("Block Size:"), adaptiveGroup_);
    adaptiveBlockSlider_ = new QSlider(Qt::Horizontal, adaptiveGroup_);
    adaptiveBlockSlider_->setRange(3, 31);
    adaptiveBlockSlider_->setValue(11);
    adaptiveBlockSlider_->setTickPosition(QSlider::TicksBelow);
    adaptiveBlockSlider_->setTickInterval(4);
    
    adaptiveBlockSpin_ = new QSpinBox(adaptiveGroup_);
    adaptiveBlockSpin_->setRange(3, 31);
    adaptiveBlockSpin_->setValue(11);
    adaptiveBlockSpin_->setSingleStep(2);
    adaptiveBlockSpin_->setSuffix(" (odd)");
    
    gridLayout->addWidget(blockLabel, 0, 0);
    gridLayout->addWidget(adaptiveBlockSlider_, 0, 1);
    gridLayout->addWidget(adaptiveBlockSpin_, 0, 2);
    
    // C constant
    QLabel *cLabel = new QLabel(tr("C Constant:"), adaptiveGroup_);
    adaptiveCSlider_ = new QSlider(Qt::Horizontal, adaptiveGroup_);
    adaptiveCSlider_->setRange(0, 20);
    adaptiveCSlider_->setValue(2);
    adaptiveCSlider_->setTickPosition(QSlider::TicksBelow);
    adaptiveCSlider_->setTickInterval(5);
    
    adaptiveCSpin_ = new QSpinBox(adaptiveGroup_);
    adaptiveCSpin_->setRange(0, 20);
    adaptiveCSpin_->setValue(2);
    
    gridLayout->addWidget(cLabel, 1, 0);
    gridLayout->addWidget(adaptiveCSlider_, 1, 1);
    gridLayout->addWidget(adaptiveCSpin_, 1, 2);
    
    adaptiveGroup_->setLayout(gridLayout);
    layout->addWidget(adaptiveGroup_);
    
    // Add information label
    QLabel *infoLabel = new QLabel(
        tr("<b>Adaptive Threshold:</b> Automatically adjusts threshold value for "
           "different regions of the image. Useful for images with uneven lighting "
           "or shadows. Block size determines neighborhood size, C constant "
           "subtracts from the mean."),
        adaptiveTab);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #2A2A2A; padding: 8px; border-radius: 4px; }");
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    modeTabs_->addTab(adaptiveTab, tr("Adaptive"));
}

/**
 * @brief Create polygon simplification controls tab
 */
void ProcessingWidget::createPolygonTab() {
    QWidget *polygonTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(polygonTab);
    
    // Polygon simplification group
    polygonGroup_ = new QGroupBox(tr("Polygon Settings"), polygonTab);
    QFormLayout *formLayout = new QFormLayout(polygonGroup_);
    
    // Epsilon parameter (controls simplification)
    polygonEpsilonSpin_ = new QDoubleSpinBox(polygonGroup_);
    polygonEpsilonSpin_->setRange(0.1, 20.0);
    polygonEpsilonSpin_->setValue(2.0);
    polygonEpsilonSpin_->setSingleStep(0.1);
    polygonEpsilonSpin_->setDecimals(1);
    polygonEpsilonSpin_->setSuffix(" px");
    formLayout->addRow(tr("Simplification:"), polygonEpsilonSpin_);
    
    // Minimum contour area
    minContourAreaSpin_ = new QSpinBox(polygonGroup_);
    minContourAreaSpin_->setRange(10, 1000);
    minContourAreaSpin_->setValue(100);
    minContourAreaSpin_->setSingleStep(10);
    minContourAreaSpin_->setSuffix(" px²");
    formLayout->addRow(tr("Min Contour Area:"), minContourAreaSpin_);
    
    polygonGroup_->setLayout(formLayout);
    layout->addWidget(polygonGroup_);
    
    // Add threshold controls for polygon mode
    QGroupBox *polyThresholdGroup = new QGroupBox(tr("Initial Threshold"), polygonTab);
    QGridLayout *threshLayout = new QGridLayout(polyThresholdGroup);
    
    QSlider *polyThreshSlider = new QSlider(Qt::Horizontal, polyThresholdGroup);
    polyThreshSlider->setRange(0, 255);
    polyThreshSlider->setValue(128);
    
    QSpinBox *polyThreshSpin = new QSpinBox(polyThresholdGroup);
    polyThreshSpin->setRange(0, 255);
    polyThreshSpin->setValue(128);
    
    threshLayout->addWidget(new QLabel(tr("Threshold:")), 0, 0);
    threshLayout->addWidget(polyThreshSlider, 0, 1);
    threshLayout->addWidget(polyThreshSpin, 0, 2);
    
    // Connect to main threshold controls
    connect(polyThreshSlider, &QSlider::valueChanged, thresholdSlider_, &QSlider::setValue);
    connect(polyThreshSpin, QOverload<int>::of(&QSpinBox::valueChanged), thresholdSpin_, &QSpinBox::setValue);
    connect(thresholdSlider_, &QSlider::valueChanged, polyThreshSlider, &QSlider::setValue);
    connect(thresholdSpin_, QOverload<int>::of(&QSpinBox::valueChanged), polyThreshSpin, &QSpinBox::setValue);
    
    polyThresholdGroup->setLayout(threshLayout);
    layout->addWidget(polyThresholdGroup);
    
    // Add information label
    QLabel *infoLabel = new QLabel(
        tr("<b>Polygon Simplification:</b> Converts image to vector-like polygons. "
           "Finds contours and simplifies them using the Douglas-Peucker algorithm. "
           "Useful for creating simplified, stylized stencils or for CNC cutting paths."),
        polygonTab);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #2A2A2A; padding: 8px; border-radius: 4px; }");
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    modeTabs_->addTab(polygonTab, tr("Polygon"));
}

/**
 * @brief Create multi-layer controls tab
 */
void ProcessingWidget::createMultiLayerTab() {
    QWidget *multiLayerTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(multiLayerTab);
    
    // Multi-layer group
    multiLayerGroup_ = new QGroupBox(tr("Multi-layer Settings"), multiLayerTab);
    QGridLayout *gridLayout = new QGridLayout(multiLayerGroup_);
    
    // Layer count
    QLabel *layerLabel = new QLabel(tr("Layer Count:"), multiLayerGroup_);
    layerCountSlider_ = new QSlider(Qt::Horizontal, multiLayerGroup_);
    layerCountSlider_->setRange(2, 8);
    layerCountSlider_->setValue(4);
    layerCountSlider_->setTickPosition(QSlider::TicksBelow);
    layerCountSlider_->setTickInterval(2);
    
    layerCountSpin_ = new QSpinBox(multiLayerGroup_);
    layerCountSpin_->setRange(2, 8);
    layerCountSpin_->setValue(4);
    
    gridLayout->addWidget(layerLabel, 0, 0);
    gridLayout->addWidget(layerCountSlider_, 0, 1);
    gridLayout->addWidget(layerCountSpin_, 0, 2);
    
    multiLayerGroup_->setLayout(gridLayout);
    layout->addWidget(multiLayerGroup_);
    
    // Add information label
    QLabel *infoLabel = new QLabel(
        tr("<b>Multi-layer Stencil:</b> Creates multiple gray levels for "
           "complex stencils with shading. Each layer represents a different "
           "depth or color intensity. Useful for painting stencils or "
           "multi-depth carvings."),
        multiLayerTab);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #2A2A2A; padding: 8px; border-radius: 4px; }");
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    modeTabs_->addTab(multiLayerTab, tr("Multi-layer"));
}

/**
 * @brief Create output settings tab
 */
void ProcessingWidget::createOutputTab() {
    QWidget *outputTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(outputTab);
    
    // Output settings group
    outputGroup_ = new QGroupBox(tr("Output Settings"), outputTab);
    QFormLayout *formLayout = new QFormLayout(outputGroup_);
    
    // Output width
    widthSpin_ = new QSpinBox(outputGroup_);
    widthSpin_->setRange(0, 10000);
    widthSpin_->setValue(0);
    widthSpin_->setSpecialValueText(tr("Original"));
    widthSpin_->setSuffix(" px");
    formLayout->addRow(tr("Width:"), widthSpin_);
    
    // Output height
    heightSpin_ = new QSpinBox(outputGroup_);
    heightSpin_->setRange(0, 10000);
    heightSpin_->setValue(0);
    heightSpin_->setSpecialValueText(tr("Original"));
    heightSpin_->setSuffix(" px");
    formLayout->addRow(tr("Height:"), heightSpin_);
    
    // Maintain aspect ratio
    maintainAspectCheck_ = new QCheckBox(tr("Maintain aspect ratio"), outputGroup_);
    maintainAspectCheck_->setChecked(true);
    formLayout->addRow("", maintainAspectCheck_);
    
    // Output format
    formatCombo_ = new QComboBox(outputGroup_);
    formatCombo_->addItem("PNG (Lossless)", "PNG");
    formatCombo_->addItem("JPEG (Compressed)", "JPEG");
    formatCombo_->addItem("BMP (Uncompressed)", "BMP");
    formLayout->addRow(tr("Format:"), formatCombo_);
    
    outputGroup_->setLayout(formLayout);
    layout->addWidget(outputGroup_);
    
    // Add DPI/PPI settings
    QGroupBox *resolutionGroup = new QGroupBox(tr("Print Resolution"), outputTab);
    QFormLayout *resLayout = new QFormLayout(resolutionGroup);
    
    QSpinBox *dpiSpin = new QSpinBox(resolutionGroup);
    dpiSpin->setRange(72, 1200);
    dpiSpin->setValue(300);
    dpiSpin->setSuffix(" DPI");
    resLayout->addRow(tr("Resolution:"), dpiSpin);
    
    QLabel *calcLabel = new QLabel(tr("Calculated size will appear here"), resolutionGroup);
    calcLabel->setWordWrap(true);
    resLayout->addRow(tr("Print Size:"), calcLabel);
    
    resolutionGroup->setLayout(resLayout);
    layout->addWidget(resolutionGroup);
    
    layout->addStretch();
    modeTabs_->addTab(outputTab, tr("Output"));
}

/**
 * @brief Setup signal/slot connections
 */
void ProcessingWidget::setupConnections() {
    // Connect sliders to spinboxes (two-way binding)
    connect(thresholdSlider_, &QSlider::valueChanged, thresholdSpin_, &QSpinBox::setValue);
    connect(thresholdSpin_, QOverload<int>::of(&QSpinBox::valueChanged), thresholdSlider_, &QSlider::setValue);
    
    connect(contrastSlider_, &QSlider::valueChanged, [this](int value) {
        contrastSpin_->setValue(value / 100.0);
    });
    connect(contrastSpin_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        contrastSlider_->setValue(static_cast<int>(value * 100));
    });
    
    connect(brightnessSlider_, &QSlider::valueChanged, [this](int value) {
        brightnessSpin_->setValue(value / 100.0);
    });
    connect(brightnessSpin_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        brightnessSlider_->setValue(static_cast<int>(value * 100));
    });
    
    connect(blurSlider_, &QSlider::valueChanged, blurSpin_, &QSpinBox::setValue);
    connect(blurSpin_, QOverload<int>::of(&QSpinBox::valueChanged), blurSlider_, &QSlider::setValue);
    
    // Edge detection controls
    connect(edgeLowSlider_, &QSlider::valueChanged, edgeLowSpin_, &QSpinBox::setValue);
    connect(edgeLowSpin_, QOverload<int>::of(&QSpinBox::valueChanged), edgeLowSlider_, &QSlider::setValue);
    
    connect(edgeHighSlider_, &QSlider::valueChanged, edgeHighSpin_, &QSpinBox::setValue);
    connect(edgeHighSpin_, QOverload<int>::of(&QSpinBox::valueChanged), edgeHighSlider_, &QSlider::setValue);
    
    connect(edgeKernelSlider_, &QSlider::valueChanged, edgeKernelSpin_, &QSpinBox::setValue);
    connect(edgeKernelSpin_, QOverload<int>::of(&QSpinBox::valueChanged), edgeKernelSlider_, &QSlider::setValue);
    
    // Adaptive threshold controls
    connect(adaptiveBlockSlider_, &QSlider::valueChanged, adaptiveBlockSpin_, &QSpinBox::setValue);
    connect(adaptiveBlockSpin_, QOverload<int>::of(&QSpinBox::valueChanged), adaptiveBlockSlider_, &QSlider::setValue);
    
    connect(adaptiveCSlider_, &QSlider::valueChanged, adaptiveCSpin_, &QSpinBox::setValue);
    connect(adaptiveCSpin_, QOverload<int>::of(&QSpinBox::valueChanged), adaptiveCSlider_, &QSlider::setValue);
    
    // Multi-layer controls
    connect(layerCountSlider_, &QSlider::valueChanged, layerCountSpin_, &QSpinBox::setValue);
    connect(layerCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged), layerCountSlider_, &QSlider::setValue);
    
    // Connect value changes to parameter updates
    connect(thresholdSlider_, &QSlider::valueChanged, this, &ProcessingWidget::onThresholdChanged);
    connect(contrastSlider_, &QSlider::valueChanged, this, &ProcessingWidget::onContrastChanged);
    connect(brightnessSlider_, &QSlider::valueChanged, this, &ProcessingWidget::onBrightnessChanged);
    connect(blurSlider_, &QSlider::valueChanged, this, &ProcessingWidget::onBlurChanged);
    
    connect(invertCheck_, &QCheckBox::toggled, this, &ProcessingWidget::onInvertToggled);
    
    // Connect mode tab changes
    connect(modeTabs_, &QTabWidget::currentChanged, this, &ProcessingWidget::onModeChanged);
    
    // Connect action buttons
    connect(processButton_, &QPushButton::clicked, this, &ProcessingWidget::onProcessClicked);
    connect(saveButton_, &QPushButton::clicked, this, &ProcessingWidget::onSaveClicked);
    connect(resetButton_, &QPushButton::clicked, this, &ProcessingWidget::onResetClicked);
    
    // Connect preset buttons
    connect(savePresetButton_, &QPushButton::clicked, this, &ProcessingWidget::onSavePresetClicked);
    connect(loadPresetButton_, &QPushButton::clicked, this, &ProcessingWidget::onLoadPresetClicked);
    
    // Connect preset combo
    connect(presetCombo_, &QComboBox::currentTextChanged, [this](const QString &text) {
        if (!text.isEmpty() && text != currentPresetName_) {
            loadPreset(text);
        }
    });
}

/**
 * @brief Get current processing parameters from UI
 * @return StencilParams structure
 */
StencilParams ProcessingWidget::getCurrentParams() const {
    updateParamsFromUI();
    return currentParams_;
}

/**
 * @brief Set parameters and update UI
 * @param params Parameters to set
 */
void ProcessingWidget::setParams(const StencilParams &params) {
    currentParams_ = params;
    updateUIFromParams();
}

/**
 * @brief Reset all controls to default values
 */
void ProcessingWidget::resetToDefaults() {
    currentParams_ = StencilParams(); // Use default constructor
    updateUIFromParams();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Save current settings as a preset
 * @param name Preset name
 */
void ProcessingWidget::savePreset(const QString &name) {
    if (name.isEmpty()) {
        return;
    }
    
    updateParamsFromUI();
    
    // Add to combo box if not already there
    if (presetCombo_->findText(name) == -1) {
        presetCombo_->addItem(name);
    }
    
    presetCombo_->setCurrentText(name);
    currentPresetName_ = name;
    
    // Save to file
    savePresetToFile(name, currentParams_);
    
    emit savePresetRequested(name);
}

/**
 * @brief Load preset by name
 * @param name Preset name
 * @return true if loaded successfully
 */
bool ProcessingWidget::loadPreset(const QString &name) {
    StencilParams params = loadPresetFromFile(name);
    if (params.mode != ProcessingMode::SIMPLE_THRESHOLD && 
        static_cast<int>(params.mode) == 0) {
        // Failed to load
        return false;
    }
    
    setParams(params);
    currentPresetName_ = name;
    presetCombo_->setCurrentText(name);
    
    emit loadPresetRequested(name);
    return true;
}

/**
 * @brief Delete a preset
 * @param name Preset name
 */
void ProcessingWidget::deletePreset(const QString &name) {
    if (name == "Default") {
        return; // Don't delete default
    }
    
    int index = presetCombo_->findText(name);
    if (index != -1) {
        presetCombo_->removeItem(index);
    }
    
    deletePresetFile(name);
}

/**
 * @brief Get list of preset names
 * @return QStringList of preset names
 */
QStringList ProcessingWidget::getPresetNames() const {
    QStringList names;
    for (int i = 0; i < presetCombo_->count(); i++) {
        names.append(presetCombo_->itemText(i));
    }
    return names;
}

/**
 * @brief Enable or disable all controls
 * @param enabled True to enable, false to disable
 */
void ProcessingWidget::setControlsEnabled(bool enabled) {
    processButton_->setEnabled(enabled);
    saveButton_->setEnabled(enabled && enabled); // Only enable save if we have a processed image
    resetButton_->setEnabled(enabled);
    
    // Enable/disable all control groups
    QList<QGroupBox*> groups = findChildren<QGroupBox*>();
    for (QGroupBox *group : groups) {
        group->setEnabled(enabled);
    }
    
    // Enable/disable mode tabs
    modeTabs_->setEnabled(enabled);
}

/**
 * @brief Update parameters from UI controls
 */
void ProcessingWidget::updateParamsFromUI() {
    // Set mode based on current tab
    int tabIndex = modeTabs_->currentIndex();
    switch (tabIndex) {
        case 0: currentParams_.mode = ProcessingMode::SIMPLE_THRESHOLD; break;
        case 1: currentParams_.mode = ProcessingMode::EDGE_DETECTION; break;
        case 2: currentParams_.mode = ProcessingMode::ADAPTIVE_THRESHOLD; break;
        case 3: currentParams_.mode = ProcessingMode::CONTOUR_POLYGON; break;
        case 4: currentParams_.mode = ProcessingMode::MULTI_LAYER; break;
        case 5: currentParams_.mode = ProcessingMode::DETAIL_PRESERVING; break;
    }
    
    // Basic parameters
    currentParams_.threshold = thresholdSlider_->value();
    currentParams_.contrast = contrastSpin_->value();
    currentParams_.brightness = brightnessSpin_->value();
    currentParams_.blurRadius = blurSlider_->value();
    currentParams_.invertColors = invertCheck_->isChecked();
    currentParams_.preserveEdges = preserveEdgesCheck_->isChecked();
    
    // Edge detection parameters
    currentParams_.edgeLowThreshold = edgeLowSlider_->value();
    currentParams_.edgeHighThreshold = edgeHighSlider_->value();
    currentParams_.edgeKernelSize = edgeKernelSlider_->value();
    
    // Adaptive threshold parameters
    currentParams_.adaptiveBlockSize = adaptiveBlockSlider_->value();
    currentParams_.adaptiveC = adaptiveCSpin_->value();
    
    // Polygon parameters
    currentParams_.polygonEpsilon = polygonEpsilonSpin_->value();
    currentParams_.minContourArea = minContourAreaSpin_->value();
    
    // Multi-layer parameters
    currentParams_.layerCount = layerCountSlider_->value();
    
    // Output parameters
    currentParams_.outputWidth = widthSpin_->value();
    currentParams_.outputHeight = heightSpin_->value();
    currentParams_.maintainAspectRatio = maintainAspectCheck_->isChecked();
}

/**
 * @brief Update UI controls from current parameters
 */
void ProcessingWidget::updateUIFromParams() {
    // Set mode tab
    int tabIndex = 0;
    switch (currentParams_.mode) {
        case ProcessingMode::SIMPLE_THRESHOLD: tabIndex = 0; break;
        case ProcessingMode::EDGE_DETECTION: tabIndex = 1; break;
        case ProcessingMode::ADAPTIVE_THRESHOLD: tabIndex = 2; break;
        case ProcessingMode::CONTOUR_POLYGON: tabIndex = 3; break;
        case ProcessingMode::MULTI_LAYER: tabIndex = 4; break;
        case ProcessingMode::DETAIL_PRESERVING: tabIndex = 0; break; // Default to basic
    }
    modeTabs_->setCurrentIndex(tabIndex);
    
    // Basic parameters
    thresholdSlider_->setValue(currentParams_.threshold);
    contrastSpin_->setValue(currentParams_.contrast);
    brightnessSpin_->setValue(currentParams_.brightness);
    blurSlider_->setValue(currentParams_.blurRadius);
    invertCheck_->setChecked(currentParams_.invertColors);
    preserveEdgesCheck_->setChecked(currentParams_.preserveEdges);
    
    // Edge detection parameters
    edgeLowSlider_->setValue(currentParams_.edgeLowThreshold);
    edgeHighSlider_->setValue(currentParams_.edgeHighThreshold);
    edgeKernelSlider_->setValue(currentParams_.edgeKernelSize);
    
    // Adaptive threshold parameters
    adaptiveBlockSlider_->setValue(currentParams_.adaptiveBlockSize);
    adaptiveCSpin_->setValue(currentParams_.adaptiveC);
    
    // Polygon parameters
    polygonEpsilonSpin_->setValue(currentParams_.polygonEpsilon);
    minContourAreaSpin_->setValue(currentParams_.minContourArea);
    
    // Multi-layer parameters
    layerCountSlider_->setValue(currentParams_.layerCount);
    
    // Output parameters
    widthSpin_->setValue(currentParams_.outputWidth);
    heightSpin_->setValue(currentParams_.outputHeight);
    maintainAspectCheck_->setChecked(currentParams_.maintainAspectRatio);
}

/**
 * @brief Slot for processing mode change
 * @param index Tab index
 */
void ProcessingWidget::onModeChanged(int index) {
    Q_UNUSED(index);
    updateParamsFromUI();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Slot for threshold value change
 * @param value New threshold value
 */
void ProcessingWidget::onThresholdChanged(int value) {
    Q_UNUSED(value);
    updateParamsFromUI();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Slot for contrast value change
 * @param value New contrast value
 */
void ProcessingWidget::onContrastChanged(double value) {
    Q_UNUSED(value);
    updateParamsFromUI();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Slot for brightness value change
 * @param value New brightness value
 */
void ProcessingWidget::onBrightnessChanged(double value) {
    Q_UNUSED(value);
    updateParamsFromUI();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Slot for blur value change
 * @param value New blur value
 */
void ProcessingWidget::onBlurChanged(int value) {
    Q_UNUSED(value);
    updateParamsFromUI();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Slot for invert colors toggle
 * @param checked True if inverted
 */
void ProcessingWidget::onInvertToggled(bool checked) {
    Q_UNUSED(checked);
    updateParamsFromUI();
    emit paramsChanged(currentParams_);
}

/**
 * @brief Slot for process button click
 */
void ProcessingWidget::onProcessClicked() {
    updateParamsFromUI();
    emit processRequested();
}

/**
 * @brief Slot for save button click
 */
void ProcessingWidget::onSaveClicked() {
    emit saveRequested();
}

/**
 * @brief Slot for reset button click
 */
void ProcessingWidget::onResetClicked() {
    resetToDefaults();
}

/**
 * @brief Slot for save preset button click
 */
void ProcessingWidget::onSavePresetClicked() {
    QString name = presetCombo_->currentText();
    if (name.isEmpty()) {
        name = QInputDialog::getText(this, tr("Save Preset"), 
                                     tr("Enter preset name:"));
    }
    
    if (!name.isEmpty()) {
        savePreset(name);
    }
}

/**
 * @brief Slot for load preset button click
 */
void ProcessingWidget::onLoadPresetClicked() {
    QString name = presetCombo_->currentText();
    if (!name.isEmpty()) {
        loadPreset(name);
    }
}

/**
 * @brief Slot for processing started signal
 */
void ProcessingWidget::onProcessingStarted() {
    progressBar_->setValue(0);
    progressBar_->setVisible(true);
    setControlsEnabled(false);
}

/**
 * @brief Slot for processing progress signal
 * @param percent Progress percentage
 */
void ProcessingWidget::onProcessingProgress(int percent) {
    progressBar_->setValue(percent);
}

/**
 * @brief Slot for processing completed signal
 */
void ProcessingWidget::onProcessingCompleted() {
    progressBar_->setValue(100);
    progressBar_->setVisible(false);
    setControlsEnabled(true);
    saveButton_->setEnabled(true);
}

/**
 * @brief Slot for processing error signal
 * @param error Error message
 */
void ProcessingWidget::onProcessingError(const QString &error) {
    Q_UNUSED(error);
    progressBar_->setVisible(false);
    setControlsEnabled(true);
    saveButton_->setEnabled(false);
}

/**
 * @brief Save preset to file
 * @param name Preset name
 * @param params Parameters to save
 */
void ProcessingWidget::savePresetToFile(const QString &name, const StencilParams &params) {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    
    QString filePath = configDir + "/" + name + ".json";
    
    QJsonObject json;
    json["name"] = name;
    json["mode"] = static_cast<int>(params.mode);
    json["threshold"] = params.threshold;
    json["contrast"] = params.contrast;
    json["brightness"] = params.brightness;
    json["blurRadius"] = params.blurRadius;
    json["invertColors"] = params.invertColors;
    json["preserveEdges"] = params.preserveEdges;
    json["edgeLowThreshold"] = params.edgeLowThreshold;
    json["edgeHighThreshold"] = params.edgeHighThreshold;
    json["edgeKernelSize"] = params.edgeKernelSize;
    json["adaptiveBlockSize"] = params.adaptiveBlockSize;
    json["adaptiveC"] = params.adaptiveC;
    json["polygonEpsilon"] = params.polygonEpsilon;
    json["minContourArea"] = params.minContourArea;
    json["layerCount"] = params.layerCount;
    json["outputWidth"] = params.outputWidth;
    json["outputHeight"] = params.outputHeight;
    json["maintainAspectRatio"] = params.maintainAspectRatio;
    
    QJsonDocument doc(json);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

/**
 * @brief Load preset from file
 * @param name Preset name
 * @return Loaded parameters
 */
StencilParams ProcessingWidget::loadPresetFromFile(const QString &name) {
    StencilParams params;
    
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString filePath = configDir + "/" + name + ".json";
    
    QFile file(filePath);
    if (!file.exists()) {
        return params; // Return default
    }
    
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject json = doc.object();
            
            params.mode = static_cast<ProcessingMode>(json["mode"].toInt());
            params.threshold = json["threshold"].toInt();
            params.contrast = json["contrast"].toDouble();
            params.brightness = json["brightness"].toDouble();
            params.blurRadius = json["blurRadius"].toInt();
            params.invertColors = json["invertColors"].toBool();
            params.preserveEdges = json["preserveEdges"].toBool();
            params.edgeLowThreshold = json["edgeLowThreshold"].toInt();
            params.edgeHighThreshold = json["edgeHighThreshold"].toInt();
            params.edgeKernelSize = json["edgeKernelSize"].toInt();
            params.adaptiveBlockSize = json["adaptiveBlockSize"].toInt();
            params.adaptiveC = json["adaptiveC"].toInt();
            params.polygonEpsilon = json["polygonEpsilon"].toDouble();
            params.minContourArea = json["minContourArea"].toInt();
            params.layerCount = json["layerCount"].toInt();
            params.outputWidth = json["outputWidth"].toInt();
            params.outputHeight = json["outputHeight"].toInt();
            params.maintainAspectRatio = json["maintainAspectRatio"].toBool();
        }
    }
    
    return params;
}

/**
 * @brief Delete preset file
 * @param name Preset name
 */
void ProcessingWidget::deletePresetFile(const QString &name) {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString filePath = configDir + "/" + name + ".json";
    
    QFile file(filePath);
    if (file.exists()) {
        file.remove();
    }
}

/**
 * @brief Save current settings as preset
 * @param name Preset name
 */
void ProcessingWidget::saveCurrentPreset(const QString &name) {
    updateParamsFromUI();
    savePresetToFile(name, currentParams_);
}