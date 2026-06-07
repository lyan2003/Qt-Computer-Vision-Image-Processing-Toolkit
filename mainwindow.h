#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QStackedWidget>
#include <QFrame>
#include <opencv2/opencv.hpp>
#include "histogramtab.h"
#include "frequencytab.h"
#include "utils.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadImageClicked();
    void onUndoClicked();
    void onResetClicked();
    void onApplyNoiseClicked();
    void onApplyFilterClicked();
    void onApplyEdgeClicked();
    void onNoiseTypeChanged(int index);
    void onFilterTypeChanged(int index);
    void onLoadFreq1Clicked();
    void onLoadFreq2Clicked();

private:
    QLabel *imgOriginalLabel;
    QLabel *imgProcessedLabel;
    QComboBox *comboNoise, *comboFilter, *comboEdge;

    QFrame *paramPanel;
    QStackedWidget *noiseParamsStack, *filterParamsStack;

    QSpinBox *spinUniformMin, *spinUniformMax;
    QSlider *sliderUniformMin, *sliderUniformMax;
    QDoubleSpinBox *spinGaussMean, *spinGaussVar;
    QSlider *sliderGaussMean, *sliderGaussVar;
    QDoubleSpinBox *spinSaltPepperProb;
    QSlider *sliderSaltPepperProb;

    QComboBox *comboMask;

    HistogramTab *histTab;
    FrequencyTab *freqTab;

    cv::Mat originalImage;
    cv::Mat processedImage;
    std::vector<cv::Mat> imageHistory;

    void saveToHistory();
    void updateParameterPanelVisibility();
};
#endif // MAINWINDOW_H
