#ifndef TRACKING_PERSON_H
#define TRACKING_PERSON_H

#include <vector>

#include <opencv2/core/core.hpp>

class TrackingPerson
{
public:
    std::vector<cv::Point2f> track_points[2];
    cv::Rect bounding_rect[2];
    int missing_count;
    std::vector<uchar> lk_status;

    typedef enum {
        TP_JUSTIFY_BOTH,
        TP_JUSTIFY_PREV,
        TP_JUSTIFY_NEXT
    }TP_JUSTIFY_ENUM;

    TrackingPerson();
    void JustifyFeaturesPoint(cv::Point origin, TP_JUSTIFY_ENUM select);
};

#endif // TRACKING_PERSON_H
