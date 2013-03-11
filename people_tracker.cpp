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

            tracking_people[p_index].JustifyFeaturesPoint(cv::Point(0, 0), larger_rect.tl(), tracking_people[p_index].TP_JUSTIFY_BOTH);
            tracking_people[p_index].MoveRect();
        }
    }

    gray.copyTo(prev_frame_);
}
