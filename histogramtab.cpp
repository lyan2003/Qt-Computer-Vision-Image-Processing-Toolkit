#include "histogramtab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>

HistogramTab::HistogramTab(QWidget *parent) : QWidget(parent)
{
    isGrayscaleMode = false;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Toolbar logic...
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    QPushButton *btnEqualize = new QPushButton("Equalize Image");
    QPushButton *btnNormalize = new QPushButton("Normalize Image");

    QString styleDarkPurple = "QPushButton { padding: 6px; background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #b026ff, stop:1 #7a00cc); color: white; border-radius: 5px; font-weight: bold; }";
    btnEqualize->setStyleSheet(styleDarkPurple);
    btnNormalize->setStyleSheet(styleDarkPurple);

    QSlider *modeSwitch = new QSlider(Qt::Horizontal);
    modeSwitch->setRange(0, 1); modeSwitch->setFixedSize(46, 22);
    // Add your slider styling here...

    controlsLayout->addWidget(btnEqualize);
    controlsLayout->addWidget(btnNormalize);
    controlsLayout->addStretch();
    controlsLayout->addWidget(new QLabel("RGB"));
    controlsLayout->addWidget(modeSwitch);
    controlsLayout->addWidget(new QLabel("Gray"));

    connect(btnEqualize, &QPushButton::clicked, this, &HistogramTab::onEqualizeClicked);
    connect(btnNormalize, &QPushButton::clicked, this, &HistogramTab::onNormalizeClicked);
    connect(modeSwitch, &QSlider::valueChanged, this, [this](int value) { onModeToggled(value == 1); });

    mainLayout->addLayout(controlsLayout);

    // UI Setup: Initialize labels before passing them
    rgbInputImgLabel = new ResponsiveLabel(); rgbOutputImgLabel = new ResponsiveLabel();
    histRLabel = new ResponsiveLabel(); histGLabel = new ResponsiveLabel(); histBLabel = new ResponsiveLabel();
    cdfRLabel = new ResponsiveLabel(); cdfGLabel = new ResponsiveLabel(); cdfBLabel = new ResponsiveLabel();
    grayInputImgLabel = new ResponsiveLabel(); grayOutputImgLabel = new ResponsiveLabel();
    grayHistLabel = new ResponsiveLabel(); grayCdfLabel = new ResponsiveLabel();

    stackedWidget = new QStackedWidget();

    // Page 1: RGB Layout
    pageRGB = new QWidget();
    QHBoxLayout *rgbLayout = new QHBoxLayout(pageRGB);

    QVBoxLayout *rgbCol1 = new QVBoxLayout();
    rgbCol1->addWidget(Utils::createBox("Original Input Image", rgbInputImgLabel));
    rgbCol1->addWidget(Utils::createBox("Processed Output Image", rgbOutputImgLabel));

    QVBoxLayout *rgbCol2 = new QVBoxLayout();
    rgbCol2->addWidget(Utils::createBox("Histogram (Red)", histRLabel));
    rgbCol2->addWidget(Utils::createBox("Histogram (Green)", histGLabel));
    rgbCol2->addWidget(Utils::createBox("Histogram (Blue)", histBLabel));

    QVBoxLayout *rgbCol3 = new QVBoxLayout();
    rgbCol3->addWidget(Utils::createBox("Distribution Curve (Red)", cdfRLabel));
    rgbCol3->addWidget(Utils::createBox("Distribution Curve (Green)", cdfGLabel));
    rgbCol3->addWidget(Utils::createBox("Distribution Curve (Blue)", cdfBLabel));

    rgbLayout->addLayout(rgbCol1, 2); rgbLayout->addLayout(rgbCol2, 3); rgbLayout->addLayout(rgbCol3, 3);

    // Page 2: Gray Layout
    pageGray = new QWidget();
    QGridLayout *grayLayout = new QGridLayout(pageGray);
    grayLayout->addWidget(Utils::createBox("Original Input Image", grayInputImgLabel), 0, 0);
    grayLayout->addWidget(Utils::createBox("Processed Output Image", grayOutputImgLabel), 1, 0);
    grayLayout->addWidget(Utils::createBox("Histogram (Grayscale)", grayHistLabel), 0, 1);
    grayLayout->addWidget(Utils::createBox("Distribution Curve (Grayscale)", grayCdfLabel), 1, 1);

    stackedWidget->addWidget(pageRGB); stackedWidget->addWidget(pageGray);
    mainLayout->addWidget(stackedWidget);
}

void HistogramTab::onModeToggled(bool checked) {
    isGrayscaleMode = checked;
    stackedWidget->setCurrentIndex(isGrayscaleMode ? 1 : 0);
    updateViews();
}

void HistogramTab::setSourceImage(const cv::Mat& img) {
    if (img.empty()) return;
    sourceImage = img.clone();
    processedImage = sourceImage.clone();
    updateViews();
}

