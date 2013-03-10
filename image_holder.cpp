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
    color = frame;

    // to gray
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    cv::blur(gray, gray, cv::Size(3, 3));
}
