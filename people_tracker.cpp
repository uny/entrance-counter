#include "people_tracker.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

PeopleTracker::PeopleTracker()
{
}

void PeopleTracker::Track(const cv::Mat &frame, std::vector<TrackingPerson> &tracking_people)
{
    std::vector<float> lk_error;
    cv::Rect larger_rect;

    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

    // change track method to fast
    for (int index = 0; index < (int)tracking_people.size(); index++) {
        if (!tracking_people[index].track_points[0].empty()) {
            if (prev_frame_.empty()) {
                frame.copyTo(prev_frame_);
            }
            larger_rect = tracking_people[index].ExpandRectToTrack(frame.size());
            tracking_people[index].JustifyFeaturesPoint(larger_rect.tl(), cv::Point(0, 0), tracking_people[index].TP_JUSTIFY_PREV);
            cv::calcOpticalFlowPyrLK(prev_frame_(larger_rect),
                                     frame(larger_rect),
                                     tracking_people[index].track_points[0],
                                     tracking_people[index].track_points[1],
                                     tracking_people[index].lk_status,
                                     lk_error);
            tracking_people[index].JustifyFeaturesPoint(cv::Point(0, 0), larger_rect.tl(), tracking_people[index].TP_JUSTIFY_BOTH);
            tracking_people[index].MoveRect();

//            cv::goodFeaturesToTrack(frame(tracking_people[index].bounding_rect[1]),
//                                    tracking_people[index].track_points[1],
//                                    FEATURE_MAXIMUM_NUM,
//                                    FEATURE_QUALITY,
//                                    FEATURE_MINIMUM_DISTANCE);
//            cv::cornerSubPix(frame(tracking_people[index].bounding_rect[1]),
//                             tracking_people[index].track_points[1],
//                             cv::Size(10, 10),
//                             cv::Size(-1, -1),
//                             termcrit);
//            tracking_people[index].JustifyFeaturesPoint(cv::Point(0, 0),
//                                                        tracking_people[index].bounding_rect[1].tl(),
//                                                        tracking_people[index].TP_JUSTIFY_NEXT);
        }
    }

    frame.copyTo(prev_frame_);
}
