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
    std::vector<cv::Point> centers;

    typedef enum {
        TP_JUSTIFY_BOTH,
        TP_JUSTIFY_PREV,
        TP_JUSTIFY_NEXT
    }TP_JUSTIFY_ENUM;

    TrackingPerson();
    void JustifyFeaturesPoint(const cv::Point &from_point,
                              const cv::Point &to_point,
                              TP_JUSTIFY_ENUM select);
    void MoveRect();
    void OverwriteLog();
    // wide rectangle of interest for tracking
    cv::Rect ExpandRectToTrack(cv::Size frame_size);

private:
    // margin of roi for tracking
    const int MARGIN_WIDTH = 50;
    const int MINIMUM_FEATURE_MOVE = 1;

    void JustifySelectedFeaturesPoint(std::vector<cv::Point2f> &features,
                                      const cv::Point &from_point,
                                      const cv::Point &to_point);
};

#endif // TRACKING_PERSON_H
