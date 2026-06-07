#include "frequencytab.h"
#include <QVBoxLayout>
#include <QGridLayout>

FrequencyTab::FrequencyTab(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    img1Label = new ResponsiveLabel();
    img2Label = new ResponsiveLabel();
    hybridLabel = new ResponsiveLabel();

    lowPassSlider = new QSlider(Qt::Horizontal);
    lowPassSlider->setRange(1, 50); lowPassSlider->setValue(15);

    highPassSlider = new QSlider(Qt::Horizontal);
    highPassSlider->setRange(1, 50); highPassSlider->setValue(15);

    connect(lowPassSlider, &QSlider::valueChanged, this, &FrequencyTab::updateHybridImage);
    connect(highPassSlider, &QSlider::valueChanged, this, &FrequencyTab::updateHybridImage);

    QGridLayout *gridLayout = new QGridLayout();

    // Container 1
    QWidget *container1 = new QWidget();
    QVBoxLayout *col1Layout = new QVBoxLayout(container1);
    col1Layout->setContentsMargins(0, 0, 0, 0);
    col1Layout->addWidget(new QLabel("Low-Pass Filter Radius:"));
    col1Layout->addWidget(lowPassSlider);
    col1Layout->addWidget(img1Label, 1);

    // Container 2
    QWidget *container2 = new QWidget();
    QVBoxLayout *col2Layout = new QVBoxLayout(container2);
    col2Layout->setContentsMargins(0, 0, 0, 0);
    col2Layout->addWidget(new QLabel("High-Pass Filter Radius:"));
    col2Layout->addWidget(highPassSlider);
    col2Layout->addWidget(img2Label, 1);

    // Using Shared Utils to create boxes
    gridLayout->addWidget(Utils::createBox("Image 1 (Low Freq)", container1), 0, 0);
    gridLayout->addWidget(Utils::createBox("Image 2 (High Freq)", container2), 1, 0);
    gridLayout->addWidget(Utils::createBox("Hybrid Image Result", hybridLabel), 0, 1, 2, 1);

    gridLayout->setRowStretch(0, 1); gridLayout->setRowStretch(1, 1);
    gridLayout->setColumnStretch(0, 1); gridLayout->setColumnStretch(1, 2);

    mainLayout->addLayout(gridLayout);
}

void FrequencyTab::setImage1(const cv::Mat& img) {
    if (img.empty()) return;
    image1 = img.clone();
    // Initially display the original, but it will be immediately updated by the slider logic
    Utils::displayImage(image1, img1Label);
    updateHybridImage();
}

void FrequencyTab::setImage2(const cv::Mat& img) {
    if (img.empty()) return;
    image2 = img.clone();
    // Initially display the original, but it will be immediately updated by the slider logic
    Utils::displayImage(image2, img2Label);
    updateHybridImage();
}

