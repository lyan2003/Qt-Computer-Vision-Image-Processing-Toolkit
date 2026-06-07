#include "mainwindow.h"
#include "utils.h" // Shared utilities for image display
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <cmath>

// ==========================================
// Constructor & UI Initialization
// ==========================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Initialize the central widget to hold all UI elements
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // --- Global UI Stylesheet ---
    // Applying CSS to ensure a modern, consistent dark-mode look across the application.
    QString inputsStyle =
        "QSpinBox, QDoubleSpinBox, QComboBox { padding: 4px; border: 1px solid #9b11ff; border-radius: 4px; background-color: #2b2b2e; color: white; }"
        "QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus { border: 1px solid #cd66ff; }"
        "QSpinBox::up-button, QDoubleSpinBox::up-button, QSpinBox::down-button, QDoubleSpinBox::down-button { width: 0px; border: none; }"

        // Custom styling for QComboBox to display the dropdown arrow correctly
        "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 25px; border-left: 1px solid #9b11ff; border-top-right-radius: 3px; border-bottom-right-radius: 3px; background: transparent; }"
        "QComboBox::down-arrow { width: 0px; height: 0px; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 6px solid white; margin-top: 2px; }"
        "QComboBox::down-arrow:on { border-top: 6px solid #cd66ff; }"
        "QComboBox QAbstractItemView { background-color: #2b2b2e; color: white; selection-background-color: #9b11ff; border: 1px solid #9b11ff; outline: none; }"

        // Custom styling for QSlider to match the theme
        "QSlider::groove:horizontal { border: 1px solid #555; height: 6px; background: #2b2b2e; border-radius: 3px; }"
        "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #cd66ff, stop:1 #9b11ff); border-radius: 3px; }"
        "QSlider::handle:horizontal { background: white; border: 2px solid #7a00cc; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }";

    QString applyBtnStyle =
        "QPushButton { padding: 4px 12px; background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #b026ff, stop:1 #7a00cc); color: white; border: none; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #cd66ff, stop:1 #9b11ff); }";

    centralWidget->setStyleSheet(inputsStyle);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0); mainLayout->setSpacing(0);

    // --- Sidebar Setup ---
    // Creating a dedicated sidebar for primary actions (Load, Undo, Reset)
    QFrame *sidebar = new QFrame();
    sidebar->setFixedWidth(80);
    sidebar->setStyleSheet("QFrame { background-color: #1e1e21; border-right: 1px solid #444; }");
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 20, 15, 20); sidebarLayout->setSpacing(20);

    QString iconBtnStyle = "QPushButton { background: #b026ff; color: white; border-radius: 25px; font-size: 22px; } QPushButton:hover { background: #cd66ff; }";

    QPushButton *btnLoad = new QPushButton("📤"); btnLoad->setFixedSize(50, 50); btnLoad->setStyleSheet(iconBtnStyle);
    QPushButton *btnUndo = new QPushButton("↩️"); btnUndo->setFixedSize(50, 50); btnUndo->setStyleSheet(iconBtnStyle);
    QPushButton *btnReset = new QPushButton("🔄"); btnReset->setFixedSize(50, 50); btnReset->setStyleSheet(iconBtnStyle);

    sidebarLayout->addWidget(btnLoad); sidebarLayout->addWidget(btnUndo); sidebarLayout->addWidget(btnReset); sidebarLayout->addStretch();

    // Connect sidebar buttons to their respective slots
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadImageClicked);
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::onUndoClicked);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    mainLayout->addWidget(sidebar);

    // --- Tabs Area Setup ---
    QTabWidget *tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    // ==========================================
    // TAB 1: Filters & Edges (Main Processing)
    // ==========================================
    QWidget *tab1 = new QWidget();
    QVBoxLayout *tab1Layout = new QVBoxLayout(tab1);

    QHBoxLayout *topToolbarLayout = new QHBoxLayout();
    topToolbarLayout->setContentsMargins(10, 10, 10, 5);

    // Helper Lambda Function: Applies DRY (Don't Repeat Yourself) principle.
    // Dynamically generates identical UI control groups (Label + ComboBox + Button)
    auto createToolGroup = [&](const QString& label, QComboBox*& combo, const QStringList& items, QPushButton*& btn, const QString& btnText) {
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(new QLabel(label));
        combo = new QComboBox(); combo->addItems(items);
        btn = new QPushButton(btnText); btn->setStyleSheet(applyBtnStyle);
        layout->addWidget(combo); layout->addWidget(btn); layout->setSpacing(5);
        return layout;
    };

    QPushButton *btnApplyNoise, *btnApplyFilter, *btnApplyEdge;
    topToolbarLayout->addLayout(createToolGroup("Noise:", comboNoise, {"None", "Uniform", "Gaussian", "Salt & Pepper"}, btnApplyNoise, "Apply Noise"));
    topToolbarLayout->addSpacing(30);
    topToolbarLayout->addLayout(createToolGroup("Filter:", comboFilter, {"None", "Average", "Gaussian", "Median"}, btnApplyFilter, "Apply Filter"));
    topToolbarLayout->addSpacing(30);
    topToolbarLayout->addLayout(createToolGroup("Edge:", comboEdge, {"None", "Sobel", "Roberts", "Prewitt", "Canny"}, btnApplyEdge, "Apply Edge"));
    topToolbarLayout->addStretch();
    tab1Layout->addLayout(topToolbarLayout);

    // --- Dynamic Parameters Panel ---
    // QStackedWidget is used to show/hide specific parameters based on user selection
    paramPanel = new QFrame();
    paramPanel->setObjectName("mainParamPanel"); // Unique ID to prevent CSS inheritance issues

    // Using ID selector '#' to apply border strictly to the outer frame
    paramPanel->setStyleSheet("#mainParamPanel { background-color: #242426; border: 1px solid #9b11ff; border-radius: 6px; margin: 0px 10px; }");

    QHBoxLayout *paramLayout = new QHBoxLayout(paramPanel);
    noiseParamsStack = new QStackedWidget(); filterParamsStack = new QStackedWidget();

    noiseParamsStack->addWidget(new QWidget()); // Index 0: None (Empty state)

    // 1. Uniform Noise Parameters (Min / Max)
    QWidget *pageUniform = new QWidget(); QHBoxLayout *layUniform = new QHBoxLayout(pageUniform);
    sliderUniformMin = new QSlider(Qt::Horizontal); spinUniformMin = new QSpinBox(); spinUniformMin->setRange(-255, 255); sliderUniformMin->setRange(-255, 255); sliderUniformMin->setValue(-50);
    connect(sliderUniformMin, &QSlider::valueChanged, spinUniformMin, &QSpinBox::setValue); connect(spinUniformMin, QOverload<int>::of(&QSpinBox::valueChanged), sliderUniformMin, &QSlider::setValue);
    sliderUniformMax = new QSlider(Qt::Horizontal); spinUniformMax = new QSpinBox(); spinUniformMax->setRange(-255, 255); sliderUniformMax->setRange(-255, 255); sliderUniformMax->setValue(50);
    connect(sliderUniformMax, &QSlider::valueChanged, spinUniformMax, &QSpinBox::setValue); connect(spinUniformMax, QOverload<int>::of(&QSpinBox::valueChanged), sliderUniformMax, &QSlider::setValue);
    layUniform->addWidget(new QLabel("Min:")); layUniform->addWidget(sliderUniformMin); layUniform->addWidget(spinUniformMin); layUniform->addSpacing(20);
    layUniform->addWidget(new QLabel("Max:")); layUniform->addWidget(sliderUniformMax); layUniform->addWidget(spinUniformMax);
    noiseParamsStack->addWidget(pageUniform);

    // 2. Gaussian Noise Parameters (Mean / Variance)
    QWidget *pageGauss = new QWidget(); QHBoxLayout *layGauss = new QHBoxLayout(pageGauss);
    sliderGaussMean = new QSlider(Qt::Horizontal); spinGaussMean = new QDoubleSpinBox(); spinGaussMean->setRange(-255, 255); sliderGaussMean->setRange(-255, 255); sliderGaussMean->setValue(0);
    connect(sliderGaussMean, &QSlider::valueChanged, [=](int v){ spinGaussMean->setValue(v); }); connect(spinGaussMean, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double v){ sliderGaussMean->setValue((int)v); });
    sliderGaussVar = new QSlider(Qt::Horizontal); spinGaussVar = new QDoubleSpinBox(); spinGaussVar->setRange(0, 5000); sliderGaussVar->setRange(0, 5000); sliderGaussVar->setValue(400);
    connect(sliderGaussVar, &QSlider::valueChanged, [=](int v){ spinGaussVar->setValue(v); }); connect(spinGaussVar, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double v){ sliderGaussVar->setValue((int)v); });
    layGauss->addWidget(new QLabel("Mean:")); layGauss->addWidget(sliderGaussMean); layGauss->addWidget(spinGaussMean); layGauss->addSpacing(20);
    layGauss->addWidget(new QLabel("Var:")); layGauss->addWidget(sliderGaussVar); layGauss->addWidget(spinGaussVar);
    noiseParamsStack->addWidget(pageGauss);

    // 3. Salt & Pepper Noise Parameters (Probability)
    QWidget *pageSP = new QWidget(); QHBoxLayout *laySP = new QHBoxLayout(pageSP);
    sliderSaltPepperProb = new QSlider(Qt::Horizontal); spinSaltPepperProb = new QDoubleSpinBox(); spinSaltPepperProb->setRange(0.01, 1.0); spinSaltPepperProb->setSingleStep(0.01); sliderSaltPepperProb->setRange(1, 100); sliderSaltPepperProb->setValue(5);
    connect(sliderSaltPepperProb, &QSlider::valueChanged, [=](int v){ spinSaltPepperProb->setValue(v / 100.0); }); connect(spinSaltPepperProb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double v){ sliderSaltPepperProb->setValue(v * 100); });
    laySP->addWidget(new QLabel("Prob:")); laySP->addWidget(sliderSaltPepperProb); laySP->addWidget(spinSaltPepperProb);
    noiseParamsStack->addWidget(pageSP);

    // Filter Parameters (Mask Size)
    filterParamsStack->addWidget(new QWidget()); // Index 0: None
    QWidget *pageFilterSize = new QWidget(); QHBoxLayout *layFilterSize = new QHBoxLayout(pageFilterSize);
    layFilterSize->addWidget(new QLabel("Mask Size:")); comboMask = new QComboBox(); comboMask->addItems({"3x3", "5x5"}); layFilterSize->addWidget(comboMask); layFilterSize->addStretch();
    filterParamsStack->addWidget(pageFilterSize);

    paramLayout->addWidget(noiseParamsStack); paramLayout->addWidget(filterParamsStack);
    tab1Layout->addWidget(paramPanel); paramPanel->setVisible(false);

    // Connecting UI signals to processing slots
    connect(comboNoise, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onNoiseTypeChanged);
    connect(comboFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterTypeChanged);
    connect(btnApplyNoise, &QPushButton::clicked, this, &MainWindow::onApplyNoiseClicked);
    connect(btnApplyFilter, &QPushButton::clicked, this, &MainWindow::onApplyFilterClicked);
    connect(btnApplyEdge, &QPushButton::clicked, this, &MainWindow::onApplyEdgeClicked);

    // --- Images Display Area ---
    QHBoxLayout *imagesLayout = new QHBoxLayout();
    QString imgStyle = "font-weight: bold; font-size: 14px; color: #9370db; border: 1px solid #555; background-color: #2b2b2e; margin: 10px;";
    imgOriginalLabel = new QLabel("Original Image"); imgOriginalLabel->setAlignment(Qt::AlignCenter); imgOriginalLabel->setStyleSheet(imgStyle); imgOriginalLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgProcessedLabel = new QLabel("Processed Image"); imgProcessedLabel->setAlignment(Qt::AlignCenter); imgProcessedLabel->setStyleSheet(imgStyle); imgProcessedLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imagesLayout->addWidget(imgOriginalLabel, 1); imagesLayout->addWidget(imgProcessedLabel, 1);
    tab1Layout->addLayout(imagesLayout, 1);
    tabWidget->addTab(tab1, "Filters & Edges");

    // ==========================================
    // TAB 2 & 3: Histograms & Frequency
    // ==========================================
    histTab = new HistogramTab();
    tabWidget->addTab(histTab, "Histograms");

    QWidget *tab3 = new QWidget();
    QVBoxLayout *tab3Layout = new QVBoxLayout(tab3);
    QHBoxLayout *freqControlsLayout = new QHBoxLayout();
    QPushButton *btnLoadFreq1 = new QPushButton("Upload Image 1"); btnLoadFreq1->setStyleSheet(applyBtnStyle);
    QPushButton *btnLoadFreq2 = new QPushButton("Upload Image 2"); btnLoadFreq2->setStyleSheet(applyBtnStyle);
    freqControlsLayout->addWidget(btnLoadFreq1); freqControlsLayout->addWidget(btnLoadFreq2); freqControlsLayout->addStretch();
    tab3Layout->addLayout(freqControlsLayout);

    freqTab = new FrequencyTab();
    tab3Layout->addWidget(freqTab);
    connect(btnLoadFreq1, &QPushButton::clicked, this, &MainWindow::onLoadFreq1Clicked);
    connect(btnLoadFreq2, &QPushButton::clicked, this, &MainWindow::onLoadFreq2Clicked);
    tabWidget->addTab(tab3, "Frequency & Hybrid");

    resize(1250, 700);
}

