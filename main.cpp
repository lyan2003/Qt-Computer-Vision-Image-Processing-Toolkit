#include "mainwindow.h"
#include <QApplication>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // إحنا مش محتاجين نقرأ الصورة هنا في الـ main ونعرضها في نافذة منفصلة
    // لأن المفروض الـ MainWindow هي اللي بتتعامل مع الصور دلوقتى

    /* // الكود ده ممكن تشيليه خالص أو تسيبيه كدة كتعليق:
    cv::Mat img = cv::imread("D:/Lyan/3rd year/2nd term/CV/test/test.jpg");
    if (!img.empty()) {
        // cv::imshow("OpenCV Test Window", img); // السطر ده هو اللي كان بيفتح النافذة الخارجية
    }
    */

    return a.exec();
}