// Draw histogram or CDF chart with X and Y axes labels and title
cv::Mat HistogramTab::drawChart(const cv::Mat& data, cv::Scalar color, bool isCDF) {

    // ==============================
    // 1️⃣ Setup canvas and margins
    // ==============================
    int width = 500, height = 220;  // Total image size
    cv::Mat plot = cv::Mat::zeros(height, width, CV_8UC3); // Black canvas
    int marginLeft = 65, marginRight = 20, marginTop = 35, marginBottom = 30; // Margins
    int plotW = width - marginLeft - marginRight;  // Width of plotting area
    int plotH = height - marginTop - marginBottom; // Height of plotting area
    int font = cv::FONT_HERSHEY_SIMPLEX;

    // ==============================
    // 2️⃣ Draw axes and border
    // ==============================
    cv::rectangle(plot, cv::Point(5, 5), cv::Point(width - 5, height - 5), cv::Scalar(60, 60, 60), 1); // Outer border
    cv::line(plot, cv::Point(marginLeft, marginTop), cv::Point(marginLeft, height - marginBottom), cv::Scalar(200, 200, 200), 1); // Y-axis
    cv::line(plot, cv::Point(marginLeft, height - marginBottom), cv::Point(width - marginRight, height - marginBottom), cv::Scalar(200, 200, 200), 1); // X-axis

    // ==============================
    // 3️⃣ X-axis logic (Intensity values 0-255)
    // ==============================
    for (int i = 0; i <= 5; ++i) {
        char buf[10];
        int val = i * 50;  // X-axis labels: 0, 50, 100, 150, 200, 250
        int x = marginLeft + (plotW * val) / 250;
        snprintf(buf, sizeof(buf), "%d", val);

        // Draw the X-axis label
        cv::putText(plot, buf, cv::Point(x - 12, height - marginBottom + 18), font, 0.35, cv::Scalar(200, 200, 200), 1);
        // Draw a small tick line
        cv::line(plot, cv::Point(x, height - marginBottom), cv::Point(x, height - marginBottom + 4), cv::Scalar(200, 200, 200), 1);
    }

    // ==============================
    // 4️⃣ Y-axis logic (Ticks, labels, title)
    // ==============================
    double maxVal;
    cv::minMaxLoc(data, nullptr, &maxVal);  // Find max value for Y-axis scaling
    if (maxVal == 0) maxVal = 1;

    int numTicks = 4; // Number of ticks/steps on Y-axis
    for (int i = 0; i <= numTicks; ++i) {
        int y = height - marginBottom - (plotH * i) / numTicks;  // Position of tick
        char buf[20];

        if (isCDF) {
            // For CDF: Y-axis shows cumulative probability (0.0 - 1.0)
            snprintf(buf, sizeof(buf), "%.2f", (double)i / numTicks);
        } else {
            // For Histogram: Y-axis shows frequency, format with 'k' for thousands
            double tickVal = (maxVal * i) / numTicks;
            if (tickVal >= 1000) snprintf(buf, sizeof(buf), "%.1fk", tickVal / 1000.0);
            else snprintf(buf, sizeof(buf), "%d", (int)tickVal);
        }

        // Draw Y-tick mark
        cv::line(plot, cv::Point(marginLeft - 4, y), cv::Point(marginLeft, y), cv::Scalar(200, 200, 200), 1);

        // Draw Y-axis label next to tick
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(buf, font, 0.35, 1, &baseline);
        cv::putText(plot, buf, cv::Point(marginLeft - 8 - textSize.width, y + 4), font, 0.35, cv::Scalar(200, 200, 200), 1);
    }

    // Draw Y-axis title
    cv::putText(plot, isCDF ? "Prob" : "Freq", cv::Point(marginLeft - 35, marginTop - 15), font, 0.45, cv::Scalar(180, 180, 180), 1);

    // ==============================
    // 5️⃣ Normalize data to fit plot height
    // ==============================
    cv::Mat normData;
    cv::normalize(data, normData, 0, plotH, cv::NORM_MINMAX);
    float binW = (float)plotW / 256.0f; // Width of each bin

    // ==============================
    // 6️⃣ Draw the histogram bars or CDF line
    // ==============================
    for (int i = 1; i < 256; i++) {
        int x1 = marginLeft + cvRound(binW * (i - 1));
        int y1 = height - marginBottom - cvRound(normData.at<float>(i - 1));
        int x2 = marginLeft + cvRound(binW * i);
        int y2 = height - marginBottom - cvRound(normData.at<float>(i));

        if (isCDF)
            // CDF: draw line connecting points
            cv::line(plot, cv::Point(x1, y1), cv::Point(x2, y2), color, 2, cv::LINE_AA);
        else
            // Histogram: draw filled rectangle/bar
            cv::rectangle(plot, cv::Point(x1, y1), cv::Point(x2, height - marginBottom), color, cv::FILLED);
    }

    return plot; // Return the chart image
}

