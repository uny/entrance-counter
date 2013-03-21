#ifndef PEOPLE_DETECTOR_H
#define PEOPLE_DETECTOR_H

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "image_holder.h"
#include "tracking_person.h"

typedef enum {
    PD_DEBUG_DETECT,
} PD_DEBUG_ENUM;

class PeopleDetector
{
public:
    PeopleDetector();
    void Init();
    void Detect(const ImageHolder &image_holder, std::vector<TrackingPerson> &tracking_people);
    void DrawForDebug(cv::Mat &drawn, PD_DEBUG_ENUM type);

private:
    cv::HOGDescriptor hog_;

    static const int HOG_MARGIN_WIDTH = 15;
    static const int HOG_IMAGE_WIDTH = 72;

    static const int FEATURE_MAXIMUM_NUM = 500;
    // will be divided by 100
    static const int FEATURE_QUALITY = 1;
    static const int FEATURE_MINIMUM_DISTANCE = 1;

    // will be divided by 10
    static const int OVERLAP_THRESHOLD = 1;

    static const int FAST_THRESHOLD = 5;

    static const int TERMCRIT_MAX_COUNT = 20;
    // will be divided by 10
    static const int TERMCRIT_EPSILON = 3;

    void ExpandRoIRectForHoG(cv::Rect &rect, const cv::Mat &frame);
    cv::Mat ResizeFrameForHoG(const cv::Mat &image, const cv::Rect &rect);
    void JustifyPersonRect(cv::Rect &rect, const cv::Rect &roi_rect);
};

#endif // PEOPLE_DETECTOR_H
