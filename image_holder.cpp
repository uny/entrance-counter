#include "image_holder.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>

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
    fgbg_->set("decisionThreshold", GMG_THRESHOLD);
}

void ImageHolder::Update(const cv::Mat &frame)
{
    cv::Mat fgmask;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> contour_poly;
    cv::Rect roi_rect;

    color = frame;

    // to gray
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    cv::blur(gray, gray, cv::Size(3, 3));

    (*fgbg_)(frame, fgmask);
    cv::erode(fgmask, fgmask, cv::Mat());
    cv::dilate(fgmask, fgmask, cv::Mat());
    cv::dilate(fgmask, fgmask, cv::Mat());
    cv::erode(fgmask, fgmask, cv::Mat());

    diff_rects.clear();
    cv::findContours(fgmask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (std::vector<cv::Point> contour : contours) {
        if (cv::contourArea(contour) < MINIMUM_AREA) {
            continue;
        }
        cv::approxPolyDP(cv::Mat(contour), contour_poly, cv::arcLength(cv::Mat(contour), true) * 0.02, true);
        roi_rect = cv::boundingRect(cv::Mat(contour_poly));
        diff_rects.push_back(roi_rect);
    }
}
