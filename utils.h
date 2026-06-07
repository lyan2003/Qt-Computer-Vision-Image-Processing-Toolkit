#ifndef UTILS_H
#define UTILS_H

#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QPainter>
#include <opencv2/opencv.hpp>

// ==========================================
// 1. Unified Responsive Label
// ==========================================
class ResponsiveLabel : public QLabel {
    Q_OBJECT
public:
    explicit ResponsiveLabel(QWidget *parent = nullptr) : QLabel(parent) {
        // Make the label expand to available space
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(100, 100); // minimum size to avoid disappearing
    }

    // Store the original QPixmap for scaling later
    void setOriginalPixmap(const QPixmap &pm) {
        originalPixmap = pm;
        update(); // triggers repaint
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        // If there's no image, fall back to normal QLabel paint
        if (originalPixmap.isNull()) {
            QLabel::paintEvent(event);
            return;
        }

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform); // smooth scaling

        // Scale the pixmap to fit the label while keeping aspect ratio
        QPixmap scaled = originalPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Center the image inside the label
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawPixmap(x, y, scaled);
    }

private:
    QPixmap originalPixmap; // stores the original image for rescaling
};

// ==========================================
// 2. Shared Utilities Helper Class
// Contains all repeated logic (DRY Principle)
// ==========================================
class Utils {
public:
    // --- 1. Create a framed box with a title for widgets ---
    static QWidget* createBox(const QString& title, QWidget* content) {
        QFrame* frame = new QFrame();
        frame->setStyleSheet(
            "QFrame { background-color: #2b2b2e; border: 1px solid #555; border-radius: 6px; margin: 2px; }"
            "QLabel { border: none; background: transparent; }"
            );

        QVBoxLayout* layout = new QVBoxLayout(frame);
        layout->setContentsMargins(10, 8, 10, 10);

        QLabel* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 13px; color: #9370db;");
        layout->addWidget(titleLabel);

        QFrame* hline = new QFrame();
        hline->setFrameShape(QFrame::HLine);
        hline->setStyleSheet("background-color: #555; max-height: 1px; margin-bottom: 5px;");
        layout->addWidget(hline);

        layout->addWidget(content); // add the main content widget
        return frame;
    }

    // --- 2. Display OpenCV Mat on ResponsiveLabel ---
    static void displayImage(const cv::Mat& img, ResponsiveLabel* label) {
        if (img.empty()) return;

        cv::Mat rgbMat;
        if (img.channels() == 3)
            cv::cvtColor(img, rgbMat, cv::COLOR_BGR2RGB); // convert BGR to RGB
        else
            cv::cvtColor(img, rgbMat, cv::COLOR_GRAY2RGB); // gray to RGB

        QImage qImg(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
        label->setOriginalPixmap(QPixmap::fromImage(qImg)); // set pixmap for responsive scaling
    }

    // --- 3. Standard displayImage for regular QLabel ---
    static void displayImageStandard(const cv::Mat& img, QLabel* label) {
        if (img.empty()) return;

        cv::Mat rgbMat;
        if (img.channels() == 3)
            cv::cvtColor(img, rgbMat, cv::COLOR_BGR2RGB);
        else
            cv::cvtColor(img, rgbMat, cv::COLOR_GRAY2RGB);

        QImage qImg(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888);
        label->setPixmap(
            QPixmap::fromImage(qImg)
                .scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
};

#endif // UTILS_H
