#include "people_tracker.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

PeopleTracker::PeopleTracker()
{
}

void PeopleTracker::Track(const ImageHolder &image_holder, std::vector<TrackingPerson> &tracking_people)
{
    cv::Mat gray;
    image_holder.gray.copyTo(gray);

    std::vector<float> lk_error;
    cv::Rect larger_rect;
    std::vector<uchar> reverse_lk_status;
    std::vector<cv::Point2f> reverse_track_points;

    // person index for loop
    int p_index = 0;
    // feature index for loop
    int f_index = 0;

    // change track method to fast
    for (p_index = 0; p_index < (int)tracking_people.size(); p_index++) {
        if (!tracking_people[p_index].track_points[0].empty()) {
            if (prev_frame_.empty()) {
                gray.copyTo(prev_frame_);
            }
            larger_rect = tracking_people[p_index].ExpandRectToTrack(gray.size());
            tracking_people[p_index].JustifyFeaturesPoint(larger_rect.tl(), cv::Point(0, 0), tracking_people[p_index].TP_JUSTIFY_PREV);
            cv::calcOpticalFlowPyrLK(prev_frame_(larger_rect),
                                     gray(larger_rect),
                                     tracking_people[p_index].track_points[0],
                                     tracking_people[p_index].track_points[1],
                                     tracking_people[p_index].lk_status,
                                     lk_error);
//            // check by reverse
//            cv::calcOpticalFlowPyrLK(gray(larger_rect),
//                                     prev_frame_(larger_rect),
//                                     tracking_people[p_index].track_points[1],
//                                     reverse_track_points,
//                                     reverse_lk_status,
//                                     lk_error);

//            // set error with reverse check
//            for (f_index = 0; f_index < (int)reverse_lk_status.size(); f_index++) {
//                if (cv::norm(reverse_track_points[f_index] - tracking_people[p_index].track_points[1][f_index]) < TRACK_MINIMUM_NORM) {
//                    tracking_people[p_index].lk_status[f_index] = 0;
//                    continue;
//                }
//            }

            tracking_people[p_index].JustifyFeaturesPoint(cv::Point(0, 0), larger_rect.tl(), tracking_people[p_index].TP_JUSTIFY_BOTH);
            tracking_people[p_index].MoveRect();

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

    gray.copyTo(prev_frame_);
}