MainWindow::~MainWindow() {}

// ==========================================
// UI Visibility & History Helpers
// ==========================================
void MainWindow::updateParameterPanelVisibility() {
    bool hasNoiseParams = (comboNoise->currentIndex() > 0);
    bool hasFilterParams = (comboFilter->currentIndex() > 0);
    noiseParamsStack->setVisible(hasNoiseParams);
    filterParamsStack->setVisible(hasFilterParams);
    paramPanel->setVisible(hasNoiseParams || hasFilterParams);
}

void MainWindow::onNoiseTypeChanged(int index) {
    noiseParamsStack->setCurrentIndex(index);
    updateParameterPanelVisibility();
    // Reset view instantly if "None" is selected
    if (index == 0 && !originalImage.empty()) {
        saveToHistory();
        processedImage = originalImage.clone();
        Utils::displayImageStandard(originalImage, imgOriginalLabel);
        Utils::displayImageStandard(processedImage, imgProcessedLabel);
    }
}

void MainWindow::onFilterTypeChanged(int index) {
    filterParamsStack->setCurrentIndex(index > 0 ? 1 : 0);
    updateParameterPanelVisibility();
}

// Maintains an undo history by deeply cloning the current matrix state into memory
void MainWindow::saveToHistory() {
    if (!processedImage.empty()) imageHistory.push_back(processedImage.clone());
}