void FrequencyTab::updateHybridImage() {
    // --- 1. Validation and Preparation ---
    if (image1.empty() || image2.empty()) return;

    // Convert to Grayscale for FFT (standard for hybrid images)
    cv::Mat gray1, gray2;
    cv::cvtColor(image1, gray1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(image2, gray2, cv::COLOR_BGR2GRAY);

    // Resize to a power of 2 (e.g., 512x512) to optimize FFT speed
    cv::resize(gray1, gray1, cv::Size(512, 512));
    cv::resize(gray2, gray2, cv::Size(512, 512));

    // Lambda function to handle the Frequency Domain Filtering
    auto applyFFTFilter = [&](const cv::Mat& input, float radius, bool isHighPass) {

        // --- 2. Expand image to optimal DFT size ---
        cv::Mat padded;
        int m = cv::getOptimalDFTSize(input.rows);
        int n = cv::getOptimalDFTSize(input.cols);
        // Add zero padding to the borders
        cv::copyMakeBorder(input, padded, 0, m - input.rows, 0, n - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

        // --- 3. Make room for Complex numbers (Real + Imaginary) ---
        // DFT output is complex, so we need a 2-channel matrix
        cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
        cv::Mat complexImg;
        cv::merge(planes, 2, complexImg);

        // --- 4. Perform Discrete Fourier Transform (DFT) ---
        cv::dft(complexImg, complexImg);

        // --- 5. Shift Zero-frequency component to the center ---
        // Rearrange quadrants so that low frequencies are in the middle
        int cx = complexImg.cols / 2;
        int cy = complexImg.rows / 2;
        cv::Mat q0(complexImg, cv::Rect(0, 0, cx, cy));   // Top-Left
        cv::Mat q1(complexImg, cv::Rect(cx, 0, cx, cy));  // Top-Right
        cv::Mat q2(complexImg, cv::Rect(0, cy, cx, cy));  // Bottom-Left
        cv::Mat q3(complexImg, cv::Rect(cx, cy, cx, cy)); // Bottom-Right
        cv::Mat tmp;
        q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);    // Swap TL with BR
        q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);    // Swap TR with BL

        // --- 6. Create the Frequency Filter Mask ---
        // Ideal Low-Pass/High-Pass filter logic based on distance from center
        cv::Mat mask = cv::Mat::zeros(complexImg.size(), CV_32F);
        for (int i = 0; i < mask.rows; i++) {
            for (int j = 0; j < mask.cols; j++) {
                float dist = std::sqrt(std::pow(i - cy, 2) + std::pow(j - cx, 2));
                if (isHighPass) {
                    mask.at<float>(i, j) = (dist > radius) ? 1.0f : 0.0f;
                } else {
                    mask.at<float>(i, j) = (dist <= radius) ? 1.0f : 0.0f;
                }
            }
        }

        // --- 7. Apply the Mask to the Spectrum ---
        // Element-wise multiplication of the complex image with our mask
        cv::Mat maskChannels[] = {mask, mask};
        cv::Mat complexMask;
        cv::merge(maskChannels, 2, complexMask);
        cv::multiply(complexImg, complexMask, complexImg);

        // --- 8. Inverse DFT to return to Spatial Domain ---
        // Shift back before Inverse DFT
        q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
        q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);

        cv::idft(complexImg, complexImg);

        // Extract the magnitude/real part
        cv::split(complexImg, planes);
        cv::normalize(planes[0], planes[0], 0, 1, cv::NORM_MINMAX);
        return planes[0];
    };

    // --- 9. Process both images and combine ---
    float r1 = lowPassSlider->value();
    float r2 = highPassSlider->value();

    cv::Mat lowFreq = applyFFTFilter(gray1, r1, false); // Keep Low Frequencies
    cv::Mat highFreq = applyFFTFilter(gray2, r2, true); // Keep High Frequencies

    // --- NEW: Prepare intermediate images for display ---
    cv::Mat lowFreqDisplay, highFreqDisplay;

    // Normalize and convert Image 1 (Low-Pass) to be visible (0-255 range)
    cv::normalize(lowFreq, lowFreqDisplay, 0, 255, cv::NORM_MINMAX);
    lowFreqDisplay.convertTo(lowFreqDisplay, CV_8UC1);

    // Normalize and convert Image 2 (High-Pass) to be visible (0-255 range)
    cv::normalize(highFreq, highFreqDisplay, 0, 255, cv::NORM_MINMAX);
    highFreqDisplay.convertTo(highFreqDisplay, CV_8UC1);

    // --- 10. Merge by averaging the two results ---
    cv::Mat hybrid = (lowFreq + highFreq) / 2.0;
    cv::normalize(hybrid, hybrid, 0, 255, cv::NORM_MINMAX);
    hybrid.convertTo(hybrid, CV_8UC1);

    // --- 11. Update UI ---
    // Display the filtered intermediate images dynamically
    Utils::displayImage(lowFreqDisplay, img1Label);
    Utils::displayImage(highFreqDisplay, img2Label);
    // Display the final combined hybrid image
    Utils::displayImage(hybrid, hybridLabel);
}
