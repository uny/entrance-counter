#ifndef PEOPLE_DETECTOR_H
#define PEOPLE_DETECTOR_H

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "tracking_person.h"

class PeopleDetector
{
public:
    PeopleDetector();
    void Init();
    void Detect(const cv::Mat &frame, std::vector<TrackingPerson> &tracking_people);

private:
    cv::Ptr<cv::BackgroundSubtractorGMG> fgbg_;
    cv::HOGDescriptor hog_;
    const int MINIMUM_AREA = 300;
    const int GMG_INIT_FRAME_NUM = 20;
    const double GMG_THRESHOLD = 0.7;

    const int HOG_MARGIN_WIDTH = 15;
    const int HOG_IMAGE_WIDTH = 72;

    const int FEATURE_MAXIMUM_NUM = 500;
    const double FEATURE_QUALITY = 0.01;
    const int FEATURE_MINIMUM_DISTANCE = 1;

    const double OVERLAP_THRESHOLD = 0.2;

    void ExpandRoIRectForHoG(cv::Rect &rect, const cv::Mat &frame);
    cv::Mat ResizeFrameForHoG(const cv::Mat &image, cv::Rect &rect);
    void JustifyPersonRect(cv::Rect &rect, const cv::Rect &roi_rect);
    void JustifyFeaturePoint(cv::Point &point, const cv::Rect &roi_rect);
};

#endif // PEOPLE_DETECTOR_H