// ==========================================
// Core Image Loading Logic (Disk I/O)
// ==========================================
// Centralized Lambda to ensure cv::imread is only called ONCE from the disk.
// Prevents redundant I/O operations and memory leaks.
auto loadOpenCVImage = [](QWidget* parent, const QString& title) -> cv::Mat {
    QString fileName = QFileDialog::getOpenFileName(parent, title, "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if(fileName.isEmpty()) return cv::Mat();
    cv::Mat img = cv::imread(fileName.toStdString());
    if(img.empty()) QMessageBox::warning(parent, "Error", "Could not load image!");
    return img;
};

void MainWindow::onLoadImageClicked() {
    cv::Mat img = loadOpenCVImage(this, "Open Main Image");
    if (img.empty()) return;

    // Store deep copies in memory. All subsequent processing happens in RAM.
    originalImage = img;
    processedImage = originalImage.clone();
    imageHistory.clear();

    Utils::displayImageStandard(originalImage, imgOriginalLabel);
    Utils::displayImageStandard(processedImage, imgProcessedLabel);
    if (histTab) histTab->setSourceImage(processedImage);
}

void MainWindow::onUndoClicked() {
    if (!imageHistory.empty()) {
        processedImage = imageHistory.back();
        imageHistory.pop_back();
    } else if (!originalImage.empty()) {
        processedImage = originalImage.clone();
    }
    Utils::displayImageStandard(originalImage, imgOriginalLabel);
    Utils::displayImageStandard(processedImage, imgProcessedLabel);
    if (histTab) histTab->setSourceImage(processedImage);
}

void MainWindow::onResetClicked() {
    if (originalImage.empty()) return;
    processedImage = originalImage.clone(); // Revert to initial memory state
    imageHistory.clear();
    Utils::displayImageStandard(originalImage, imgOriginalLabel);
    Utils::displayImageStandard(processedImage, imgProcessedLabel);
    if (histTab) histTab->setSourceImage(processedImage);
}

void MainWindow::onLoadFreq1Clicked() {
    cv::Mat img = loadOpenCVImage(this, "Open Low-Freq Image");
    if (!img.empty()) freqTab->setImage1(img);
}

void MainWindow::onLoadFreq2Clicked() {
    cv::Mat img = loadOpenCVImage(this, "Open High-Freq Image");
    if (!img.empty()) freqTab->setImage2(img);
}

// ==========================================
// Image Processing Algorithms
// ==========================================

void MainWindow::onApplyNoiseClicked() {
    if (processedImage.empty() || comboNoise->currentIndex() == 0) return;
    saveToHistory();

    int type = comboNoise->currentIndex();
    cv::Mat noiseImg = cv::Mat::zeros(processedImage.size(), processedImage.type()), result;

    if (type == 1) {
        // Uniform Noise: Additive random values bounded by min/max
        cv::randu(noiseImg, cv::Scalar::all(spinUniformMin->value()), cv::Scalar::all(spinUniformMax->value()));
        cv::add(processedImage, noiseImg, result, cv::noArray(), processedImage.type());
    } else if (type == 2) {
        // Gaussian Noise: Additive random values based on Normal Distribution (Mean, StdDev)
        cv::randn(noiseImg, cv::Scalar::all(spinGaussMean->value()), cv::Scalar::all(std::sqrt(spinGaussVar->value())));
        cv::add(processedImage, noiseImg, result, cv::noArray(), processedImage.type());
    } else if (type == 3) {
        // Salt & Pepper Noise: Probabilistic pixel replacement (0 or 255)
        double prob = spinSaltPepperProb->value();
        result = processedImage.clone();
        cv::Mat randMat(result.size(), CV_32F);
        cv::randu(randMat, 0.0, 1.0); // Generate probability matrix

        for (int i = 0; i < result.rows; i++) {
            for (int j = 0; j < result.cols; j++) {
                float randomValue = randMat.at<float>(i, j);
                if (randomValue < prob / 2.0) { // Pepper (Black)
                    if (result.channels() == 3) result.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                    else result.at<uchar>(i, j) = 0;
                } else if (randomValue > 1.0 - (prob / 2.0)) { // Salt (White)
                    if (result.channels() == 3) result.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
                    else result.at<uchar>(i, j) = 255;
                }
            }
        }
    }
    processedImage = result;
    Utils::displayImageStandard(processedImage, imgProcessedLabel);
}

void MainWindow::onApplyFilterClicked() {
    if (processedImage.empty() || comboFilter->currentIndex() == 0) return;
    saveToHistory();

    int type = comboFilter->currentIndex();
    int ksize = (comboMask->currentIndex() == 0) ? 3 : 5; // Extract Kernel Size
    cv::Mat result;

    // Apply specific spatial filtering using OpenCV built-in functions
    if (type == 1) cv::blur(processedImage, result, cv::Size(ksize, ksize));
    else if (type == 2) cv::GaussianBlur(processedImage, result, cv::Size(ksize, ksize), 0);
    else if (type == 3) cv::medianBlur(processedImage, result, ksize);

    processedImage = result;
    Utils::displayImageStandard(processedImage, imgProcessedLabel);
}

void MainWindow::onApplyEdgeClicked() {
    if (processedImage.empty() || comboEdge->currentIndex() == 0) return;
    saveToHistory();

    int type = comboEdge->currentIndex();
    cv::Mat gray, result;

    // Convert to Grayscale for edge detection algorithms
    if (processedImage.channels() == 3) cv::cvtColor(processedImage, gray, cv::COLOR_BGR2GRAY);
    else gray = processedImage.clone();

    if (type == 4) {
        // Canny: Multi-stage edge detection (using OpenCV implementation)
        cv::Canny(gray, result, 100, 200);
    } else {
        result = cv::Mat::zeros(gray.size(), CV_8UC1);
        int kx[3][3] = {0}, ky[3][3] = {0};

        // Define Kernels based on user selection
        if (type == 1) { // Sobel Masks
            int sx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
            int sy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
            memcpy(kx, sx, sizeof(sx)); memcpy(ky, sy, sizeof(sy));
        } else if (type == 3) { // Prewitt Masks
            int px[3][3] = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
            int py[3][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};
            memcpy(kx, px, sizeof(px)); memcpy(ky, py, sizeof(py));
        }

        bool isRoberts = (type == 2);
        int offset = isRoberts ? 0 : 1; // Adjust padding based on kernel size (2x2 vs 3x3)

        // Unified Convolution Loop: Evaluates Roberts (2x2) or Sobel/Prewitt (3x3)
        // This eliminates redundant loops and enforces DRY architecture.
        for (int i = offset; i < gray.rows - 1; i++) {
            for (int j = offset; j < gray.cols - 1; j++) {
                int gx = 0, gy = 0;

                if (isRoberts) {
                    // Roberts Cross Gradient Operators
                    gx = gray.at<uchar>(i, j) - gray.at<uchar>(i+1, j+1);
                    gy = gray.at<uchar>(i, j+1) - gray.at<uchar>(i+1, j);
                } else {
                    // Standard 3x3 Convolution
                    for (int ki = -1; ki <= 1; ki++) {
                        for (int kj = -1; kj <= 1; kj++) {
                            int pixel = gray.at<uchar>(i + ki, j + kj);
                            gx += pixel * kx[ki + 1][kj + 1];
                            gy += pixel * ky[ki + 1][kj + 1];
                        }
                    }
                }
                // Calculate gradient magnitude: M = sqrt(gx^2 + gy^2)
                result.at<uchar>(i, j) = cv::saturate_cast<uchar>(std::sqrt(gx*gx + gy*gy));
            }
        }
    }
    processedImage = result;
    Utils::displayImageStandard(processedImage, imgProcessedLabel);
}
