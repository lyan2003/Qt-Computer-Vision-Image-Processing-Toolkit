#ifndef HISTOGRAMTAB_H
#define HISTOGRAMTAB_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <opencv2/opencv.hpp>
#include "utils.h" // Import shared classes

class HistogramTab : public QWidget
{
    Q_OBJECT
public:
    explicit HistogramTab(QWidget *parent = nullptr);
    void setSourceImage(const cv::Mat& img);

private slots:
    void onEqualizeClicked();
    void onNormalizeClicked();
    void onModeToggled(bool checked);

private:
    cv::Mat sourceImage;
    cv::Mat processedImage;
    bool isGrayscaleMode;

    QStackedWidget *stackedWidget;
    QWidget *pageRGB;
    QWidget *pageGray;

    ResponsiveLabel *rgbInputImgLabel, *rgbOutputImgLabel;
    ResponsiveLabel *histRLabel, *histGLabel, *histBLabel;
    ResponsiveLabel *cdfRLabel, *cdfGLabel, *cdfBLabel;
    ResponsiveLabel *grayInputImgLabel, *grayOutputImgLabel;
    ResponsiveLabel *grayHistLabel, *grayCdfLabel;

    cv::Mat drawChart(const cv::Mat& data, cv::Scalar color, bool isCDF);
    void updateViews();
};

#endif // HISTOGRAMTAB_H
