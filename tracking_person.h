#ifndef TRACKING_PERSON_H
#define TRACKING_PERSON_H

#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

typedef enum {
    TP_TRANSITION_PREV,
    TP_TRANSITION_NEXT,
    TP_TRANSITION_BOTH
}TP_TRANSITION_ENUM;

class TrackingPerson
{
public:
    /**
     * @brief track_points
     * 0: previous tracking points
     * 1: current tracking points
     */
    std::vector<cv::Point2f> track_points[2];
    /**
     * @brief track_confidence
     * ratio of size of tracking points between current and detected
     * become less than MINIMUM_TRACK_CONFIDENCE, judge the person has been missed
     */
    double track_confidence;
    cv::Rect bounding_rect[2];
    std::vector<uchar> lk_status;
    std::vector<cv::Point> centroid;

    TrackingPerson();
    void JustifyFeaturesPoint(const cv::Point &from_point,
                              const cv::Point &to_point,
                              TP_TRANSITION_ENUM select);
    bool MoveRect();
    // wide rectangle of interest for tracking
    cv::Rect ExpandRectToTrack(cv::Size frame_size);
    /**
     * @brief InitializeForDetection
     * call after detected
     */
    void InitializeForDetection();
    static void OverwriteLog(std::vector<TrackingPerson> &tracking_people);

private:
    // margin of roi for tracking
    static const int MARGIN_WIDTH = 20;
    // will be divided by 10
    static const int MINIMUM_TRACK_CONFIDENCE = 5;

    void JustifySelectedFeaturesPoint(std::vector<cv::Point2f> &features,
                                      const cv::Point &from_point,
                                      const cv::Point &to_point);
};

#endif // TRACKING_PERSON_H
