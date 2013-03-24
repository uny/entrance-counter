#include "image_holder.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

ImageHolder::ImageHolder()
{
}

void ImageHolder::Init()
{
    // prepare for background substraction, ref. bgfg_gmg.cpp
    fgbg_ = cv::Algorithm::create<cv::BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
    if (fgbg_.empty()) {
        std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
        return;
    }
    // TODO: maybe need to tune these parameters
    fgbg_->set("initializationFrames", GMG_INIT_FRAME_NUM);
    fgbg_->set("decisionThreshold", (GMG_THRESHOLD / 10.0));
}

void ImageHolder::Update(const cv::Mat &frame)
{
    int64 start_time;

    cv::Mat fgmask;

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> contour_poly;
    cv::Rect roi_rect;

    color = frame;

    start_time = cv::getTickCount();
    // to gray
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    cv::blur(gray, gray, cv::Size(3, 3));
    std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::flush;

    start_time = cv::getTickCount();
    (*fgbg_)(frame, fgmask);
    std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::flush;
    start_time = cv::getTickCount();
    cv::erode(fgmask, fgmask, cv::Mat());
    cv::dilate(fgmask, fgmask, cv::Mat());
    cv::dilate(fgmask, fgmask, cv::Mat());
    cv::erode(fgmask, fgmask, cv::Mat());
    std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::flush;

    start_time = cv::getTickCount();
    diff = cv::Scalar::all(0);
    gray.copyTo(diff, fgmask);
    std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << ", " << std::flush;

    start_time = cv::getTickCount();
    diff_rects.clear();
    cv::findContours(fgmask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (const std::vector<cv::Point> &contour : contours) {
        if (cv::contourArea(contour) < MINIMUM_AREA) {
            continue;
        }
        cv::approxPolyDP(cv::Mat(contour), contour_poly, cv::arcLength(cv::Mat(contour), true) * 0.02, true);
        roi_rect = cv::boundingRect(cv::Mat(contour_poly));
        diff_rects.push_back(roi_rect);
    }
    std::cout << (cv::getTickCount() - start_time) * 1000 / cv::getTickFrequency() << std::endl;
}