void HistogramTab::updateViews() {

    // Exit if either source or processed image is missing
    if (sourceImage.empty() || processedImage.empty()) return;

    // Histogram configuration
    int histSize = 256;                    // Number of bins (for 8-bit image: 0–255)
    float range[] = {0, 256};              // Intensity range
    const float* histRange[] = {range};    // Pointer required by calcHist()

    // =========================
    // GRAYSCALE MODE
    // =========================
    if (isGrayscaleMode) {

        // Display original image in grayscale section
        Utils::displayImage(sourceImage, grayInputImgLabel);

        cv::Mat grayProcessed;

        // Ensure processed image is grayscale
        if (processedImage.channels() == 3)
            cv::cvtColor(processedImage, grayProcessed, cv::COLOR_BGR2GRAY);
        else
            grayProcessed = processedImage.clone();

        // Display processed grayscale image
        Utils::displayImage(grayProcessed, grayOutputImgLabel);

        cv::Mat gray_hist, gray_cdf;

        // Calculate histogram (counts intensity frequency)
        cv::calcHist(&grayProcessed, 1, 0, cv::Mat(),
                     gray_hist, 1, &histSize, histRange);

        // Compute CDF (Cumulative Distribution Function)
        // Each value accumulates previous values
        gray_cdf = gray_hist.clone();
        for (int i = 1; i < histSize; i++)
            gray_cdf.at<float>(i) += gray_cdf.at<float>(i - 1);

        // Draw and display histogram
        Utils::displayImage(
            drawChart(gray_hist, cv::Scalar(180, 180, 180), false),
            grayHistLabel);

        // Draw and display CDF
        Utils::displayImage(
            drawChart(gray_cdf, cv::Scalar(180, 180, 180), true),
            grayCdfLabel);
    }

    // =========================
    // RGB MODE
    // =========================
    else {

        // Display original and processed color images
        Utils::displayImage(sourceImage, rgbInputImgLabel);
        Utils::displayImage(processedImage, rgbOutputImgLabel);

        cv::Mat colorWorkImg = processedImage.clone();

        // Ensure image has 3 channels (convert if grayscale)
        if (colorWorkImg.channels() == 1)
            cv::cvtColor(colorWorkImg, colorWorkImg, cv::COLOR_GRAY2BGR);

        std::vector<cv::Mat> bgr_planes;

        // Split image into B, G, R channels
        cv::split(colorWorkImg, bgr_planes);

        // Arrays to store histogram and CDF for each channel
        cv::Mat hist[3], cdf[3];

        // Colors used to draw each channel's chart (B, G, R)
        cv::Scalar colors[3] = {
            cv::Scalar(255, 0, 0),   // Blue
            cv::Scalar(0, 255, 0),   // Green
            cv::Scalar(0, 0, 255)    // Red
        };

        // Labels to display histograms and CDFs
        ResponsiveLabel* histLabels[3] = {histBLabel, histGLabel, histRLabel};
        ResponsiveLabel* cdfLabels[3]  = {cdfBLabel,  cdfGLabel,  cdfRLabel};

        // Loop through B, G, R channels
        for (int i = 0; i < 3; i++) {

            // Calculate histogram for channel i
            cv::calcHist(&bgr_planes[i], 1, 0, cv::Mat(),
                         hist[i], 1, &histSize, histRange);

            // Draw and display histogram
            Utils::displayImage(
                drawChart(hist[i], colors[i], false),
                histLabels[i]);

            // Compute CDF for channel i
            cdf[i] = hist[i].clone();
            for (int j = 1; j < histSize; j++)
                cdf[i].at<float>(j) += cdf[i].at<float>(j - 1);

            // Draw and display CDF
            Utils::displayImage(
                drawChart(cdf[i], colors[i], true),
                cdfLabels[i]);
        }
    }
}

void HistogramTab::onEqualizeClicked() {

    // If there is no image loaded, exit the function
    if (processedImage.empty()) return;

    // Case 1: Grayscale image (single channel)
    if (processedImage.channels() == 1) {

        // Apply histogram equalization directly
        // This redistributes intensity values to enhance contrast
        cv::equalizeHist(processedImage, processedImage);

    } else {

        // Case 2: Color image (3 channels BGR)

        cv::Mat ycrcb;

        // Convert from BGR to YCrCb color space
        // Y  = Luminance (brightness)
        // Cr = Red chroma
        // Cb = Blue chroma
        // We modify only the brightness channel to avoid color distortion
        cv::cvtColor(processedImage, ycrcb, cv::COLOR_BGR2YCrCb);

        std::vector<cv::Mat> channels;

        // Split the YCrCb image into separate channels
        cv::split(ycrcb, channels);

        // Apply histogram equalization only on Y channel (brightness)
        // This improves contrast without changing colors
        cv::equalizeHist(channels[0], channels[0]);

        // Merge the modified channels back into one image
        cv::merge(channels, ycrcb);

        // Convert back from YCrCb to BGR color space
        cv::cvtColor(ycrcb, processedImage, cv::COLOR_YCrCb2BGR);
    }

    // Refresh histogram and image display after equalization
    updateViews();
}

void HistogramTab::onNormalizeClicked() {

    if (processedImage.empty()) return;

    // Normalize the pixel values of the processed image
    // cv::normalize(src, dst, alpha, beta, norm_type)
    // alpha = 0, beta = 255 → scale pixel values to range [0, 255]
    // cv::NORM_MINMAX → linear scaling based on min and max pixel values
    cv::normalize(processedImage, processedImage, 0, 255, cv::NORM_MINMAX);

    updateViews();
}
