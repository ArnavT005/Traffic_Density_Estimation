#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <pthread.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <math.h>
using namespace std;  // for standard library constructs
using namespace cv;   // for opencv library constructs
using namespace chrono; // for time measurement constructs

cv::Mat subImg(cv::Mat frame1, cv::Mat frame2, int thr = 20) {

    cv::Mat diff, thresh;

    //grayscale
    cv::cvtColor(frame1, frame1, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame1, frame1, cv::Size(5, 5), 0);

    //grayscale
    cv::cvtColor(frame2, frame2, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(frame2, frame2, cv::Size(5, 5), 0);

    //absolute frame diff
    cv::absdiff(frame1, frame2, diff);

    //threshold
    cv::threshold(diff, thresh, thr, 255, cv::THRESH_BINARY);

    // DILATE & ERODE
    cv::Mat kernel5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat kernel21x21 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21));

    cv::erode(thresh, thresh, kernel5x5);
    cv::dilate(thresh, thresh, kernel21x21);

    return thresh;

}

float findArea(cv::Mat thresh) {
    float area = 0;
    for (int i = 0; i < thresh.rows; i++) {
        for (int j = 0; j < thresh.cols; j++) {
            if (thresh.at<uchar>(i, j) != 0)
                area = area + 1;
        }
    }
    return area;
}

cv::Mat warpAndCrop(cv::Mat image, cv::Mat matrix) {
    cv::Size size = image.size();
    cv::Mat transformed_image, cropped_image;
    //warp image using matrix
    cv::warpPerspective(image, transformed_image, matrix, size);
    //crop using preselected points
    cropped_image = transformed_image(cv::Range(52, 831), cv::Range(472, 801));
    return cropped_image;
}
