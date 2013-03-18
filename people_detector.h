#ifndef PEOPLE_DETECTOR_H
#define PEOPLE_DETECTOR_H

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "image_holder.h"
#include "tracking_person.h"

class PeopleDetector
{
public:
    PeopleDetector();
    void Init();
    void Detect(const ImageHolder &image_holder, std::vector<TrackingPerson> &tracking_people);

private:
    cv::HOGDescriptor hog_;

    const int HOG_MARGIN_WIDTH = 15;
    const int HOG_IMAGE_WIDTH = 72;

    const int FEATURE_MAXIMUM_NUM = 500;
    const double FEATURE_QUALITY = 0.01;
    const int FEATURE_MINIMUM_DISTANCE = 1;

    const double OVERLAP_THRESHOLD = 0.1;

    const int TERMCRIT_MAX_COUNT = 20;
    const double TERMCRIT_EPSILON = 0.3;

    void ExpandRoIRectForHoG(cv::Rect &rect, const cv::Mat &frame);
    cv::Mat ResizeFrameForHoG(const cv::Mat &image, cv::Rect &rect);
    void JustifyPersonRect(cv::Rect &rect, const cv::Rect &roi_rect);
};

#endif // PEOPLE_DETECTOR_H
