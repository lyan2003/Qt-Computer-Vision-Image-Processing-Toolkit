#ifndef FREQUENCYTAB_H
#define FREQUENCYTAB_H

#include <QWidget>
#include <QSlider>
#include <opencv2/opencv.hpp>
#include "utils.h" // Import shared classes

class FrequencyTab : public QWidget
{
    Q_OBJECT
public:
    explicit FrequencyTab(QWidget *parent = nullptr);
    void setImage1(const cv::Mat& img);
    void setImage2(const cv::Mat& img);

private slots:
    void updateHybridImage();

private:
    cv::Mat image1;
    cv::Mat image2;

    ResponsiveLabel *img1Label;
    ResponsiveLabel *img2Label;
    ResponsiveLabel *hybridLabel;

    QSlider *lowPassSlider;
    QSlider *highPassSlider;
};

#endif // FREQUENCYTAB_H
